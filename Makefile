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

############################################################
# define modules
############################################################
SVR_MODULE = \
			  dev/svr/proto 	\
			  dev/svr/common 	\
			  dev/svr/confsvrd	\
			  dev/svr/proxysvrd	\
			  dev/svr/connsvrd	\
			  dev/svr/gamesvrd	\

TEST_MODULE = \
			  dev/lsf_test/algorithm	\
			  dev/lsf_test/asio			\
			  dev/lsf_test/container	\
			  dev/lsf_test/encrypt 		\
			  dev/lsf_test/util 		\

############################################################
# for svr
############################################################
all :
	for dir in $(SVR_MODULE); do \
		make -C $$dir; \
	done

clean :
	for dir in $(SVR_MODULE); do \
		make clean -C $$dir; \
	done

############################################################
# for test
############################################################
test : 				\
	test_compile 	\
	test_asio 		\
	test_container 	\
	test_encrypt 	\
	test_util 		\

test_clean :
	for dir in $(TEST_MODULE); do \
		make clean -C $$dir; \
	done

test_compile:
	for dir in $(TEST_MODULE); do \
		make -C $$dir; \
	done

test_asio :
	@./test/bin/test_net
	@./test/bin/test_tcp
	@./test/bin/test_udp
	@./test/bin/test_asio

test_meta :
	@./test/bin/test_macro
	@./test/bin/test_type_list
	@./test/bin/test_type_traits

test_container :
	@./test/bin/test_array
	@./test/bin/test_benchmark ./test/conf/test_rb_tree1.conf
	@./test/bin/test_benchmark ./test/conf/test_rb_tree2.conf
	@./test/bin/test_list
	@./test/bin/test_map
	@./test/bin/test_pool
	@./test/bin/test_queue
	@./test/bin/test_rb_tree
	@./test/bin/test_set
	@./test/bin/test_basic_container
	@./test/bin/test_shared_mem

test_encrypt :
	@./test/bin/test_base64
	@./test/bin/test_md5

test_util :
	@./test/bin/test_config ./test/conf/test_config.conf
	@./test/bin/test_date
	@./test/bin/test_log
	@./test/bin/test_random
	@./test/bin/test_system
	@./test/bin/test_backtrace
	@./test/bin/test_string_ext
	@./test/bin/test_type_cast

.PHONY : all clean test test_clean check


# vim:ts=4:sw=4:ft=make:
