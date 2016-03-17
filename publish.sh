#!/bin/bash
#===============================================================================
#      FILENAME: publish.sh
#
#   DESCRIPTION: ---
#         NOTES: ---
#        AUTHOR: leoxiang
#      REVISION: 2014-12-31 by leoxiang
#===============================================================================

PATH="$(dirname $0)/release/script/lbf:$PATH"
source lbf_init.sh

# init vars
var_pack_list=""
var_dir="../texas_versions/server"

# check usage
function usage
{
  echo "./publish [version]"
  exit
}
util::is_empty $1 && usage

# output update content
echo "please select which to pack: "
echo "1)  confsvrd"
echo "2)  connsvrd"
echo "3)  datasvrd"
echo "4)  gamesvrd"
echo "5)  gmsvrd"
echo "6)  httpsvrd"
echo "7)  logsvrd"
echo "8)  proxysvrd"
echo "9)  roomsvrd"
echo "10) script"
echo "11) config"
echo "12) table"
echo "13) matchsvrd"
read -p "please select which to pack: " var_select_list

# init update list
for _var_select in ${var_select_list}; do
  case ${_var_select} in
    1)  var_pack_list+=" ./release/bin/confsvrd"  ;;
    2)  var_pack_list+=" ./release/bin/connsvrd"  ;;
    3)  var_pack_list+=" ./release/bin/datasvrd"  ;;
    4)  var_pack_list+=" ./release/bin/gamesvrd"  ;;
    5)  var_pack_list+=" ./release/bin/gmsvrd"    ;;
    6)  var_pack_list+=" ./release/bin/httpsvrd"  ;;
    7)  var_pack_list+=" ./release/bin/logsvrd"   ;;
    8)  var_pack_list+=" ./release/bin/proxysvrd" ;;
    9)  var_pack_list+=" ./release/bin/roomsvrd"  ;;
    10) var_pack_list+=" ./release/script/server.sh"
        var_pack_list+=" ./release/script/crossdomain.xml"
        var_pack_list+=" ./release/script/flashd.py"
        var_pack_list+=" ./release/script/lbf"
        ;;
    11) var_pack_list+=" ./release/conf/deploy.cfg"
        ;;
    12) var_pack_list+=" ./release/conf/conf_game.proto"
        var_pack_list+=" ./release/table/*.csv"
        ;;
    13) var_pack_list+=" ./release/bin/matchsvrd" ;;
    *)  echo "unknown tpye ${_var_select}"; exit 0;;
  esac
done

# proccess
echo "======================"
echo "[begin clear]"
svn up  ${var_dir} --accept theirs-full
svn revert ${var_dir} -R

echo "======================"
echo "[begin update]"
for _file in ${var_pack_list}; do
  echo ${_file} $(path::dirname ${_file})
  _dir=../texas_versions/server/$(path::dirname ${_file})
  path::is_exist ${_dir} || mkdir -p ${_dir}
  cp -fr ${_file} ${_dir}
done

echo "======================"
echo "[upload svn]"
svn st ${var_dir} | grep "^?" | while read _line; do
  _file=$(svn st ${_line} | grep ? | awk '{print $2}')
  echo svn add ${_file}
  svn add ${_file}
done
svn ci ${var_dir} -m "texas package ${1} at $(date '+%Y%m%d%H%M%S')"
svn info ${var_dir}/release

# vim:ts=2:sw=2:et:
