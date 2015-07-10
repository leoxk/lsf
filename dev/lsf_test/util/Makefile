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
#      REVISION: 2011-09-13 by leoxiang
#===============================================================================#

PROJECT_DIR := ../../..
-include $(PROJECT_DIR)/Makefile.shared

# define targets
BIN = \
	  $(TEST_BIN_DIR)/test_config			\
	  $(TEST_BIN_DIR)/test_date				\
	  $(TEST_BIN_DIR)/test_log				\
	  $(TEST_BIN_DIR)/test_random 	 		\
	  $(TEST_BIN_DIR)/test_system 	 		\
	  $(TEST_BIN_DIR)/test_file_lock 		\
	  $(TEST_BIN_DIR)/test_backtrace 		\
	  $(TEST_BIN_DIR)/test_string_ext 		\

# main targets
all: $(BIN)

$(BIN): $(TEST_BIN_DIR)/%: $(DEP_DIR)/%.o
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -o $@ $^

clean:
	$(RM) $(BIN) $(DEP_DIR)

.PHONY: all clean

# default dependency rule
-include  $(PROJECT_DIR)/Makefile.rule

# vim:ts=4:sw=4:ft=make:
