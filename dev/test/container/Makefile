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
#      REVISION: 2011-09-13 by leoxiang
#===============================================================================#

PROJECT_DIR := ../../..
-include $(PROJECT_DIR)/Makefile.shared

# define targets
BIN = \
	  $(BIN_DIR)/test_array 		\
	  $(BIN_DIR)/test_list 			\
	  $(BIN_DIR)/test_pool 			\
	  $(BIN_DIR)/test_queue 		\
	  $(BIN_DIR)/test_set			\
	  $(BIN_DIR)/test_map			\
	  $(BIN_DIR)/test_rb_tree		\
	  $(BIN_DIR)/test_benchmark 	\
	  $(BIN_DIR)/test_variant 		\

# main targets
all: $(BIN)

$(BIN): $(BIN_DIR)/%: %.o
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -o $@ $^

clean:
	$(RM) $(BIN) *.o *.d

.PHONY: all clean

# default dependency rule
-include  $(PROJECT_DIR)/Makefile.rule

# vim:ts=4:sw=4:ft=make:
