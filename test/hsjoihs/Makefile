.PHONY: clean test all
.SILENT:
.ONESHELL:

CC:=gcc
LDFLAGS:=-static
CPP:=cpp
AS:=as

GEN=unknown
SRCS=$(wildcard *.c)
OBJS=$(SRCS:%.c=bin/$(GEN)/%.o)
BIN=bin/$(GEN)/hsjoihs_test.out

bin/$(GEN)/%.o: %.c *.h
	@mkdir -p `dirname $@`
	$(CPP) $< | $(AZCC) | $(AS) - -o $@

$(BIN): $(OBJS)
	@mkdir -p `dirname $@`
	$(CC) $^ ../tool/testutil.o -o $@ $(LDFLAGS)

test: $(BIN)
	./$<

clean:
	rm -rf $(OBJS) $(BIN)

all:
	$(MAKE) clean # 順序に依存している
	$(MAKE) test
