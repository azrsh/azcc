CFLAGS=-std=c11 -g -static -Wall

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

UNIT_TEST_SRCS=$(wildcard test/unit/*.c)
UNIT_CC_TESTS=$(UNIT_TEST_SRCS:test/unit/%.c=test/unit/cc/%.out)
UNIT_AZ1_TESTS=$(UNIT_TEST_SRCS:test/unit/%.c=test/unit/az1/%.out)
FUNCTIONAL_TEST_SRCS=$(wildcard test/functional/*.c)
FUNCTIONAL_AZ1_TESTS=$(FUNCTIONAL_TEST_SRCS:test/functional/%.c=test/functional/az1/%.out)
FUNCTIONAL_AZ2_TESTS=$(FUNCTIONAL_TEST_SRCS:test/functional/%.c=test/functional/az2/%.out)

TEST_TOOL_SRCS=$(wildcard test/tool/*.c)
TEST_TOOL_OBJS=$(TEST_TOOL_SRCS:.c=.o)

TEST_SELF_SRCS=$(wildcard test/self/*.c)
TEST_SELF_OBJS=$(TEST_SELF_SRCS:.c=.o)


# 1st Generation Compile

azcc: $(OBJS)
	$(CC) -o azcc $(OBJS) $(LDFLAGS)

$(OBJS): *.h


# 1st Generation Old Test

test-old: azcc $(TEST_TOOL_OBJS)
	./test.sh


# 1st Generation Test

test/unit/cc/%.out: $(filter-out main.o, $(OBJS)) $(TEST_TOOL_OBJS) test/unit/%.c
	$(CC) -o $@ -I ./ test/unit/$*.c $(filter-out main.o, $(OBJS)) $(TEST_TOOL_OBJS)

test/functional/az1/%.out: azcc $(TEST_TOOL_OBJS) test/functional/%.c
	./azcc test/functional/$*.c > test/functional/az1/$*.s
	$(CC) -o $@ test/functional/az1/$*.s $(TEST_TOOL_OBJS)

test-unit: $(UNIT_CC_TESTS)
	for i in $^; do echo $$i; ./$$i || exit 1; echo; done

test-functional: $(FUNCTIONAL_AZ1_TESTS)
	for i in $^; do echo $$i; ./$$i || exit 1; echo; done

test: test-unit test-functional


# 2nd Generation

test/self/%.o: azcc test/self/%.c
	cpp -I test/self/dummylib test/self/$*.c > test/self/$*.i
	./azcc test/self/$*.i > test/self/$*.s
	cc -c -o $@ test/self/$*.s

self: $(TEST_SELF_OBJS)

test/unit/az1/%.out: self $(filter-out main.o, $(TEST_SELF_OBJS)) $(TEST_TOOL_OBJS) test/unit/%.c
	./test/unit/rmlink.sh
	cpp -I test/self test/unit/$*.c > test/unit/az1/$*.i
	./azcc test/unit/az1/$*.i > test/unit/az1/$*.s
	$(CC) -o $@ test/unit/az1/$*.s $(filter-out main.o, $(OBJS)) $(TEST_TOOL_OBJS)
	./test/unit/makelink.sh

test-unit2: $(UNIT_AZ1_TESTS)
	for i in $^; do echo $$i; ./$$i || exit 1; echo; done


# Others

clean:
	rm -f azcc *.o *~ tmp*
	rm -f test/tool/*.o
	rm -f test/unit/cc/*
	rm -f test/unit/az1/*
	rm -f test/functional/az1/*
	rm -f test/functional/az2/*
	rm -f test/self/*.o test/self/*.s test/self/*.i

.PHONY: test test-unit test-functional test-old self clean
