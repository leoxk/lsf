#===============================================================================
#      FILENAME: Makefile
#         USAGE: make            :generate executable
#                make clean      :remove objects, executable, prerequisits
#
#   DESCRIPTION: ---
#         NOTES: Makefile.shared - define project configuration
#                Makefile.rule  - auto-generate dependencies for c/c++ files
#                Remember to inlcude build.mk after all your targets!
#        AUTHOR: leoxiang, leoxiang727@qq.com
#       COMPANY: 
#      REVISION: 2015-06-08 by leoxiang
#===============================================================================#

MODULE_DIRS = \
			  svr/proto 		\
			  svr/common 		\
			  svr/tcpsvrd/src 	\

all : 
	for dir in $(MODULE_DIRS); do \
		make -C $$dir; \
	done

clean : 
	for dir in $(MODULE_DIRS); do \
		make clean -C $$dir; \
	done

.PHONY : all clean

# vim:ts=4:sw=4:ft=make:
