#!/bin/bash
#===============================================================================
#      FILENAME: temp_server.sh
#
#   DESCRIPTION: ---
#         NOTES: ---
#        AUTHOR: leoxiang, leoxiang@tencent.com
#       COMPANY: Tencent Co.Ltd
#      REVISION: 2012-02-08 by leoxiang
#===============================================================================

PATH="$(dirname $0)/lbf/lbf:$PATH"
source lbf_init.sh

############################################################
# User-defined variable
############################################################
# server settings
export var_project_path=$(path::abs_pathname $(path::dirname $0)/..)
export var_log_file="${var_project_path}/log/script.log"
export var_backup_dir="${var_project_path}/backup"
export var_backup_files=(bin conf script)
export var_max_try=30

# mod settings
declare -A var_mod=(["conf"]="./bin/confsvrd ./conf/confsvrd.cfg" 
                    ["proxy"]="./bin/proxysvrd 127.0.0.1 60001 0")

############################################################
# Main Logic
############################################################
function usage 
{
  echo "Usage: $(path::basename $0) [start|stop|restart] [all|$(echo ${!var_mod[@]}] | tr ' ' '|')"
  echo "       $(path::basename $0) [status|checklive|reload] [all|$(echo ${!var_mod[@]}] | tr ' ' '|')"
  #echo "       $(path::basename $0) [backup|rollback]"
  echo "       $(path::basename $0) [clearshm]"
}

function main 
{
  cd ${var_project_path}
  ulimit -s 81920

  case $1 in 
    start)      shift; do_start         ${@};;
    stop)       shift; do_stop          ${@};;
    restart)    shift; do_restart       ${@};;
    status)     shift; do_status        ${@};;
    checklive)  shift; do_checklive     ${@};;
    reload)     shift; do_reload        ${@};;
    clearshm)   shift; do_clearshm      ${@};;
    #backup)     shift; server::backup   ${@};;
    #rollback)   shift; server::rollback ${@};;
    *)          usage;;
  esac
  io::no_output cd -
}

function do_start
{
  util::is_empty $1 && usage && return 1

  case $1 in
    all) 
      for _mod in "${var_mod[@]}"; do
        server::start ${_mod}
      done ;;

    *) server::start ${var_mod[$1]};;
  esac
}

function do_stop
{
  util::is_empty $1 && usage && return 1

  case $1 in
    all) 
      for _mod in "${var_mod[@]}"; do
        server::stop ${_mod}
      done ;;

    *) server::stop ${var_mod[$1]};;
  esac
}

function do_restart 
{
  util::is_empty $1 && usage && return 1

  case $1 in
    all) 
      for _mod in "${var_mod[@]}"; do
        server::restart ${_mod}
      done ;;

    *) server::restart ${var_mod[$1]};;
  esac
}

function do_status 
{
  util::is_empty $1 && usage && return 1

  case $1 in
    all) 
      for _mod in "${var_mod[@]}"; do
        server::status ${_mod}
      done ;;

    *) server::status ${var_mod[$1]};;
  esac
}

function do_checklive 
{
  util::is_empty $1 && usage && return 1

  case $1 in
    all) 
      for _mod in "${var_mod[@]}"; do
        server::checklive ${_mod}
      done ;;

    *) server::checklive ${var_mod[$1]};;
  esac
}

function do_reload 
{
  util::is_empty $1 && usage && return 1

  case $1 in
    all) 
      for _mod in "${var_mod[@]}"; do
        server::reload ${_mod}
      done ;;

    *) server::reload ${var_mod[$1]};;
  esac
}

function do_clearshm 
{
  read -p "clear shm will lose all user data, please confirm [yes/no]: " var_confirm
  [ ${var_confirm} != "yes" ] && return

  for _id in $(ipcs -m | awk '{ if ($6 == 0) { print $2 } }'); do
    sudo ipcrm -m $_id
  done

  ipcs -m
}

############################################################
# MOST OF TIMES YOU DO NOT NEED TO CHANGE THESE
############################################################
#################################
# process-related funcs
function server::is_alive 
{
  io::no_output pgrep -xf "${*}" && return 0 || return 1
}

function server::status 
{
  if server::is_alive "${@}"; then
    echo $(io::green "[$@] is alive")
  else
    echo $(io::red "[$@] is not alive")
  fi
}

function server::start 
{
  ulimit -c unlimited
  sudo sysctl -w kernel.shmmax=4000000000 >/dev/null
  ! server::is_alive "${@}" && eval "${@}"

  for ((_cnt = 1; _cnt < ${var_max_try}; _cnt++)); do
    sleep 0.3
    server::is_alive "${@}" && break
    [ $(( $_cnt % 10)) -eq 0 ] && eval "${@}"
  done

  if [ ! $_cnt -eq ${var_max_try} ]; then
    io::log_info ${var_log_file} $(io::green "[$@] start succeed")
    return 0
  else
    io::log_warn ${var_log_file} $(io::red "[$@] start failed")
    return 1
  fi
}

function server::stop 
{
  server::is_alive "${@}" && pkill -USR2 -xf "${*}"

  for ((_cnt = 1; _cnt < ${var_max_try}; _cnt++)); do
    sleep 0.1
    ! server::is_alive "${@}" && break
  done

  if [ ! $_cnt -eq ${var_max_try} ]; then 
    io::log_info ${var_log_file} $(io::green "[$@] stop succeed")
    return 0
  else
    io::log_warn ${var_log_file} $(io::red "[$@] stop failed")
    return 1
  fi
}

function server::restart
{
  ! server::stop ${@} && return 1
  ! server::start ${@} && return 1
}

function server::checklive 
{
  if ! server::is_alive "${@}"; then
    #server::alarm "$(path::basename $1) running num is ${_process_cur_num}, restart it"
    server::start ${@}
  fi
  return 0
}

function server::reload 
{
  util::is_empty $1 && echo "Usage: ${FUNCNAME} [process_cmd]" && return 1

  pkill -USR1 -xf "${*}"

  echo "success reload config $1"
  return 0
}

#################################
# backup-related funcs
function server::backup
{
  # generate backup dir using current time
  local _base_dir="${var_backup_dir}/$(date +"%Y-%m-%d_%H-%M-%S")"

  # backup each file once a time
  for _src in ${var_backup_files[@]}; do
    echo "starting to backup \"${_src}\""
    local _dest_dir="$(path::dirname ${_base_dir}/${_src})"
    ! path::is_exist ${_dest_dir} && mkdir -p ${_dest_dir}
    cp -rf ${_src} ${_dest_dir}
  done

  # write log
  io::log_info ${var_log_file} "succeed backup files to ${_dest_dir}"
}

function server::rollback 
{
  # output instruct msg
  echo "select which version you want to rollback"

  # restore files
  select _dir in $(ls ${var_backup_dir}); do
    local _dest_dir="${var_backup_dir}/${_dir}"
    echo "you select ${_dest_dir}, these file will be restored: " && find ${_dest_dir} -type f
    cp -rf ${_dest_dir}/* ${var_project_path}
    break
  done

  # write log
  io::log_info ${var_log_file} "succeed restore files from ${_dest_dir}"
}

#################################
# here we start the main logic
main "${@}"

# vim:ts=2:sw=2:et:ft=sh:
