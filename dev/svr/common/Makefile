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

PROJECT_DIR := ../../..
-include $(PROJECT_DIR)/Makefile.shared

LIB = $(LIB_DIR)/libcommon.a

OBJ = $(patsubst %.cpp, $(DEP_DIR)/%.o, $(wildcard *.cpp))
OBJ += $(patsubst %.c, $(DEP_DIR)/%.o, $(wildcard *.c))

$(LIB) :  $(OBJ) 
	$(AR) $@ $^
	
clean:
	$(RM) $(LIB) $(DEP_DIR) 

.PHONY: clean

# default dependency rule
-include  $(PROJECT_DIR)/Makefile.rule

# vim:ts=4:sw=4:ft=make:
