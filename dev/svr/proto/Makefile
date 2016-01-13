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
		 $(DEP_DIR)/conf_deploy.pb.o	\
		 $(DEP_DIR)/conf_game.pb.o		\
		 $(DEP_DIR)/msg_cs.pb.o			\
		 $(DEP_DIR)/msg_ss.pb.o			\
		 $(DEP_DIR)/data_db.pb.o		\
		 $(DEP_DIR)/data_mem.pb.o		\


$(LIB) : $(OBJ)
	$(AR) $@ $^

$(OBJ) : $(DEP_DIR)/%.o : %.cc $(patsubst $(DEP_DIR)/%.o, %.h, $(OBJ))
	$(CXX) $(CXXFLAGS) $(INC_FLAGS) -c -o $@ $<


%.pb.cc : %.proto
	protoc $< --cpp_out=./

%.pb.h : %.proto
	protoc $< --cpp_out=./

clean:
	$(RM) $(LIB) $(DEP_DIR)
	$(RM) $(patsubst $(DEP_DIR)/%.o, %.cc, $(OBJ))
	$(RM) $(patsubst $(DEP_DIR)/%.o, %.h,  $(OBJ))

.PHONY: clean

# vim:ts=4:sw=4:
