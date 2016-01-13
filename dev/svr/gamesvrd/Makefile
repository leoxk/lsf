#===============================================================================
#      FILENAME: Makefile
#   DESCRIPTION: ---
#         NOTES: Makefile.shared - define project configuration
#                Makefile.rule  - auto-generate dependencies for c/c++ files
#                Remember to inlcude build.mk after all your targets!
#        AUTHOR: leoxiang, leoxiang727@qq.com
#      REVISION: 2015-06-25 by leoxiang
#===============================================================================

PROJECT_DIR := ../../..
-include $(PROJECT_DIR)/Makefile.shared

# define targets
BIN = $(BIN_DIR)/gamesvrd

# define obj
BIN_OBJ = $(patsubst %.cpp, $(DEP_DIR)/%.o, $(wildcard *.cpp))

# main targets
all: $(BIN)

$(BIN): $(BIN_OBJ) $(DEP_LIB)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -o $@ $^ $(LD_FLAGS)

clean:
	$(RM) $(BIN) $(DEP_DIR)

.PHONY: all clean

# default dependency rule
-include  $(PROJECT_DIR)/Makefile.rule

# vim:ts=4:sw=4:ft=make:
