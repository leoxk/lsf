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
#      REVISION: 2012-08-15 by leoxiang
#===============================================================================#

MODULE_DIRS = \
			  dev/test/algorithm		\
			  dev/test/asio				\
			  dev/test/basic			\
			  dev/test/container		\
			  dev/test/encrypt 			\
			  dev/test/util 			\
			  dev/test/meta 			\

all : 
	for dir in $(MODULE_DIRS); do \
		make -C $$dir; \
	done

test : 				\
	test_asio 		\
	test_basic 		\
	test_container 	\
	test_encrypt 	\
	test_meta 		\
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

test_container :
	./release/bin/test_array
	#./release/bin/test_benchmark release/conf/test_rb_tree1.conf
	#./release/bin/test_benchmark release/conf/test_rb_tree2.conf
	./release/bin/test_list
	./release/bin/test_map
	./release/bin/test_pool
	./release/bin/test_queue
	./release/bin/test_rb_tree
	./release/bin/test_set
	./release/bin/test_variant

test_encrypt :
	./release/bin/test_base64
	./release/bin/test_md5
	
test_meta :
	./release/bin/test_macro
	./release/bin/test_type_list
	./release/bin/test_type_traits

test_util :
	./release/bin/test_config release/conf/test_config.conf
	./release/bin/test_date
	./release/bin/test_log
	./release/bin/test_random
	./release/bin/test_shared_mem
	./release/bin/test_shared_ptr
	./release/bin/test_system

clean : 
	for dir in $(MODULE_DIRS); do \
		make clean -C $$dir; \
	done

.PHONY : all clean test


# vim:ts=4:sw=4:ft=make:
