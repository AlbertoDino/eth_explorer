#ifndef __ETH_TRACE_H
#define __ETH_TRACE_H

#include "eth.h"

/* -- eth_trace -- */

json_object* execute_eth_trace (struct arguments *arg);

int ex_response_eth_trace (const json_object *paylaod_request, struct arguments *arg, struct t_rpcResponse *resp);

struct EthcallNode {

    char * transaction_hash;
    char * address_from;
    char * address_to;

    char * call_type;       // function called name derived from event ?
    char * topic_signature; // I don't think I need indexed paramters or data (for now)

    char * selector;

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
