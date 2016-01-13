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
BIN = $(BIN_DIR)/confsvrd

# define obj
OBJ  = $(patsubst %.cpp, $(DEP_DIR)/%.o, $(wildcard *.cpp))
OBJ += $(patsubst %.cc,  $(DEP_DIR)/%.o, $(wildcard *.cc))

# main targets
all: $(BIN) $(CONF_DIR)/conf_game.proto

$(BIN): $(OBJ) $(DEP_LIB)
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -o $@ $^ $(LD_FLAGS)

$(CONF_DIR)/% : $(DEV_DIR)/svr/proto/%
	$(CP) $^ $@

clean:
	$(RM) $(BIN) $(DEP_DIR)

.PHONY: all clean

# default dependency rule
-include  $(PROJECT_DIR)/Makefile.rule

# vim:ts=4:sw=4:ft=make:
