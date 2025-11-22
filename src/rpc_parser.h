#ifndef __RPC_PARSER_H
#define __RPC_PARSER_H

#include <json-c/json.h>

struct t_rpcResponse {
    char* id;
    enum { RPC_SUCCESS, RPC_ERROR } type;
    union {
        char *value; // allocate dynamically 

        struct {
            int code;
            char* message; // allocate dynamically 
        } error_info;

    } data;
};

typedef struct t_rpcResponse t_rpcResponse;

int parse_json_rpc_response(const char *jstring, t_rpcResponse *response, int verbose);

void free_rpc_response(t_rpcResponse* response);

#endif