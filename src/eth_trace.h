#ifndef __ETH_TRACE_H
#define __ETH_TRACE_H

#include "eth.h"

/* -- eth_trace -- */

// entry point of eth_trace program
json_object* execute_eth_trace (struct arguments *arg);

struct EthcallNode {
    char * transaction_hash;
    char * address_from;
    char * address_to;
    long long gas_used;
    char * call_type;       // function called name derived from event ?
    char * topic_signature; // I don't think I need indexed paramters or data (for now)
    char * selector;
    int    is_proxy;
    char * proxy_type;
    char ** topics;
    size_t num_topics;
    struct EthcallNode ** children;
    size_t num_children;
    json_object* data;
};

struct EthcallNode* alloc_node();

void init_EthcallNode(struct EthcallNode* node);

void free_ethcallNode(struct EthcallNode* node);

struct EthcallNode* find_address_from_node(struct EthcallNode* node,const char* address);

void add_child(struct EthcallNode* parent,struct EthcallNode* child);

#endif
