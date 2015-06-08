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

MODULE_DIRS = \
			  lsf/algorithm		\
			  lsf/asio			\
			  lsf/basic			\
			  lsf/meta 			\
			  lsf/container		\
			  lsf/encrypt 		\
			  lsf/util 			\

all : 
	for dir in $(MODULE_DIRS); do \
		make -C $$dir; \
	done

clean : 
	for dir in $(MODULE_DIRS); do \
		make clean -C $$dir; \
	done

test : 				\
	test_asio 		\
	test_basic 		\
	test_meta 		\
	test_container 	\
	test_encrypt 	\
	test_util 		\

test_asio :
	./bin/test_ip
	./bin/test_tcp
	./bin/test_udp

test_basic :
	./bin/test_basic_array
	./bin/test_buffer
	./bin/test_string_ext
	./bin/test_type_cast

test_meta :
	./bin/test_macro
	./bin/test_type_list
	./bin/test_type_traits

test_container :
	./bin/test_array
	./bin/test_benchmark ./conf/test_rb_tree1.conf
	./bin/test_benchmark ./conf/test_rb_tree2.conf
	./bin/test_list
	./bin/test_map
	./bin/test_pool
	./bin/test_queue
	./bin/test_rb_tree
	./bin/test_set
	./bin/test_variant
	./bin/test_basic_container
	./bin/test_shared_mem

test_encrypt :
	./bin/test_base64
	./bin/test_md5
	
test_util :
	./bin/test_config ./conf/test_config.conf
	./bin/test_date
	./bin/test_log
	./bin/test_random
	./bin/test_shared_ptr
	./bin/test_system

.PHONY : all clean test

# vim:ts=4:sw=4:ft=make:
