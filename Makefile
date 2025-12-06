CC := gcc
CFLAGS := -g
LIBS := -lcurl -ljson-c
TARGET := eth-explorer

SRCS := main.c common_topics.c eth.c w_curl.c rpc_parser.c eth_simulate.c eth_trace.c
VPATH := src

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(addprefix src/,$(SRCS)) $(CFLAGS) $(LIBS) -o $@

clean:
	rm -f $(TARGET)

.PHONY: all clean