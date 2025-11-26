#ifndef __ETH_H
#define __ETH_H

#include "rpc_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include <json-c/json.h>
#include <time.h>

/** -- Type Definition -- **/

struct arguments
{
    /*argument state*/
    int verbose;
    const char * json_rpc_url;
    const char * action;
    const char * call;
    const char * module;
    const char * requestId;
    char       * params;
};

/* Networks */ 

struct network_option {
    const char * name;
    const char * rpc_url;
};


/** -- Type Functions -- **/

void init_arguments(struct arguments *arguments);

void free_arguments(struct arguments *arguments);

void set_params(struct arguments  * arg,const char* params);

/** --  RPC Modules -- **/ 

typedef json_object* (*exec_program) (struct arguments *arg);

struct rpc_module_id {
    const char * name;
};

typedef struct rpc_module_id rpc_module_id;

struct rpc_program {
    rpc_module_id  id;
    exec_program   execute_rpc_program;
    const char *   desc;

};

/** --  Generic Function -- **/ 

int          make_rpc_call    (const char * module, struct arguments *arg, t_rpcResponse *response);
json_object* build_rpc_payload (const char * module, struct arguments *arg);

/// Converts a Hex string into a long long in decimal format
long long    htol   (const char * hex);

/// Converts a long long decimal number into a hex string.
/// make sure the char * has enough buffer for long character strings
int          ltohex (long long value, char * strHex);

char *       unixtodt(time_t unix_timestamp);

/** --  Common EVM Signature Hashes  -- **/ 

struct evm_signature_topic {
    const char * hash;
    char * signature_name;
};

char * get_common_signature(const char * hash);

#endif