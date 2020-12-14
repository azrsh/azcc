.ONESHELL:

CFLAGS:=-std=c11 -g -Wall -Wextra
LDFLAGS:=-static

SRCS=$(wildcard src/*.c)
UNIT_TEST_SRCS=$(wildcard test/unit/*.c)
FUNCTIONAL_TEST_SRCS=$(wildcard test/functional/*.c)
UNIT_TEST_DIRS:=test/unit/cc test/unit/az1cc test/unit/ccaz1 test/unit/az1az1 test/unit/az2cc test/unit/ccaz2 test/unit/az2az2
FUNCTIONAL_TEST_DIRS:=test/functional/az1 test/functional/az2 test/functional/az3
BIN_PARENT_DIR:=bin
BIN_DIRS:=bin/gen1 bin/gen2 bin/gen3
TEST_TOOL_SRCS=$(wildcard test/tool/*.c)
TEST_TOOL_OBJS=$(TEST_TOOL_SRCS:.c=.o)
TEST_SHELL_SCRIPTS=$(wildcard test/yanorei32/*.sh)

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
GEN2_GEN3_DIFF=$(SRCS:src/%.c=log/diff-gen2-gen3-%.log)

RUN_TESTS_CONTINUE_ON_FAIL=for i in $^; do if [ ! -d $$i ]; then { ./$$i > ./$$i.log && echo "\033[32mPASS\033[m $$i";} || { echo "\033[31mFAIL\033[m $$i For more information, see $$i.log";}; fi done
RUN_TESTS_STOP_ON_FAIL=for i in $^; do if [ ! -d $$i ]; then { ./$$i > ./$$i.log && echo "\033[32mPASS\033[m $$i";} || { echo "\033[31mFAIL\033[m $$i For more information, see $$i.log" && exit 1;}; fi done

RUN_TESTS=$(RUN_TESTS_CONTINUE_ON_FAIL)
#RUN_TESTS=$(RUN_TESTS_STOP_ON_FAIL)

# 1st Generation Compile

$(GEN1_BIN): $(GEN1_OBJS)
	$(CC) -o $(GEN1_BIN) $(GEN1_OBJS) $(LDFLAGS)

bin/gen1/%.o: $(BIN_DIRS) src/*.h src/%.c
	$(CC) $(CFLAGS) -c -o $@ src/$*.c


# 1st Generation Old Test

test-old: azcc $(TEST_TOOL_OBJS)
	test/old/test.sh


# 1st Generation Test

test/unit/cc/%.out: $(filter-out bin/gen1/main.o, $(GEN1_OBJS)) $(TEST_TOOL_OBJS) test/unit/%.c
	$(CC) $(CFLAGS) -o $@ -I src test/unit/$*.c $(filter-out bin/gen1/main.o, $(GEN1_OBJS)) $(TEST_TOOL_OBJS) $(LDFLAGS)

test/functional/az1/%.out: $(GEN1_BIN) $(TEST_TOOL_OBJS) test/functional/%.c
	cpp -I test/dummylib test/functional/$*.c > test/functional/az1/$*.i
	$(GEN1_BIN) test/functional/az1/$*.i > test/functional/az1/$*.s
	$(CC) -o $@ test/functional/az1/$*.s $(TEST_TOOL_OBJS) $(LDFLAGS)

test-unit: $(UNIT_TEST_DIRS) $(UNIT_CC_TESTS)
	$(RUN_TESTS)

test-functional: $(FUNCTIONAL_TEST_DIRS) $(FUNCTIONAL_AZ1_TESTS)
	$(RUN_TESTS)

test-shell-scripts: $(TEST_SHELL_SCRIPTS)
	$(RUN_TESTS)

test: test-unit test-functional test-shell-scripts


# 2nd Generation

bin/gen2/%.o: $(BIN_DIRS) $(GEN1_BIN) src/%.c
	cpp -I test/dummylib src/$*.c > bin/gen2/$*.i
	$(GEN1_BIN) bin/gen2/$*.i > bin/gen2/$*.s
	$(CC) -c -o $@ bin/gen2/$*.s

test/unit/ccaz1/%.out: $(filter-out bin/gen2/main.o, $(GEN2_OBJS)) $(TEST_TOOL_OBJS) test/unit/%.c
	$(CC) $(CFLAGS) -c -I src -o test/unit/ccaz1/$*.o test/unit/$*.c
	$(CC) -o $@ test/unit/ccaz1/$*.o $(filter-out bin/gen2/main.o, $(GEN2_OBJS)) $(TEST_TOOL_OBJS) $(LDFLAGS)

test/unit/az1cc/%.out: $(GEN1_BIN) $(filter-out bin/gen1/main.o, $(GEN1_OBJS)) $(TEST_TOOL_OBJS) test/unit/%.c
	cpp -I src -I test/dummylib test/unit/$*.c > test/unit/az1cc/$*.i
	$(GEN1_BIN) test/unit/az1cc/$*.i > test/unit/az1cc/$*.s
	$(CC) -o $@ test/unit/az1cc/$*.s $(filter-out bin/gen1/main.o, $(GEN1_OBJS)) $(TEST_TOOL_OBJS) $(LDFLAGS)

test/unit/az1az1/%.out: $(GEN1_BIN) $(filter-out bin/gen2/main.o, $(GEN2_OBJS)) $(TEST_TOOL_OBJS) test/unit/%.c
	cpp -I src -I test/dummylib test/unit/$*.c > test/unit/az1az1/$*.i
	$(GEN1_BIN) test/unit/az1az1/$*.i > test/unit/az1az1/$*.s
	$(CC) -o $@ test/unit/az1az1/$*.s $(filter-out bin/gen2/main.o, $(GEN2_OBJS)) $(TEST_TOOL_OBJS) $(LDFLAGS)

$(GEN2_BIN): $(GEN2_OBJS)
	$(CC) -o $(GEN2_BIN) $(GEN2_OBJS) $(LDFLAGS)

test/functional/az2/%.out: $(GEN2_BIN) $(TEST_TOOL_OBJS) test/functional/%.c
	cpp -I test/dummylib test/functional/$*.c > test/functional/az2/$*.i
	$(GEN2_BIN) test/functional/az2/$*.i > test/functional/az2/$*.s
	$(CC) -o $@ test/functional/az2/$*.s $(TEST_TOOL_OBJS) $(LDFLAGS)

test-unit2: $(UNIT_TEST_DIRS) $(UNIT_AZ1CC_TESTS) $(UNIT_AZ1AZ1_TESTS) $(UNIT_CCAZ1_TESTS)
	$(RUN_TESTS)

test-functional2: $(FUNCTIONAL_TEST_DIRS) $(FUNCTIONAL_AZ2_TESTS)
	$(RUN_TESTS)

test2: test-unit2 test-functional2


# 3rd Generation

bin/gen3/%.o: $(BIN_DIRS) $(GEN2_BIN) src/%.c
	cpp -I test/dummylib src/$*.c > bin/gen3/$*.i
	$(GEN2_BIN) bin/gen3/$*.i > bin/gen3/$*.s
	$(CC) -c -o $@ bin/gen3/$*.s


test/unit/ccaz2/%.out: $(filter-out bin/gen3/main.o, $(GEN3_OBJS)) $(TEST_TOOL_OBJS) test/unit/%.c
	$(CC) $(CFLAGS) -c -I src -o test/unit/ccaz2/$*.o test/unit/$*.c
	$(CC) -o $@ test/unit/ccaz2/$*.o $(filter-out bin/gen3/main.o, $(GEN3_OBJS)) $(TEST_TOOL_OBJS) $(LDFLAGS)

test/unit/az2cc/%.out: $(GEN2_BIN) $(filter-out bin/gen1/main.o, $(GEN1_OBJS)) $(TEST_TOOL_OBJS) test/unit/%.c
	cpp -I src -I test/dummylib test/unit/$*.c > test/unit/az2cc/$*.i
	$(GEN2_BIN) test/unit/az2cc/$*.i > test/unit/az2cc/$*.s
	$(CC) -o $@ test/unit/az2cc/$*.s $(filter-out bin/gen1/main.o, $(GEN1_OBJS)) $(TEST_TOOL_OBJS) $(LDFLAGS)

test/unit/az2az2/%.out: $(GEN2_BIN) $(filter-out bin/gen3/main.o, $(GEN3_OBJS)) $(TEST_TOOL_OBJS) test/unit/%.c
	cpp -I src -I test/dummylib test/unit/$*.c > test/unit/az2az2/$*.i
	$(GEN2_BIN) test/unit/az2az2/$*.i > test/unit/az2az2/$*.s
	$(CC) -o $@ test/unit/az2az2/$*.s $(filter-out bin/gen3/main.o, $(GEN3_OBJS)) $(TEST_TOOL_OBJS) $(LDFLAGS)

$(GEN3_BIN): $(GEN3_OBJS)
	$(CC) -o $(GEN3_BIN) $(GEN3_OBJS) $(LDFLAGS)

test/functional/az3/%.out: $(GEN3_BIN) $(TEST_TOOL_OBJS) test/functional/%.c
	cpp -I test/dummylib test/functional/$*.c > test/functional/az3/$*.i
	$(GEN3_BIN) test/functional/az3/$*.i > test/functional/az3/$*.s
	$(CC) -o $@ test/functional/az3/$*.s $(TEST_TOOL_OBJS) $(LDFLAGS)

test-unit3: $(UNIT_TEST_DIRS) $(UNIT_AZ2CC_TESTS) $(UNIT_AZ2AZ2_TESTS) $(UNIT_CCAZ2_TESTS)
	$(RUN_TESTS)

test-functional3: $(FUNCTIONAL_TEST_DIRS) $(FUNCTIONAL_AZ3_TESTS)
	$(RUN_TESTS)

log/diff-gen2-gen3-%.log: $(GEN2_BIN) $(GEN3_BIN)
	mkdir -p log
	{ diff bin/gen2/$*.s bin/gen3/$*.s > $@ && echo "\033[32mPASS\033[m $@";} || { echo "\033[31mFAIL\033[m $@ For more information, see $@";}

test-gen2-gen3-diff: $(GEN2_GEN3_DIFF)

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

$(UNIT_TEST_DIRS):
	mkdir $@

$(FUNCTIONAL_TEST_DIRS):
	mkdir $@

$(BIN_DIRS): $(BIN_PARENT_DIR)
	mkdir $@

$(BIN_PARENT_DIR):
	mkdir $@

.PHONY: test-old test-all test test-unit test-functional test2 test-unit2 test-functional2 test3 test-unit3 test-functional3 test-gen2-gen3-diff clean all
.SILENT: test-all test test-unit test-functional test-shell-scripts test2 test-unit2 test-functional2 test3 test-unit3 test-functional3 test-gen2-gen3-diff $(GEN2_GEN3_DIFF) $(BIN_PARENT_DIR) $(BIN_DIRS) $(UNIT_TEST_DIRS) $(FUNCTIONAL_TEST_DIRS) 
