include ../Makefile.inc

release : sort_demo.r.o
	$(LD) -o $@ $^

debug   : sort_demo.d.o
	$(LD) -o $@ $^

utest   : sort_test.d.o
	$(LD)  -o $@ $^ $(GTEST_LIBS)

run_utest : utest
	./$^

test : run_utest

run_itest : debug
	echo "Integration tests..."
	./$^

all: depends run_utest run_itest

include Makefile.hdrdep
