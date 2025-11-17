#include "eth.h"
#include "w_curl.h"

void init_arguments(struct arguments *arguments) {
    arguments->verbose       = 0;
    arguments->action        = 0;
    arguments->module        = 0;
    arguments->requestId     = "etherexplorer";
    arguments->params        = 0;
    arguments->rpc_responses = json_object_new_object();
    json_object_object_add(arguments->rpc_responses, "logs" ,json_object_new_array());
}

void free_arguments(struct arguments *arguments) {
    arguments->verbose   = 0;
    arguments->action    = 0;
    arguments->module    = 0;
    arguments->requestId = 0;

    if(arguments->params)
        free(arguments->params);

    //json_object_put(arguments->rpc_responses);
    arguments->rpc_responses = 0;
}    

void set_params(struct arguments  * arg,const char* params)
{
    if( arg->params)
        free(arg->params);
    arg->params = strdup(params);
}

void arg_add_rpc_response(struct arguments  * arg, const char* module, json_object * parameters, json_object * response)
{
    json_object * rpc_param_copy;
    json_object * rpc_response_copy;

    json_object_deep_copy(parameters , &rpc_param_copy   , 0);
    json_object_deep_copy(response   , &rpc_response_copy, 0);

    json_object * rpc_log = json_object_new_object();

    json_object_object_add(rpc_log, "module"   , json_object_new_string(module));
    json_object_object_add(rpc_log, "params"   , rpc_param_copy);
    json_object_object_add(rpc_log, "response" , rpc_response_copy);

    json_object * logs =  json_object_object_get(arg->rpc_responses,"logs");
    json_object_array_add(logs, rpc_log);
}

json_object* build_rpc_payload(const char * module, struct arguments *arg)
{
    json_object *jparams = 0;
    if(arg->params == 0)
    {
        jparams = json_object_new_array();
    }
    else
    {
        jparams = json_tokener_parse(arg->params);
        if(jparams == 0)
        {
            fprintf(stderr, "Invalid params argument: %s. Not a json object\n", arg->params);
            return 0;
        }
    }

    json_object *json_rpc_obj = json_object_new_object();
    json_object_object_add(json_rpc_obj, "jsonrpc", json_object_new_string("2.0"));
    json_object_object_add(json_rpc_obj, "method", json_object_new_string(module));
    
    json_object_object_add(json_rpc_obj, "params", jparams);
    json_object_object_add(json_rpc_obj, "id", json_object_new_string(arg->requestId));

    return json_rpc_obj;
}

long long htol (const char * hex)
{
    char *hptr = 0;
    errno = 0; 
    long long num = strtoll(hex,&hptr,16);
    if (errno != 0 || *hptr != '\0') 
    {
        fprintf(stderr, "Error: Invalid hex string for block number: %s\n", hex);
        return 0;
    }
    return num;
}

int ltohex (long long value, char * strHex)
{
    snprintf(strHex,sizeof(strHex), "%#llx",value);
    return 0;
}

int make_rpc_call(const char * module, struct arguments *arg, t_rpcResponse *response)
{
    const char *headers[] = {
                "Content-Type: application/json",
                "Accept: application/json"
    };

    json_object *json_rpc_obj = build_rpc_payload(arg->module, arg);

    if(!json_rpc_obj)
    {
        fprintf(stderr,"Error generating json-payload.\n");
        return -1;
    }

    const char *json_payload = json_object_to_json_string(json_rpc_obj);

    t_response http_response = w_curl_http_post(arg->json_rpc_url, json_payload, headers, 2, arg->verbose);

    int parseResult = parse_json_rpc_response(http_response.memory, response, arg->verbose);

    json_object_put(json_rpc_obj);
    free_t_response(&http_response);

    return parseResult;
}
