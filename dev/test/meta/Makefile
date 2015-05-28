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
	  $(BIN_DIR)/test_macro 			\
	  $(BIN_DIR)/test_type_traits		\
	  $(BIN_DIR)/test_type_list 		\

# main targets
all: $(BIN)

$(BIN): $(BIN_DIR)/%: $(DEP_DIR)/%.o
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -o $@ $^

clean:
	$(RM) $(BIN) $(DEP_DIR)

.PHONY: all clean

# default dependency rule
-include  $(PROJECT_DIR)/Makefile.rule

# vim:ts=4:sw=4:ft=make:
