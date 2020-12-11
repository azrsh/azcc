.ONESHELL:

CFLAGS:=-std=c11 -g -static -Wall -Wextra

SRCS=$(wildcard src/*.c)
UNIT_TEST_SRCS=$(wildcard test/unit/*.c)
FUNCTIONAL_TEST_SRCS=$(wildcard test/functional/*.c)
TEST_TOOL_SRCS=$(wildcard test/tool/*.c)
TEST_TOOL_OBJS=$(TEST_TOOL_SRCS:.c=.o)

UNIT_CC_TESTS=$(UNIT_TEST_SRCS:test/unit/%.c=test/unit/cc/%.out)
FUNCTIONAL_AZ1_TESTS=$(FUNCTIONAL_TEST_SRCS:test/functional/%.c=test/functional/az1/%.out)
GEN1_OBJS=$(SRCS:src/%.c=bin/gen1/%.o)
GEN1_BIN:=bin/gen1/azcc

UNIT_AZ1CC_TESTS=$(UNIT_TEST_SRCS:test/unit/%.c=test/unit/az1cc/%.out)
UNIT_CCAZ1_TESTS=$(UNIT_TEST_SRCS:test/unit/%.c=test/unit/ccaz1/%.out)
UNIT_AZ1AZ1_TESTS=$(UNIT_TEST_SRCS:test/unit/%.c=test/unit/az1az1/%.out)
FUNCTIONAL_AZ2_TESTS=$(FUNCTIONAL_TEST_SRCS:test/functional/%.c=test/functional/az2/%.out)
GEN2_OBJS=$(SRCS:src/%.c=bin/gen2/%.o)
GEN2_BIN:=bin/gen2/azcc

UNIT_AZ2CC_TESTS=$(UNIT_TEST_SRCS:test/unit/%.c=test/unit/az2cc/%.out)
UNIT_CCAZ2_TESTS=$(UNIT_TEST_SRCS:test/unit/%.c=test/unit/ccaz2/%.out)
UNIT_AZ2AZ2_TESTS=$(UNIT_TEST_SRCS:test/unit/%.c=test/unit/az2az2/%.out)
FUNCTIONAL_AZ3_TESTS=$(FUNCTIONAL_TEST_SRCS:test/functional/%.c=test/functional/az3/%.out)
GEN3_OBJS=$(SRCS:src/%.c=bin/gen3/%.o)
GEN3_BIN:=bin/gen3/azcc
GEN2_GEN3_DIFF_DUMMIES=$(SRCS:%.c=bin/gen2-gen3-diff/%.diff)


# 1st Generation Compile

$(GEN1_BIN): $(GEN1_OBJS)
	$(CC) -o $(GEN1_BIN) $(GEN1_OBJS) $(LDFLAGS)

bin/gen1/%.o: src/*.h src/%.c
	$(CC) -c -o $@ src/$*.c


# 1st Generation Old Test

test-old: azcc $(TEST_TOOL_OBJS)
	test/old/test.sh


# 1st Generation Test

test/unit/cc/%.out: $(filter-out bin/gen1/main.o, $(GEN1_OBJS)) $(TEST_TOOL_OBJS) test/unit/%.c
	$(CC) -o $@ -I src test/unit/$*.c $(filter-out bin/gen1/main.o, $(GEN1_OBJS)) $(TEST_TOOL_OBJS)

test/functional/az1/%.out: $(GEN1_BIN) $(TEST_TOOL_OBJS) test/functional/%.c
	cpp -I test/dummylib test/functional/$*.c > test/functional/az1/$*.i
	$(GEN1_BIN) test/functional/az1/$*.i > test/functional/az1/$*.s
	$(CC) -o $@ test/functional/az1/$*.s $(TEST_TOOL_OBJS)

test-unit: $(UNIT_CC_TESTS)
	for i in $^; do echo -n "$$i => "; (./$$i > ./$$i.log && echo "\033[32mPASS\033[m") || (echo "\033[31mFAIL\033[m For more information, see $$i.log" && exit 1); done

test-functional: $(FUNCTIONAL_AZ1_TESTS)
	for i in $^; do echo -n "$$i => "; (./$$i > ./$$i.log && echo "\033[32mPASS\033[m") || (echo "\033[31mFAIL\033[m For more information, see $$i.log" && exit 1); done

test: test-unit test-functional


# 2nd Generation

bin/gen2/%.o: $(GEN1_BIN) src/%.c
	cpp -I test/dummylib src/$*.c > bin/gen2/$*.i
	$(GEN1_BIN) bin/gen2/$*.i > bin/gen2/$*.s
	$(CC) -c -o $@ bin/gen2/$*.s


test/unit/ccaz1/%.out: $(filter-out bin/gen2/main.o, $(GEN2_OBJS)) $(TEST_TOOL_OBJS) test/unit/%.c
	test/unit/rmlink.sh
	$(CC) -c -I src -o test/unit/ccaz1/$*.o test/unit/$*.c
	$(CC) -o $@ test/unit/ccaz1/$*.o $(filter-out bin/gen2/main.o, $(GEN2_OBJS)) $(TEST_TOOL_OBJS)
	test/unit/makelink.sh

test/unit/az1cc/%.out: $(GEN1_BIN) $(filter-out bin/gen1/main.o, $(GEN1_OBJS)) $(TEST_TOOL_OBJS) test/unit/%.c
	test/unit/rmlink.sh
	cpp -I src -I test/dummylib test/unit/$*.c > test/unit/az1cc/$*.i
	$(GEN1_BIN) test/unit/az1cc/$*.i > test/unit/az1cc/$*.s
	$(CC) -o $@ test/unit/az1cc/$*.s $(filter-out bin/gen1/main.o, $(GEN1_OBJS)) $(TEST_TOOL_OBJS)
	test/unit/makelink.sh

test/unit/az1az1/%.out: $(GEN1_BIN) $(filter-out bin/gen2/main.o, $(GEN2_OBJS)) $(TEST_TOOL_OBJS) test/unit/%.c
	test/unit/rmlink.sh
	cpp -I src -I test/dummylib test/unit/$*.c > test/unit/az1az1/$*.i
	$(GEN1_BIN) test/unit/az1az1/$*.i > test/unit/az1az1/$*.s
	$(CC) -o $@ test/unit/az1az1/$*.s $(filter-out bin/gen2/main.o, $(GEN2_OBJS)) $(TEST_TOOL_OBJS)
	test/unit/makelink.sh

$(GEN2_BIN): $(GEN2_OBJS)
	$(CC) -o $(GEN2_BIN) $(GEN2_OBJS) $(LDFLAGS)

test/functional/az2/%.out: $(GEN2_BIN) $(TEST_TOOL_OBJS) test/functional/%.c
	cpp -I test/dummylib test/functional/$*.c > test/functional/az2/$*.i
	$(GEN2_BIN) test/functional/az2/$*.i > test/functional/az2/$*.s
	$(CC) -o $@ test/functional/az2/$*.s $(TEST_TOOL_OBJS)

test-unit2: $(UNIT_AZ1CC_TESTS) $(UNIT_AZ1AZ1_TESTS) $(UNIT_CCAZ1_TESTS)
	for i in $^; do echo -n "$$i => "; (./$$i > ./$$i.log && echo "\033[32mPASS\033[m") || (echo "\033[31mFAIL\033[m For more information, see $$i.log" && exit 1); done

test-functional2: $(FUNCTIONAL_AZ2_TESTS)
	for i in $^; do echo -n "$$i => "; (./$$i > ./$$i.log && echo "\033[32mPASS\033[m") || (echo "\033[31mFAIL\033[m For more information, see $$i.log" && exit 1); done

test2: test-unit2 test-functional2


# 3rd Generation

bin/gen3/%.o: $(GEN2_BIN) src/%.c
	cpp -I test/dummylib src/$*.c > bin/gen3/$*.i
	$(GEN2_BIN) bin/gen3/$*.i > bin/gen3/$*.s
	$(CC) -c -o $@ bin/gen3/$*.s


test/unit/ccaz2/%.out: $(filter-out bin/gen3/main.o, $(GEN3_OBJS)) $(TEST_TOOL_OBJS) test/unit/%.c
	$(CC) -c -I src -o test/unit/ccaz2/$*.o test/unit/$*.c
	$(CC) -o $@ test/unit/ccaz2/$*.o $(filter-out bin/gen3/main.o, $(GEN3_OBJS)) $(TEST_TOOL_OBJS)

test/unit/az2cc/%.out: $(GEN2_BIN) $(filter-out bin/gen1/main.o, $(GEN1_OBJS)) $(TEST_TOOL_OBJS) test/unit/%.c
	cpp -I src -I test/dummylib test/unit/$*.c > test/unit/az2cc/$*.i
	$(GEN2_BIN) test/unit/az2cc/$*.i > test/unit/az2cc/$*.s
	$(CC) -o $@ test/unit/az2cc/$*.s $(filter-out bin/gen1/main.o, $(GEN1_OBJS)) $(TEST_TOOL_OBJS)

test/unit/az2az2/%.out: $(GEN2_BIN) $(filter-out bin/gen3/main.o, $(GEN3_OBJS)) $(TEST_TOOL_OBJS) test/unit/%.c
	cpp -I src -I test/dummylib test/unit/$*.c > test/unit/az2az2/$*.i
	$(GEN2_BIN) test/unit/az2az2/$*.i > test/unit/az2az2/$*.s
	$(CC) -o $@ test/unit/az2az2/$*.s $(filter-out bin/gen3/main.o, $(GEN3_OBJS)) $(TEST_TOOL_OBJS)

$(GEN3_BIN): $(GEN3_OBJS)
	$(CC) -o $(GEN3_BIN) $(GEN3_OBJS) $(LDFLAGS)

test/functional/az3/%.out: $(GEN3_BIN) $(TEST_TOOL_OBJS) test/functional/%.c
	cpp -I test/dummylib test/functional/$*.c > test/functional/az3/$*.i
	$(GEN3_BIN) test/functional/az3/$*.i > test/functional/az3/$*.s
	$(CC) -o $@ test/functional/az3/$*.s $(TEST_TOOL_OBJS)

test-unit3: $(UNIT_AZ2CC_TESTS) $(UNIT_AZ2AZ2_TESTS) $(UNIT_CCAZ2_TESTS)
	for i in $^; do echo -n "$$i => "; (./$$i > ./$$i.log && echo "\033[32mPASS\033[m") || (echo "\033[31mFAIL\033[m For more information, see $$i.log" && exit 1); done

test-functional3: $(FUNCTIONAL_AZ3_TESTS)
	for i in $^; do echo -n "$$i => "; (./$$i > ./$$i.log && echo "\033[32mPASS\033[m") || (echo "\033[31mFAIL\033[m For more information, see $$i.log" && exit 1); done

bin/gen2-gen3-diff/%.diff: bin/gen2/%.s bin/gen3/%.s
	diff bin/gen2/$*.s bin/gen3/$*.s

test-gen2-gen3-diff: $(GEN_GEN3_DIFF_DUMMIES)

test3: test-unit3 test-functional3 test-gen2-gen3-diff


test-all: test test2 test3

# Others

clean:
	-rm -f test/tool/*.o
	-rm -f test/old/tmp*
	-rm -f $(GEN1_BIN) $(GEN1_OBJS)
	-rm -f $(GEN2_BIN) $(GEN2_OBJS)
	-rm -f $(GEN3_BIN) $(GEN3_OBJS)
	-rm -f test/unit/*/*.i test/unit/*/*.s test/unit/*/*.o test/unit/*/*.out
	-rm -f test/functional/*/*.i test/functional/*/*.s test/functional/*/*.o test/functional/*/*.out
	-rm -f bin/*/azcc bin/*/*.o bin/*/*.s bin/*/*.i

all: clean test-all

.PHONY: test-old test-all test test-unit test-functional test2 test-unit2 test-functional2 test3 test-unit3 test-functional3 test-gen2-gen3-diff $(GEN2_GEN3_DIFF_DUMMIES) clean all
.SILENT: test-all test test-unit test-functional test2 test-unit2 test-functional2 test3 test-unit3 test-functional3
