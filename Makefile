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
#      REVISION: 2012-08-15 by leoxiang
#===============================================================================#

MODULE_DIRS = \
			  dev/test/algorithm		\
			  dev/test/asio				\
			  dev/test/basic			\
			  dev/test/meta 			\
			  dev/test/container		\
			  dev/test/encrypt 			\
			  dev/test/util 			\

all : 
	for dir in $(MODULE_DIRS); do \
		make -C $$dir; \
	done

test : 				\
	test_asio 		\
	test_basic 		\
	test_meta 		\
	test_container 	\
	test_encrypt 	\
	test_util 		\

test_asio :
	./release/bin/test_ip
	./release/bin/test_tcp
	./release/bin/test_udp

test_basic :
	./release/bin/test_basic_array
	./release/bin/test_buffer
	./release/bin/test_string_ext
	./release/bin/test_type_cast

test_meta :
	./release/bin/test_macro
	./release/bin/test_type_list
	./release/bin/test_type_traits

test_container :
	./release/bin/test_array
	./release/bin/test_benchmark release/conf/test_rb_tree1.conf
	./release/bin/test_benchmark release/conf/test_rb_tree2.conf
	./release/bin/test_list
	./release/bin/test_map
	./release/bin/test_pool
	./release/bin/test_queue
	./release/bin/test_rb_tree
	./release/bin/test_set
	./release/bin/test_variant
	./release/bin/test_basic_container
	./release/bin/test_shared_mem

test_encrypt :
	./release/bin/test_base64
	./release/bin/test_md5
	
test_util :
	./release/bin/test_config release/conf/test_config.conf
	./release/bin/test_date
	./release/bin/test_log
	./release/bin/test_random
	./release/bin/test_shared_ptr
	./release/bin/test_system

clean : 
	for dir in $(MODULE_DIRS); do \
		make clean -C $$dir; \
	done

.PHONY : all clean test


# vim:ts=4:sw=4:ft=make:
