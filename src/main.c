#include "eth.h"
#include "w_curl.h"
#include "rpc_parser.h"
#include "eth_simulate.h"
#include "eth_trace.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include <json-c/json.h>

static char doc[]                    = "A simple program for making RPC calls to evm blockchains.";
static char args_doc[]               = "!...";
const char *argp_program_bug_address = "dinoitaly@gmail.com";
const char *argp_program_version     = "version 1.0";

struct rpc_program rpc_modules[] = {
    { "<ethereum RPC method>"    , execute_eth_simulate , "Any standard Ethereum RPC method supported by the network. See API reference for Parameters"  },
    { "eth_simulate"             , execute_eth_simulate , "Replays a Tx call on the previous block."  },
    { "eth_simulate_latest"      , execute_eth_simulate , "Replays a Tx call on the latest block."  },
    { "eth_trace"                , execute_eth_trace    , "Visualize how a transaction interacts with multiple contracts."  },
};

/* Networks */ 

/*Parse Arguments*/
#define MODULESS_KEY   2
#define JSONRPC_KEY   'j'
#define CALL_KEY      'c'
#define PARAMETER_KEY 'p'
#define REQUESTID_KEY 'i'
#define VERBOSE_KEY   'v'
#define NETWORK_KEY   'n'

/* Arguments */
        
struct argp_option options[] ={
    /*
    1st  @ - long option
    2nd  @ - short option, if a number it will be the associated key of the long option
    3rd  @ - there is a mandatory argument to the option
    4th  @ - option flags 0=mandatory , OPTION_ARG_OPTIONAL * if long options required a = between the option and the argumnt
    5th  @ - description to show in the --help
    */
    {"builtin-modules"  , MODULESS_KEY  , 0             , 0                    , "Gets the list of modules embedded in the program" },
    {"network"          , NETWORK_KEY   , "NETWORKNAME" , 0                    , "Specify the network by a envrioment name. The name have as `export <network_name>=<json-rpc-url>`." },
    {"jsonrpc"          , JSONRPC_KEY   , "URL"         , 0                    , "Specify Json Rpc url" },
    {"call"             , CALL_KEY      , "MODULENAME"  , 0                    , "Call a specific module via RPC" },
    {"parameters"       , PARAMETER_KEY , "PARAMS"      , 0                    , "Module's arguments in JSON format, default is empty []. In Unix bash remember to quote the argument." },
    {"requestId"        , REQUESTID_KEY , "ID"          , OPTION_ARG_OPTIONAL  , "Request Id, default is 'eth-explorer'" },
    {0                  , VERBOSE_KEY   , 0             , OPTION_ARG_OPTIONAL  , "enable verbose logging" },
    {0}
};

/*
key  : can be short option
arg  : option argument
state: arg state
*/
error_t parse_opt(int key,char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key)
    {
    case MODULESS_KEY:
        for(size_t i = 0; i< sizeof(rpc_modules) / sizeof(struct rpc_program); i++)
        {
             printf("MODULE NAME: %*s  | %*s\n", -30, rpc_modules[i].id.name, -20, rpc_modules[i].desc);
        } 
        break;

    case JSONRPC_KEY:
        arguments->json_rpc_url = arg;
        break;
    case NETWORK_KEY:
        arguments->json_rpc_url = getenv(arg);
        if(arguments->json_rpc_url==0)
        {
            fprintf(stderr,"Enviroment variable %s not found.\n",arg);
            fprintf(stderr,"Set it with: export %s=<rpc_url>\n.",arg);
            exit(1);
        }
        break;
    case CALL_KEY:
        arguments->action = "call";
        arguments->module = arg;
        break;
     case PARAMETER_KEY:
        if(arg == NULL)
        {
            set_params(arguments, "[]"); 
        }
        else
        {
            set_params(arguments, arg); 
        }
        break;
     case REQUESTID_KEY:
        if(arg == NULL)
        {
            arguments->requestId = "eth-explorer";
        }
        else
        {
            arguments->requestId = arg;
        }
        break; 
    case VERBOSE_KEY:
        arguments->verbose = 1;
        break;
    }
    return 0; // OK - continue parsing
};

static struct argp argp = {options,parse_opt, args_doc, doc};

json_object* execute_standard_eth_rpc(struct arguments *arg);

int main(int argc, char **argv)
{    
    struct  arguments arguments;
    init_arguments(&arguments);
    argp_parse(&argp,argc,argv,0,0,&arguments);
    init_w_curl();

    if(arguments.action && strcmp(arguments.action,"call")==0)
    {
        exec_program exec_func = execute_standard_eth_rpc;
        size_t size = sizeof(rpc_modules) / sizeof(struct rpc_program);
        for(size_t i = 0; i < size; i++)
        {
            if (strcmp(arguments.module, rpc_modules[i].id.name) == 0) {
                exec_func = rpc_modules[i].execute_rpc_program;
            }
        }
        json_object * result = exec_func(&arguments);
        if(result)
        {
            const char *json_output = json_object_to_json_string(result);
            fprintf(stdout, "%s\n", json_output);
        }
    }
    cleanup_w_curl();
    free_arguments(&arguments);
    return 0;
}

json_object* execute_standard_eth_rpc(struct arguments *arg)
{
    json_object* result = 0;
    if(arg==0)
        return result;
    t_rpcResponse rpcResponse = {0};
    int call_res = make_rpc_call(arg->module, arg, &rpcResponse);
    if(call_res >= 0)
    {
        if(rpcResponse.type == RPC_SUCCESS)
        {
            result = json_tokener_parse(rpcResponse.data.value);
        }
        if(rpcResponse.type == RPC_ERROR)
        {
            fprintf(stderr, "code: %d, message: %s\n", rpcResponse.data.error_info.code, rpcResponse.data.error_info.message);
        }
    }
    return result;
}
