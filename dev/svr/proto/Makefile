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
#      REVISION: 2015-05-28 by leoxiang
#===============================================================================#

PROJECT_DIR := ../../..
-include $(PROJECT_DIR)/Makefile.shared

LIB = $(LIB_DIR)/libproto.a

OBJ = \
		 $(DEP_DIR)/conf_deploy.o	\
		 $(DEP_DIR)/conf_game.o		\
		 $(DEP_DIR)/data_player.o	\
		 $(DEP_DIR)/data_mem.o		\
		 $(DEP_DIR)/msg_base.o		\
		 $(DEP_DIR)/msg_cs.o		\
		 $(DEP_DIR)/msg_ss.o		\

$(LIB) :  $(OBJ) 
	$(AR) $@ $^
	
$(OBJ) :$(DEP_DIR)/%.o : %.proto
	protoc $< --cpp_out=./
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -c -o $@ $(patsubst %.proto, %.pb.cc, $<)

clean:
	$(RM) $(LIB) $(DEP_DIR) 
	$(RM) $(patsubst $(DEP_DIR)/%.o, %.pb.cc, $(OBJ))
	$(RM) $(patsubst $(DEP_DIR)/%.o, %.pb.h,  $(OBJ))

.PHONY: clean

# default dependency rule
-include  $(PROJECT_DIR)/Makefile.rule

# vim:ts=4:sw=4:
