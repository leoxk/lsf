#===============================================================================
#      FILENAME: Makefile
#         USAGE: make            :generate executable
#                make clean      :remove objects, executable, prerequisits
#
#   DESCRIPTION: ---
#         NOTES: Makefile.shared - define project configuration
#                Makefile.rule  - auto-generate dependencies for c/c++ files
#                Remember to inlcude Makefile.rule after all your targets!
#        AUTHOR: leoxiang, leoxiang727@qq.com
#       COMPANY: 
#      REVISION: 2012-08-15 by leoxiang
#===============================================================================#

MODULE_DIRS = \
			  dev 	\
			  test 	\

all : 
	for dir in $(MODULE_DIRS); do \
		make -C $$dir; \
	done

clean : 
	for dir in $(MODULE_DIRS); do \
		make clean -C $$dir; \
	done

test : 
	for dir in $(MODULE_DIRS); do \
		make test -C $$dir; \
	done

.PHONY : all clean test


# vim:ts=4:sw=4:ft=make:
