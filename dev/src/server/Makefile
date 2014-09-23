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
#      REVISION: 2012-10-18 by leoxiang
#===============================================================================#

PROJECT_DIR := ../../..
-include $(PROJECT_DIR)/Makefile.shared
				  
############################################################
# Target Specify
############################################################
# define targets
LIB = $(LIB_DIR)/liblsfserver.a

HEADER = $(DEV_DIR)/lsf/server/detail/srv_framework.h \
		 $(DEV_DIR)/lsf/server/detail/oi_log.h 	      \

OBJ = \
	  oi_log.o 					\
	  oi_misc.o					\
	  strtime.o					\
	  oi_str.o					\
	  oi_misc.o					\
	  multi_file_processing.o 	\
	  srv_framework.o 			\

export CFLAGS += -DMFP_EPOLL

# main targets
all: $(LIB) $(HEADER)

$(LIB): $(OBJ)
	$(AR) $@ $^

$(HEADER): $(DEV_DIR)/lsf/server/detail/% : %
	$(CP) $< $@

clean:
	$(RM) $(LIB) $(HEADER) *.o *.d

.PHONY: all clean

############################################################
# Rule Specify
############################################################
-include $(PROJECT_DIR)/Makefile.rule

# vim:ts=4:sw=4:ft=make:
