#===============================================================================
#      FILENAME: Makefile
#         USAGE: make            :generate executable
#                make clean      :remove objects, executable, prerequisits
#
#   DESCRIPTION: ---
#         NOTES: config.mk - define project configuration
#                build.mk  - auto-generate dependencies for c/c++ files
#                Remember to inlcude build.mk after all your targets!
#        AUTHOR: leoxiang, leoxiang@tencent.com
#       COMPANY: Tencent Co.Ltd
#      REVISION: 2012-02-28 by leoxiang
#===============================================================================#

TEST_FILES   := $(wildcard ./test/*.sh)

test:
	@chmod a+x $(TEST_FILES)
	@for _test in $(TEST_FILES); do echo "start to run $$_test"; ./$$_test | grep --color -E "Failed|$$" ; done || exit 0

.PHONY: test

# vim:ts=4:sw=4:ft=make:
