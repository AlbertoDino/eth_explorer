#include "eth_simulate.h"

int set_eth_call_parameters(int use_latest_block, t_rpcResponse * response,struct  arguments *arg);

/* -- eth_simulate -- */

json_object* execute_eth_simulate (struct arguments *arg)
{
    int terminated             = 0;
    json_object   * result     = 0;
    
    int use_latest_block = (strcmp(arg->module, "eth_simulate_latest") == 0);

    arg->module = "eth_getTransactionByHash";
    do {
        t_rpcResponse rpc_response = {0};
        terminated                 = 1;
        if(make_rpc_call(arg->module, arg, &rpc_response) == 0)
        {
            if(rpc_response.type == RPC_SUCCESS)
            {
                if(strcmp(arg->module, "eth_call") == 0)
                {
                    result = rpc_response.data.value;
                }  

                if(strcmp(arg->module, "eth_getTransactionByHash") == 0)
                {
                    if(set_eth_call_parameters(use_latest_block, &rpc_response,arg) != 0)
                        return 0;

                    arg->module = "eth_call";
                    terminated  = 0;
                }          
            }

            if(rpc_response.type == RPC_ERROR)
            {
                fprintf(stderr, "code: %d, message: %s\n", rpc_response.data.error_info.code, rpc_response.data.error_info.message);
            }
        }        
        free_rpc_response(&rpc_response);

    } while (terminated==0);
    
    return result;
}

int set_eth_call_parameters(int use_latest_block, t_rpcResponse * rpc_response, struct arguments *arg)
{
    // ** Stage 1 eth_getTransactionByHash ** 
    json_object *eth_call_params = json_object_new_array_ext(2); 
    json_object *eth_param_obj   = json_object_new_object();

    json_object *to          = NULL;
    json_object *from        = NULL;
    json_object *blocknumber = NULL;
    json_object *input       = NULL;
    json_object *value       = NULL;

    json_object_object_get_ex(rpc_response->data.value ,"from"        , &from);
    json_object_object_get_ex(rpc_response->data.value ,"to"          , &to);
    json_object_object_get_ex(rpc_response->data.value ,"blockNumber" , &blocknumber);
    json_object_object_get_ex(rpc_response->data.value ,"input"       , &input);
    json_object_object_get_ex(rpc_response->data.value ,"value"       , &value);

    json_object_object_add(eth_param_obj, "to"      , to);
    json_object_object_add(eth_param_obj, "from"    , from);
    json_object_object_add(eth_param_obj, "input"   , input);
    json_object_object_add(eth_param_obj, "value"   , value);

    json_object_array_add(eth_call_params, eth_param_obj);

    if(use_latest_block)
    {
        json_object_array_add(eth_call_params, json_object_new_string("latest"));
    }
    else
    {
        const char* hex_blocknumber = json_object_get_string(blocknumber);

        // make conversion from Hex to Decimal
        char *endptr = 0;
        errno = 0; 
        long long dec_blocknumber   = strtoll(hex_blocknumber,&endptr,16);
        if (errno != 0 || *endptr != '\0') 
        {
            fprintf(stderr, "Error: Invalid hex string for block number: %s\n", hex_blocknumber);
            return 0;
        }

        dec_blocknumber--;
        char hex_buffer[64];
        snprintf(hex_buffer,sizeof(hex_buffer), "%#llx",dec_blocknumber);
        json_object_array_add(eth_call_params, json_object_new_string(hex_buffer));
    }

    set_params(arg, json_object_to_json_string(eth_call_params));
    json_object_put(eth_call_params);

    return 0;
}