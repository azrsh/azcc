.ONESHELL:

CC:=gcc
AS:=as

CFLAGS:=-std=c11 -g -Wall -Wextra
LDFLAGS:=-static

SRCS=$(wildcard src/*.c)
UNIT_TEST_SRCS=$(wildcard test/unit/*.c)
FUNCTIONAL_TEST_SRCS=$(wildcard test/functional/*.c)
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

#Makefileの変数を関数として使用している。
#デフォルトでは、テストが失敗しても処理は続行され、リターンコードも0になる。
#makeコマンドの引数でRUN_TESTSに代入するテスト関数を変更すると、テスト失敗時に異常終了させることもできる
RUN_TESTS_CONTINUE_ON_FAIL=\
	for i in $^; do \
		if [ ! -d $$i ]; then \
			./$$i > ./$$i.log; \
			if [ $$? -eq 0 ]; then \
				echo "\033[32mPASS\033[m $$i"; \
			else \
				echo "\033[31mFAIL\033[m $$i For more information, see $$i.log"; \
			fi; \
		fi; \
	done;

RUN_TESTS_STOP_ON_FAIL=\
	res=1; \
	for i in $^; do \
		if [ ! -d $$i ]; then \
			./$$i > ./$$i.log; \
			if [ $$? -eq 0 ]; then \
				echo "\033[32mPASS\033[m $$i"; \
			else \
				res=0; \
				echo "\033[31mFAIL\033[m $$i For more information, see $$i.log"; \
			fi; \
		fi; \
	done; \
	if [ $$res -eq 0 ]; then \
		exit 1;\
	fi

RUN_TESTS=$(RUN_TESTS_CONTINUE_ON_FAIL)
#RUN_TESTS=$(RUN_TESTS_STOP_ON_FAIL)


# 1st Generation Compile

$(GEN1_BIN): $(GEN1_OBJS)
	@mkdir -p $(@D)
	$(CC) -o $@ $^ $(LDFLAGS)

bin/gen1/%.o: src/%.c src/*.h
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<


# 1st Generation Old Test

test-old: $(GEN1_BIN) $(TEST_TOOL_OBJS)
	test/old/test.sh


# 1st Generation Test

test/unit/cc/%.out: test/unit/%.c $(filter-out bin/gen1/main.o, $(GEN1_OBJS)) $(TEST_TOOL_OBJS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ -I src $< $(filter-out $<, $^) $(LDFLAGS)

test/functional/az1/%.out: test/functional/%.c $(GEN1_BIN) $(TEST_TOOL_OBJS)
	@mkdir -p $(@D)
	cpp -I test/dummylib $< > $(@:%.out=%.i)
	$(GEN1_BIN) $(@:%.out=%.i) > $(@:%.out=%.s)
	$(AS) -o $(@:%.out=%.o) $(@:%.out=%.s)
	$(CC) -o $@ $(@:%.out=%.o) $(filter-out $< $(GEN1_BIN), $^) $(LDFLAGS)

test-unit: $(UNIT_CC_TESTS)
	$(RUN_TESTS)

test-functional: $(FUNCTIONAL_AZ1_TESTS)
	$(RUN_TESTS)

test-shell-scripts: $(TEST_SHELL_SCRIPTS)
	$(RUN_TESTS)

test: test-unit test-functional test-shell-scripts


# 2nd Generation
$(GEN2_BIN): $(GEN2_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

bin/gen2/%.o: src/%.c src/*.h $(GEN1_BIN)
	@mkdir -p $(@D)
	cpp -I test/dummylib $< > $(@:%.o=%.i)
	$(GEN1_BIN) $(@:%.o=%.i) > $(@:%.o=%.s)
	$(AS) -o $@ $(@:%.o=%.s)

test/unit/ccaz1/%.out: test/unit/%.c $(filter-out bin/gen2/main.o, $(GEN2_OBJS)) $(TEST_TOOL_OBJS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -I src -o $(@:%.out=%.o) $<
	$(CC) -o $@ $(@:%.out=%.o) $(filter-out $<, $^) $(LDFLAGS)

test/unit/az1cc/%.out: test/unit/%.c $(GEN1_BIN) $(filter-out bin/gen1/main.o, $(GEN1_OBJS)) $(TEST_TOOL_OBJS)
	@mkdir -p $(@D)
	cpp -I test/dummylib -I src $< > $(@:%.out=%.i)
	$(GEN1_BIN) $(@:%.out=%.i) > $(@:%.out=%.s)
	$(AS) -o $(@:%.out=%.o) $(@:%.out=%.s)
	$(CC) -o $@ $(@:%.out=%.o) $(filter-out $< $(GEN1_BIN), $^) $(LDFLAGS)

test/unit/az1az1/%.out: test/unit/%.c $(GEN1_BIN) $(filter-out bin/gen2/main.o, $(GEN2_OBJS)) $(TEST_TOOL_OBJS)
	@mkdir -p $(@D)
	cpp -I test/dummylib -I src $< > $(@:%.out=%.i)
	$(GEN1_BIN) $(@:%.out=%.i) > $(@:%.out=%.s)
	$(AS) -o $(@:%.out=%.o) $(@:%.out=%.s)
	$(CC) -o $@ $(@:%.out=%.o) $(filter-out $< $(GEN1_BIN), $^) $(LDFLAGS)

test/functional/az2/%.out: test/functional/%.c $(GEN2_BIN) $(TEST_TOOL_OBJS)
	@mkdir -p $(@D)
	cpp -I test/dummylib -I src $< > $(@:%.out=%.i)
	$(GEN2_BIN) $(@:%.out=%.i) > $(@:%.out=%.s)
	$(AS) -o $(@:%.out=%.o) $(@:%.out=%.s)
	$(CC) -o $@ $(@:%.out=%.o) $(filter-out $< $(GEN2_BIN), $^) $(LDFLAGS)

test-unit2: $(UNIT_AZ1CC_TESTS) $(UNIT_AZ1AZ1_TESTS) $(UNIT_CCAZ1_TESTS)
	$(RUN_TESTS)

test-functional2: $(FUNCTIONAL_AZ2_TESTS)
	$(RUN_TESTS)

test2: test-unit2 test-functional2


# 3rd Generation
$(GEN3_BIN): $(GEN3_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

bin/gen3/%.o: src/%.c src/*.h $(GEN2_BIN)
	@mkdir -p $(@D)
	cpp -I test/dummylib $< > $(@:%.o=%.i)
	$(GEN1_BIN) $(@:%.o=%.i) > $(@:%.o=%.s)
	$(AS) -o $@ $(@:%.o=%.s)


test/unit/ccaz2/%.out: test/unit/%.c $(filter-out bin/gen3/main.o, $(GEN3_OBJS)) $(TEST_TOOL_OBJS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -I src -o $(@:%.out=%.o) $<
	$(CC) -o $@ $(@:%.out=%.o) $(filter-out $<, $^) $(LDFLAGS)

test/unit/az2cc/%.out: test/unit/%.c $(GEN2_BIN) $(filter-out bin/gen1/main.o, $(GEN1_OBJS)) $(TEST_TOOL_OBJS)
	@mkdir -p $(@D)
	cpp -I src -I test/dummylib $< > $(@:%.out=%.i)
	$(GEN2_BIN) $(@:%.out=%.i) >  $(@:%.out=%.s)
	$(AS) -o $(@:%.out=%.o) $(@:%.out=%.s)
	$(CC) -o $@ $(@:%.out=%.o) $(filter-out $< $(GEN2_BIN), $^) $(LDFLAGS)

test/unit/az2az2/%.out: test/unit/%.c $(GEN2_BIN) $(filter-out bin/gen3/main.o, $(GEN3_OBJS)) $(TEST_TOOL_OBJS)
	@mkdir -p $(@D)
	cpp -I src -I test/dummylib $< > $(@:%.out=%.i)
	$(GEN2_BIN) $(@:%.out=%.i) > $(@:%.out=%.s)
	$(AS) -o $(@:%.out=%.o) $(@:%.out=%.s)
	$(CC) -o $@ $(@:%.out=%.o) $(filter-out $< $(GEN2_BIN), $^) $(LDFLAGS)

test/functional/az3/%.out: test/functional/%.c $(GEN3_BIN) $(TEST_TOOL_OBJS)
	@mkdir -p $(@D)
	cpp -I src -I test/dummylib $< > $(@:%.out=%.i)
	$(GEN3_BIN) $(@:%.out=%.i) > $(@:%.out=%.s)
	$(AS) -o $(@:%.out=%.o) $(@:%.out=%.s)
	$(CC) -o $@ $(@:%.out=%.o) $(filter-out $< $(GEN3_BIN), $^) $(LDFLAGS)

test-unit3: $(UNIT_AZ2CC_TESTS) $(UNIT_AZ2AZ2_TESTS) $(UNIT_CCAZ2_TESTS)
	$(RUN_TESTS)

test-functional3: $(FUNCTIONAL_AZ3_TESTS)
	$(RUN_TESTS)

log/diff-gen2-gen3-%.log: $(GEN2_BIN) $(GEN3_BIN)
	@mkdir -p log
	{ diff bin/gen2/$*.s bin/gen3/$*.s > $@ && echo "\033[32mPASS\033[m $@";} || { echo "\033[31mFAIL\033[m $@ For more information, see $@";}

test-gen2-gen3-diff: $(GEN2_GEN3_DIFF)

test3: test-unit3 test-functional3 test-gen2-gen3-diff


test-all: test test2 test3

# Others

clean:
	-rm -f test/tool/*.o
	-rm -f test/old/tmp*
	-rm -rf test/unit/*/
	-rm -rf test/functional/*/
	-rm -rf bin

all: clean test-all

.PHONY: test-old test-all test test-unit test-functional test2 test-unit2 test-functional2 test3 test-unit3 test-functional3 test-gen2-gen3-diff clean all
.SILENT: test-all test test-unit test-functional test-shell-scripts test2 test-unit2 test-functional2 test3 test-unit3 test-functional3 test-gen2-gen3-diff $(GEN2_GEN3_DIFF)
