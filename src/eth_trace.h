#ifndef __ETH_TRACE_H
#define __ETH_TRACE_H

#include "eth.h"

#define EMPTY_ADDRESS "0x0000000000000000000000000000000000000000000000000000000000000000"

/* -- eth_trace -- */

// entry point of eth_trace program
json_object* execute_eth_trace (struct arguments *arg);

struct EthcallNode {
    char *    time_stamp;
    char *    miner;
    char *    parent_block_hash;
    char *    transaction_hash;
    char *    address_from;
    char *    address_to;
    char *    block_number;
    long long gas_used;
    char *    call_type;       
    char *    topic_signature;
    char *    selector;
    int       is_proxy;
    long long log_index;
    char *    proxy_type;
    char **   topics;
    size_t    num_topics;    
    size_t    num_children;
    struct EthcallNode ** children;
    json_object* data;
};

struct EthcallNode* alloc_node();

void init_EthcallNode(struct EthcallNode* node);

void free_ethcallNode(struct EthcallNode* node);

struct EthcallNode* find_node_from_logindex(struct EthcallNode* node,long long logIndex);

struct EthcallNode* find_node_from_address(struct EthcallNode* node,const char* address);

void add_child(struct EthcallNode* parent,struct EthcallNode* child);

#endif
