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

PROJECT_DIR := .
-include $(PROJECT_DIR)/Makefile.shared

############################################################
# define modules
############################################################
SVR_MODULE = \
			  dev/svr/proto 		\
			  dev/svr/common 		\
			  dev/svr/confsvrd		\
			  dev/svr/datasvrd		\
			  dev/svr/gamesvrd		\
			  dev/svr/proxysvrd		\
			  dev/svr/connsvrd		\
			  dev/svr/httpsvrd  	\
			  dev/svr/test 			\

TEST_MODULE = \
			  dev/lsf/test/basic		\
			  dev/lsf/test/algorithm	\
			  dev/lsf/test/asio			\
			  dev/lsf/test/container	\
			  dev/lsf/test/util 		\

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
	test_basic 		\
	test_asio 		\
	test_container 	\
	test_util 		\
	test_algorithm  \

test_clean :
	for dir in $(TEST_MODULE); do \
		make clean -C $$dir; \
	done

test_compile:
	for dir in $(TEST_MODULE); do \
		make -C $$dir; \
	done

test_algorithm :
	@./test/bin/test_combination
	@./test/bin/test_two_dimensional_table ./test/conf/test_two_dimensional_table.csv

test_basic :
	@./test/bin/test_scope_exit
	@./test/bin/test_type_cast
	@./test/bin/test_type_traits
	@./test/bin/test_any

test_asio :
	@./test/bin/test_address
	@./test/bin/test_sockaddr
	@./test/bin/test_socket
	@./test/bin/test_async
	@./test/bin/test_curl

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

test_util :
	@./test/bin/test_config ./test/conf/test_config.conf
	@./test/bin/test_date
	@./test/bin/test_log
	@./test/bin/test_random
	@./test/bin/test_system
	@./test/bin/test_backtrace
	@./test/bin/test_string_ext
	@./test/bin/test_file_lock
	@./test/bin/test_locale
	@./test/bin/test_words_filter

table :
	for _file in $$(ls $(TABLE_DIR)/*.xlsx); do \
		xlsx2csv $$_file $$(dirname $$_file)/$$(basename $$_file .xlsx).csv; \
	done

.PHONY : all clean test test_clean check table1 table2

# vim:ts=4:sw=4:ft=make:
