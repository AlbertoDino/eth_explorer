CC     := gcc
CFLAGS := -g
LDLIBS := -lcurl -ljson-c
TARGET := eth-explorer

SRCS := main.c common_topics.c eth.c w_curl.c rpc_parser.c eth_simulate.c eth_trace.c
VPATH := src

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(addprefix src/,$(SRCS)) $(CFLAGS) $(LDLIBS) -o $@

clean:
	rm -f $(TARGET)

.PHONY: all clean