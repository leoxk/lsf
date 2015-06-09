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

# define targets
BIN = $(BIN_DIR)/tcpsvrd

# define obj
OBJ  = $(patsubst %.cpp, $(DEP_DIR)/%.o, $(wildcard *.cpp))
OBJ += $(patsubst %.cc,  $(DEP_DIR)/%.o, $(wildcard *.cc))

# main targets
all: $(BIN)

$(BIN): $(OBJ) $(LIB)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -o $@ $^ $(LD_FLAGS) 

clean:
	$(RM) $(BIN) $(DEP_DIR)

.PHONY: all clean

# default dependency rule
-include  $(PROJECT_DIR)/Makefile.rule


# vim:ts=4:sw=4:ft=make:
