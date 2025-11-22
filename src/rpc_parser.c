#include "rpc_parser.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void free_rpc_response(t_rpcResponse* response)
{
    if(response->id)
    {
        free(response->id);
        response->id =0;
    }

    switch (response->type)
    {
    case RPC_SUCCESS:
        if(response->data.value) 
        {
            free(response->data.value);
            response->data.value = 0;
        }
        break;
    
    case RPC_ERROR:
        if(response->data.error_info.message) 
        {
            free(response->data.error_info.message);
            response->data.error_info.message = 0;
        }
        break;
    }
}

int parse_json_rpc_response(const char *jstring, t_rpcResponse *response, int verbose)
{
    if(!jstring || !response)
        return -1;

    json_object *root    = NULL;
    json_object *jId     = NULL;
    json_object *jResult = NULL;
    json_object *jError  = NULL;

    response->id                      = 0;
    response->type                    = 0;
    response->data.value              = NULL;
    response->data.error_info.code    = 0;
    response->data.error_info.message = 0;

    root = json_tokener_parse(jstring);

    int returnCode = -1;

    if(!root)
    {
        if(verbose)
            printf("RpcParser> Invalid JSON string.\n");
        goto cleanup;
    }
    
    if(json_object_get_type(root) != json_type_object) {
        if(verbose)
            printf("RpcParser> JSON-RPC response is not an object.\n");
        goto cleanup;
    }

    // -- check id
    if(!json_object_object_get_ex(root,"id", &jId)){
        if(verbose)
            printf("RpcParser> JSON-RPC response 'id' missing.\n");
        goto cleanup;
    }

    if(json_object_get_type(jId) != json_type_string){
        if(verbose)
            printf("RpcParser> JSON-RPC response 'id' is not an string.\n");
        goto cleanup;
    }
    
    const char *raw_id = json_object_get_string(jId);
    response->id       = strdup(raw_id);

    // -- check result or error
    json_bool has_result = json_object_object_get_ex(root,"result",&jResult);
    json_bool has_error  = json_object_object_get_ex(root,"error",&jError);

    if(has_result && !has_error) {
        response->type       = RPC_SUCCESS;
        response->data.value = strdup(json_object_to_json_string(jResult));
    }
    else if(!has_result && has_error) {
        response->type                    = RPC_ERROR;
        response->data.error_info.code    = 0;
        response->data.error_info.message = 0;

        if(json_object_get_type(jError) != json_type_object) {
            if(verbose)
                printf("RpcParser> JSON-RPC 'error' field is not an object.\n");
            goto cleanup;
        }   
        
        json_object *jcode, *jmessage;
        if(!json_object_object_get_ex(jError, "code", &jcode) || json_object_get_type(jcode)!= json_type_int)
        {
            if(verbose)
                printf("RpcParser> JSON-RPC 'code' field missing or invalid. Expecting int. \n");
            goto cleanup;
        }

        response->data.error_info.code = json_object_get_int(jcode);

        if(!json_object_object_get_ex(jError, "message", &jmessage) || json_object_get_type(jmessage) != json_type_string)
        {
            if(verbose)
                printf("RpcParser> JSON-RPC 'message' field missing or invalid. Expecting string. \n");
            goto cleanup;
        }

        const char *raw_message           = json_object_get_string(jmessage);
        response->data.error_info.message = strdup(raw_message);

    }

    returnCode = 0;

    cleanup:

    if(root) 
        json_object_put(root); //free all internal json objects
        
    return returnCode;
}