# Makefile 
# 

.PHONY: all
.SUFFIXES:.cpp .c .ro

# make_distrib.pl substitutes the correct version
VERSION=1.2.0

# boost_thread lib used by test application. 
# You may change the library name
LD_BOOST_THREAD_LIB=-lboost_thread

BASE_OPT = -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS -D_FILE_OFFSET_BITS=64 -I..
COMP_OPT = -c $(CFLAGS) $(BASE_OPT)
CPP_COMP_OPT = -std=c++0x -c $(CXXFLAGS) $(BASE_OPT)
#-fno-enforce-eh-specs

#LD_LIBS = -lpthread -ldl -lstdc++ 

COMPILER_ROOT = $(shell dirname `dirname \`which 	$(CXX)\``)

LD_OPTS = $(LDFLAGS)



####################################
# cds library

CDS_OBJS=$(OBJ_PATH)/hzp_gc.o \
         $(OBJ_PATH)/init.o \
         $(OBJ_PATH)/hrc_gc.o \
         $(OBJ_PATH)/ptb_gc.o \
         $(OBJ_PATH)/michael_heap.o \
         $(OBJ_PATH)/topology_hpux.o \
         $(OBJ_PATH)/topology_linux.o

CDS_DEBUG_LIB=libcds-debug.so
CDS_RELEASE_LIB=libcds.so

$(OBJ_PATH)/hzp_gc.o: ../src/hzp_gc.cpp
	$(CXX) $(CPP_COMP_OPT) -o $@ $<
$(OBJ_PATH)/hrc_gc.o: ../src/hrc_gc.cpp
	$(CXX) $(CPP_COMP_OPT) -o $@ $<
$(OBJ_PATH)/ptb_gc.o: ../src/ptb_gc.cpp
	$(CXX) $(CPP_COMP_OPT) -o $@ $<
$(OBJ_PATH)/init.o: ../src/init.cpp
	$(CXX) $(CPP_COMP_OPT) -o $@ $<
$(OBJ_PATH)/michael_heap.o: ../src/michael_heap.cpp
	$(CXX) $(CPP_COMP_OPT) -o $@ $<
$(OBJ_PATH)/topology_hpux.o: ../src/topology_hpux.cpp
	$(CXX) $(CPP_COMP_OPT) -o $@ $<
$(OBJ_PATH)/topology_linux.o: ../src/topology_linux.cpp
	$(CXX) $(CPP_COMP_OPT) -o $@ $<

$(CDS_DEBUG_LIB).$(VERSION) : $(CDS_OBJS)
	$(CXX) $(LD_OPTS) $(CDS_OBJS) -o $@ 
	mv ./$(CDS_DEBUG_LIB).$(VERSION) $(BIN_PATH)/$(CDS_DEBUG_LIB).$(VERSION)
	ln -sf $(CDS_DEBUG_LIB).$(VERSION) $(BIN_PATH)/$(CDS_DEBUG_LIB)

$(CDS_RELEASE_LIB).$(VERSION) : $(CDS_OBJS)
	$(CXX) $(LD_OPTS) $(CDS_OBJS) -o $@ 
	mv ./$(CDS_RELEASE_LIB).$(VERSION) $(BIN_PATH)/$(CDS_RELEASE_LIB).$(VERSION)
	ln -sf $(CDS_RELEASE_LIB).$(VERSION) $(BIN_PATH)/$(CDS_RELEASE_LIB)

debug : $(CDS_DEBUG_LIB).$(VERSION)
release : $(CDS_RELEASE_LIB).$(VERSION)

all: debug release

##########################################
# Make tests

OBJ_TEST_PATH=$(OBJ_PATH)

TEST_COMMONHDR_SRC_DIR=../tests
TEST_COMMON_OBJS=$(TEST_COMMONHDR_SRC_DIR)/cppunit/test_main.o \
    $(TEST_COMMONHDR_SRC_DIR)/cppunit/thread.o

$(TEST_COMMON_OBJS): %.o: %.cpp
	$(CXX) $(CPP_COMP_OPT) -I$(TEST_COMMONHDR_SRC_DIR) $< -o $@

TESTHDR_SRC_DIR=../tests/test-hdr
TESTHDR_OBJS= \
    $(TESTHDR_SRC_DIR)/misc/cxx11_atomic_class.o \
    $(TESTHDR_SRC_DIR)/misc/cxx11_atomic_func.o \
    $(TESTHDR_SRC_DIR)/misc/allocator_test.o \
    $(TESTHDR_SRC_DIR)/misc/michael_allocator.o \
    $(TESTHDR_SRC_DIR)/deque/hdr_michael_deque_hp.o \
    $(TESTHDR_SRC_DIR)/deque/hdr_michael_deque_ptb.o \
    $(TESTHDR_SRC_DIR)/map/hdr_intrusive_michael_set_hrc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_intrusive_michael_set_hrc_lazy.o \
    $(TESTHDR_SRC_DIR)/map/hdr_intrusive_splitlist_set_hrc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_intrusive_splitlist_set_hrc_lazy.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_map_hp.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_map_hrc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_map_ptb.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_map_nogc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_map_lazy_hp.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_map_lazy_hrc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_map_lazy_ptb.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_map_lazy_nogc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_set_hp.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_set_hrc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_set_ptb.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_set_nogc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_set_lazy_hp.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_set_lazy_hrc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_set_lazy_ptb.o \
    $(TESTHDR_SRC_DIR)/map/hdr_michael_set_lazy_nogc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_map_hp.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_map_hrc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_map_ptb.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_map_nogc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_map_lazy_hp.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_map_lazy_hrc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_map_lazy_ptb.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_map_lazy_nogc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_set_hp.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_set_hrc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_set_nogc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_set_ptb.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_set_lazy_hp.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_set_lazy_hrc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_set_lazy_nogc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_splitlist_set_lazy_ptb.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_intrusive_lazy_hrc.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_lazy_hp.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_lazy_hrc.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_lazy_nogc.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_lazy_ptb.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_lazy_kv_hp.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_lazy_kv_hrc.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_lazy_kv_nogc.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_lazy_kv_ptb.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_intrusive_michael_hrc.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_michael_hp.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_michael_hrc.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_michael_nogc.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_michael_ptb.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_michael_kv_hp.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_michael_kv_hrc.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_michael_kv_nogc.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_michael_kv_ptb.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_intrusive_basketqueue_hrc.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_intrusive_moirqueue_hrc.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_intrusive_msqueue_hrc.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_intrusive_tsigas_cycle_queue.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_intrusive_vyukovmpmc_cycle_queue.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_basketqueue_hrc.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_basketqueue_hzp.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_basketqueue_ptb.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_moirqueue_hrc.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_moirqueue_hzp.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_moirqueue_ptb.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_msqueue_hrc.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_msqueue_hzp.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_msqueue_ptb.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_optimistic_hzp.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_optimistic_ptb.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_rwqueue.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_vyukov_mpmc_cyclic.o \
    $(TESTHDR_SRC_DIR)/queue/queue_test_header.o \
    $(TESTHDR_SRC_DIR)/stack/hdr_intrusive_treiber_stack_hrc.o \
    $(TESTHDR_SRC_DIR)/stack/hdr_treiber_stack_hp.o \
    $(TESTHDR_SRC_DIR)/stack/hdr_treiber_stack_hrc.o \
    $(TESTHDR_SRC_DIR)/stack/hdr_treiber_stack_ptb.o 

$(TESTHDR_OBJS): %.o: %.cpp
	$(CXX) $(CPP_COMP_OPT) -I$(TESTHDR_SRC_DIR) -I$(TEST_COMMONHDR_SRC_DIR) $< -o $@
	
TESTHDR_OBJS_NO_OFFSETOF_WARN= \
    $(TESTHDR_SRC_DIR)/deque/hdr_intrusive_michael_deque_hp.o \
    $(TESTHDR_SRC_DIR)/deque/hdr_intrusive_michael_deque_ptb.o \
    $(TESTHDR_SRC_DIR)/map/hdr_intrusive_michael_set_hp.o \
    $(TESTHDR_SRC_DIR)/map/hdr_intrusive_michael_set_ptb.o \
    $(TESTHDR_SRC_DIR)/map/hdr_intrusive_michael_set_nogc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_intrusive_michael_set_hp_lazy.o \
    $(TESTHDR_SRC_DIR)/map/hdr_intrusive_michael_set_ptb_lazy.o \
    $(TESTHDR_SRC_DIR)/map/hdr_intrusive_michael_set_nogc_lazy.o \
    $(TESTHDR_SRC_DIR)/map/hdr_intrusive_splitlist_set_hp.o \
    $(TESTHDR_SRC_DIR)/map/hdr_intrusive_splitlist_set_nogc.o \
    $(TESTHDR_SRC_DIR)/map/hdr_intrusive_splitlist_set_ptb.o \
    $(TESTHDR_SRC_DIR)/map/hdr_intrusive_splitlist_set_hp_lazy.o \
    $(TESTHDR_SRC_DIR)/map/hdr_intrusive_splitlist_set_nogc_lazy.o \
    $(TESTHDR_SRC_DIR)/map/hdr_intrusive_splitlist_set_ptb_lazy.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_intrusive_lazy_hp.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_intrusive_lazy_nogc.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_intrusive_lazy_ptb.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_intrusive_michael_hp.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_intrusive_michael_nogc.o \
	$(TESTHDR_SRC_DIR)/ordered_list/hdr_intrusive_michael_ptb.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_intrusive_basketqueue_hp.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_intrusive_basketqueue_ptb.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_intrusive_moirqueue_hp.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_intrusive_moirqueue_ptb.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_intrusive_msqueue_hp.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_intrusive_msqueue_ptb.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_intrusive_optimisticqueue_hp.o \
    $(TESTHDR_SRC_DIR)/queue/hdr_intrusive_optimisticqueue_ptb.o \
    $(TESTHDR_SRC_DIR)/stack/hdr_intrusive_treiber_stack_hp.o \
    $(TESTHDR_SRC_DIR)/stack/hdr_intrusive_treiber_stack_ptb.o

$(TESTHDR_OBJS_NO_OFFSETOF_WARN): %.o: %.cpp
	$(CXX) $(CPP_COMP_OPT) -I$(TESTHDR_SRC_DIR) -I$(TEST_COMMONHDR_SRC_DIR) -Wno-invalid-offsetof $< -o $@

TEST_SRC_DIR=../tests/unit
TEST_DATA_DIR=`pwd`/../tests/data

TEST_OBJ_FILE= \
	$(TEST_SRC_DIR)/michael_alloc.o \
    $(TEST_SRC_DIR)/alloc/hoard_threadtest.o \
    $(TEST_SRC_DIR)/alloc/larson.o \
    $(TEST_SRC_DIR)/alloc/linux_scale.o \
    $(TEST_SRC_DIR)/alloc/michael_allocator.o \
    $(TEST_SRC_DIR)/alloc/random.o \
    $(TEST_SRC_DIR)/lock/spinlock.o \
    $(TEST_SRC_DIR)/map2/map_find_int.o \
    $(TEST_SRC_DIR)/map2/map_find_string.o \
    $(TEST_SRC_DIR)/map2/map_insdel_func.o \
    $(TEST_SRC_DIR)/map2/map_insdel_int.o \
    $(TEST_SRC_DIR)/map2/map_insdel_item_int.o \
    $(TEST_SRC_DIR)/map2/map_insdel_string.o \
    $(TEST_SRC_DIR)/map2/map_insdel_item_string.o \
	$(TEST_SRC_DIR)/map2/map_insfind_int.o \
	$(TEST_SRC_DIR)/misc/allocator.o \
    $(TEST_SRC_DIR)/misc/bitop_st.o \
    $(TEST_SRC_DIR)/misc/thread_init_fini.o \
    $(TEST_SRC_DIR)/queue/pop_mt.o \
    $(TEST_SRC_DIR)/queue/push_mt.o \
    $(TEST_SRC_DIR)/queue/random_mt.o \
    $(TEST_SRC_DIR)/queue/reader_writer_mt.o \
    $(TEST_SRC_DIR)/queue/Queue_bounded_empty_ST.o \
    $(TEST_SRC_DIR)/stack/stack_push_mt.o 

TEST_EXE=$(BIN_PATH)/cds-unit

$(TEST_OBJ_FILE): %.o: %.cpp
	$(CXX) $(CPP_COMP_OPT) -I$(TEST_SRC_DIR) -I$(TEST_COMMONHDR_SRC_DIR) $< -o $@
    
make_test : $(BIN_PATH)/test-hdr $(BIN_PATH)/cds-unit 
	cd $(TEST_DATA_DIR); perl -X split.pl

$(BIN_PATH)/test-hdr : $(TESTHDR_OBJS) $(TESTHDR_OBJS_NO_OFFSETOF_WARN) $(TEST_COMMON_OBJS)
	$(CXX) $(LD_OPTS) -L$(BIN_PATH) \
	$(TESTHDR_OBJS) $(TESTHDR_OBJS_NO_OFFSETOF_WARN) $(TEST_COMMON_OBJS) \
	$(LD_BOOST_THREAD_LIB) -lcds -lrt -o $@

$(BIN_PATH)/cds-unit : $(TEST_OBJ_FILE) $(TEST_COMMON_OBJS)
	$(CXX) $(LD_OPTS) -L$(BIN_PATH) $(TEST_OBJ_FILE) $(TEST_COMMON_OBJS) $(LD_BOOST_THREAD_LIB) -lcds -lrt -o $@
	ln -sf $(TEST_DATA_DIR)/test.conf $(TEST_DATA_DIR)/test-debug.conf $(TEST_DATA_DIR)/dictionary.txt $(BIN_PATH)

make_debug_test : $(BIN_PATH)/test-hdr-debug $(BIN_PATH)/cds-unit-debug 

$(BIN_PATH)/test-hdr-debug : $(TESTHDR_OBJS) $(TESTHDR_OBJS_NO_OFFSETOF_WARN) $(TEST_COMMON_OBJS)
	$(CXX) $(LD_OPTS) -L$(BIN_PATH) \
	$(TESTHDR_OBJS) $(TESTHDR_OBJS_NO_OFFSETOF_WARN) $(TEST_COMMON_OBJS) \
	$(LD_BOOST_THREAD_LIB) -lcds-debug -lrt -o $@

$(BIN_PATH)/cds-unit-debug : $(TEST_OBJ_FILE) $(TEST_COMMON_OBJS)
	$(CXX) $(LD_OPTS) -L$(BIN_PATH) $(TEST_OBJ_FILE) $(TEST_COMMON_OBJS) $(LD_BOOST_THREAD_LIB) -lcds-debug -lrt -o $@
	ln -sf $(TEST_DATA_DIR)/test.conf $(TEST_DATA_DIR)/test-debug.conf $(TEST_DATA_DIR)/dictionary.txt $(BIN_PATH)

test: make_test 
test_debug: make_debug_test

test_hdr: $(BIN_PATH)/test-hdr
test_hdr_debug: $(BIN_PATH)/test-hdr-debug

##########################################
#
clean: 
	rm -f $(OBJ_PATH)/debug/*
	rm -f $(OBJ_PATH)/release/*
	rm -f $(TEST_COMMON_OBJS) $(TESTHDR_OBJS) $(TESTHDR_OBJS_NO_OFFSETOF_WARN) $(TEST_OBJ_FILE)
	rm -f $(BIN_PATH)/libcds*
	rm -f $(BIN_PATH)/cds-unit*

