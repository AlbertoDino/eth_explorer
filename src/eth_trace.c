#include "eth_trace.h"

#define EIP1967_IMPLEMENTATION_SLOT "0x360894a13ba1a3210667c828492db98dca3e2076cc3735a920a3ca505d382bbc"
#define EIP1967_ADMIN_SLOT          "0xb53127684a568b3173ae13b9f8a6016e243e63b6e8ee1178d6a717850b5d6103"
#define EIP1967_BEACON_SLOT         "0xa3f0ad74e5423aebfd80d3ef4346578335a9a72aeaee59ff6cb3582b35133d50"

#define CONTRACT_TYPE_PROXY          1
#define CONTRACT_TYPE_IMPLEMENTATION 2


struct  {
    char * slot;
    char * type;
} ProxySlots[] = {
    { EIP1967_IMPLEMENTATION_SLOT , "Proxy" },
    { EIP1967_ADMIN_SLOT          , "Proxy" },
    { EIP1967_BEACON_SLOT         , "Beacon Proxy" }
};

//================================  eth_trace Context ================================\\

int                 set_eth_getLogs_parameters (t_rpcResponse * response,struct  arguments *arg);

struct EthcallNode* build_call_tree            (struct EthcallNode *root, json_object * rpc_result,struct  arguments *arg);

void                print_call_tree            (struct EthcallNode * root);

int                 get_bytecode               (const char        * address
                                                , const char       * blocknumber
                                                , struct arguments * arg
                                                , t_rpcResponse    * out_response);

int                 get_StoregeAt              (const char       *address
                                                , const char       *index
                                                , const char       *blocknumber
                                                , struct arguments *arg 
                                                , t_rpcResponse    *out_response);
// fastst for < 32 elements
void                insertion_sort             (long long *buffer, size_t size);

json_object* execute_eth_trace (struct arguments  * arg)
{  
    t_rpcResponse rpc_txByHash  = {0};
    t_rpcResponse rpc_txReceipt = {0};
    t_rpcResponse rpc_getlogs   = {0};

    json_object   *txReceipt_result = 0;


    
    /* stage 1. eth_getTransactionByHash */
    arg->module = "eth_getTransactionByHash";
        
    if(make_rpc_call(arg->module, arg, &rpc_txByHash) != 0)
    {
        return 0;
    }

    /* stage 2. eth_getTransactionReceipt */
    arg->module = "eth_getTransactionReceipt";
    if(make_rpc_call(arg->module, arg, &rpc_txReceipt) != 0)
    {
        return 0;
    }

    /* stage 3. eth_getLogs */
    /*
    if(set_eth_getLogs_parameters(&rpc_txReceipt, arg)!=0)
    {
        return 0;
    }

    arg->module = "eth_getLogs";    
    if(make_rpc_call(arg->module, arg, &rpc_getlogs) != 0)
    {
        return 0;
    }
    */

    /* build call tree from logs */
    struct EthcallNode *root  = alloc_node();

    txReceipt_result          = json_tokener_parse(rpc_txReceipt.data.value);

    json_object *address_from = json_object_object_get(txReceipt_result , "from");
    json_object *address_to   = json_object_object_get(txReceipt_result , "to");
    json_object *tx_hash      = json_object_object_get(txReceipt_result , "transactionHash");
    json_object *gas_used     = json_object_object_get(txReceipt_result , "gasUsed");

    root->transaction_hash    = strdup(json_object_get_string(tx_hash));
    root->address_from        = strdup(json_object_get_string(address_from));
    root->address_to          = strdup(json_object_get_string(address_to));
    root->gas_used            = htol(json_object_get_string(gas_used));

    struct EthcallNode* all_logs_tree = build_call_tree(root, txReceipt_result, arg);
    print_call_tree(root);

    // free res.
    json_object_put(txReceipt_result);
    free_rpc_response(&rpc_txByHash);
    free_rpc_response(&rpc_txReceipt);
    free_rpc_response(&rpc_getlogs);
    free_ethcallNode(root);

    return 0;
}

int set_eth_getLogs_parameters(t_rpcResponse * response,struct  arguments *arg)
{    
    json_object * rpc_response           = json_tokener_parse(response->data.value);
    json_object * rpc_response_blockHash = json_object_object_get(rpc_response, "blockHash");            
    const char  * hash                   = json_object_get_string(rpc_response_blockHash);

    json_object * eth_new_params = json_object_new_array();
    json_object * block_hash     = json_object_new_object();
    
    json_object_object_add(block_hash, "blockhash" , json_object_new_string(hash));
    json_object_array_add(eth_new_params, block_hash);

    set_params(arg, json_object_to_json_string(eth_new_params));

    json_object_put(eth_new_params);
    json_object_put(rpc_response);
    return 0;
}

struct EthcallNode * build_call_tree(struct EthcallNode *root, json_object * rpc_result,struct  arguments *arg)
{
    json_object *logs = json_object_object_get(rpc_result, "logs");
    size_t logs_count = json_object_array_length(logs);

    long long *log_index_list = malloc(sizeof(long long)*logs_count);
    size_t num_events         = 0;

    struct EthcallNode *logslist = alloc_node();

    for(size_t i =0; i < logs_count; i++) {               
        
        json_object *log    = json_object_array_get_idx (logs, i);
        json_object *txHash = json_object_object_get    (log, "transactionHash");

        if(strcmp(root->transaction_hash, json_object_get_string(txHash))!=0)
        {
            //this log is not part of our transaction
            continue;
        }

        json_object *address      = json_object_object_get(log, "address");        
        json_object *topics       = json_object_object_get(log, "topics");
        json_object *log_index    = json_object_object_get(log, "logIndex");
        size_t num_topics         = json_object_array_length(topics)-1;

        struct EthcallNode *child = alloc_node(); 
        if(num_topics>0)
        {
            child->num_topics = num_topics;
            child->topics     = malloc(sizeof(char*)*num_topics);
            for (size_t i = 0; i < num_topics; i++)
            {
                // skiping topic0
                child->topics[i] = strdup(json_object_get_string(json_object_array_get_idx(topics,1+i)));
            }
        }
        
        child->log_index    = htol(json_object_get_string(log_index));
        child->address_to   = strdup (json_object_get_string(address));        
        const char  *topic0 = json_object_get_string(json_object_array_get_idx(topics,0)); // topic0 is the complete event signature hash
        child->call_type    = get_common_signature(topic0);
        child->selector     = malloc(sizeof(char)*11);
        strncpy(child->selector, topic0, sizeof(char)*10);
        child->selector[10] = '\0';

        if(!child->call_type) {
            child->call_type = strdup ("Call");
        }

        // Check if the contract is a proxy
       
        size_t proxy_count = sizeof(ProxySlots)/sizeof(ProxySlots[0]);
        for (size_t i = 0; i < proxy_count; i++)
        {
            t_rpcResponse rpc_getStorage = {0};
            if(get_StoregeAt(child->address_to, ProxySlots[i].slot, "latest", arg, &rpc_getStorage)==0)
            {
                json_object * response_getStorage = json_tokener_parse(rpc_getStorage.data.value);
                const char * slot_address         = json_object_get_string(response_getStorage);
                if(strcmp(json_object_get_string(response_getStorage),"0x0000000000000000000000000000000000000000000000000000000000000000")!=0)
                {
                    child->is_proxy   = CONTRACT_TYPE_PROXY;
                    child->proxy_type = strdup(ProxySlots[i].type);

                    char address[64] = {0};
                    address[0] = '0';
                    address[1] = 'x';
                    size_t len = strlen(slot_address);
                    strncpy(&address[2],&slot_address[len-40], 40);

                    struct EthcallNode *child_proxy_impl = alloc_node();
                    child_proxy_impl->address_to = strdup(address);
                    child_proxy_impl->is_proxy   = CONTRACT_TYPE_IMPLEMENTATION;
                    child_proxy_impl->proxy_type = strdup("Implementation");

                    add_child(child,child_proxy_impl);     

                }
                json_object_put(response_getStorage);
            }
            free_rpc_response(&rpc_getStorage);
        }

        add_child(logslist, child);     

        log_index_list[num_events] = child->log_index;
        num_events++;   
    }

    insertion_sort(log_index_list,num_events);

    struct EthcallNode * prev_node_call = 0;
    for (size_t i = 0; i < num_events; i++)
    {
        long long log_index            = log_index_list[i];
        struct EthcallNode * node_call = find_node_from_logindex(logslist,log_index);
        if (node_call == 0 )
        {
            fprintf(stderr,"Cannot find node call");
            exit(1);
        }

        struct EthcallNode *address_node = find_node_from_address(root, node_call->address_to);        
        struct EthcallNode *parent       = address_node != 0 ? address_node : root;

        add_child(parent, node_call);     

    }
        
    return root;
}

int get_bytecode( const char       * address
                , const char       * blocknumber
                , struct arguments * arg
                , t_rpcResponse    * out_response)
{
    arg->module = "eth_getCode";

    json_object * eth_new_params = json_object_new_array();
    json_object_array_add(eth_new_params,json_object_new_string(address));
    json_object_array_add(eth_new_params,json_object_new_string(blocknumber));

    set_params(arg, json_object_to_json_string(eth_new_params));
    json_object_put(eth_new_params);
    if(make_rpc_call(arg->module, arg, out_response) != 0)
    {
        return -1;
    }

    return 0;
}

int get_StoregeAt ( const char       *address
                  , const char       *index
                  , const char       *blocknumber
                  , struct arguments *arg 
                  , t_rpcResponse    *out_response)
{
    arg->module = "eth_getStorageAt";

    json_object * eth_new_params = json_object_new_array();
    json_object_array_add(eth_new_params, json_object_new_string(address));
    json_object_array_add(eth_new_params, json_object_new_string(index));
    json_object_array_add(eth_new_params, json_object_new_string(blocknumber));

    set_params(arg, json_object_to_json_string(eth_new_params));
    json_object_put(eth_new_params);

    if(make_rpc_call(arg->module, arg, out_response) != 0)
    {
        return -1;
    }
    return 0;
}

void print_call_tree_child (struct EthcallNode * node,const char * parent_prefix, int deep, int is_last)
{
    if(!node)
        return;

    const char * local_prefix = is_last == 1 ? ( node->is_proxy != CONTRACT_TYPE_IMPLEMENTATION ? "└──" : "  ╰╾─" ) :  ( node->is_proxy != CONTRACT_TYPE_IMPLEMENTATION ? "├──" : "┞╾─" );

    //fprintf(stdout,"%*s" , (3*deep)," ");
    fprintf(stdout,"%s"  , parent_prefix);    
    fprintf(stdout,"%s"  , local_prefix);
    fprintf(stdout," %s " , node->address_to);

    if(node->gas_used > 0)
    {
        int gas_used = node->gas_used;
        fprintf(stdout,"| 🔋 Gas: %d " , gas_used);
    }

    if(node->selector)
        fprintf(stdout,"[%s] ", node->selector);

    if(node->log_index!=0)
        fprintf(stdout,"%lld ", node->log_index);

    if(node->is_proxy == CONTRACT_TYPE_PROXY)
    {
        fprintf(stdout,"🔄");        
    }

    if(node->is_proxy == CONTRACT_TYPE_IMPLEMENTATION)
    {
        fprintf(stdout,"🔗");        
    }

    if(node->proxy_type != 0)
    {
        fprintf(stdout," %s ",node->proxy_type);        
    }

    if(node->call_type)
        fprintf(stdout,"(%s)", node->call_type);

    fprintf(stdout,"\n");

    deep++;
    for (size_t i = 0; i < node->num_children; i++)
    {
        struct EthcallNode * child = node->children[i];

        // create prefix based on parent for correct alignment 
        char parent_prefix_buffer[64] = {0};                     
        sprintf(parent_prefix_buffer,"%*s%s%s", (3* (deep==1 ? 1 : 0)), "", parent_prefix , is_last ? "" : "│  ");

        print_call_tree_child(child, parent_prefix_buffer, deep, (i == node->num_children-1) ? 1 : 0 );              
    }
}

void print_call_tree (struct EthcallNode * root)
{
    fprintf(stdout,"from: \n"); 
    fprintf(stdout," └── %s \n", root->address_from); 
    fprintf(stdout,"to: \n"); 
    int deep = 0;
    print_call_tree_child(root, "   ", deep, 1);
    
}

//================================  Ethcall node funcs ================================\\

struct EthcallNode* alloc_node()
{
    struct EthcallNode *node = malloc(sizeof(*node));
    init_EthcallNode(node);
    return node;
}

void init_EthcallNode(struct EthcallNode* node)
{
    node->transaction_hash = 0;
    node->address_from    = 0;
    node->address_to      = 0;
    node->gas_used        = 0;
    node->call_type       = 0;
    node->topic_signature = 0;
    node->selector        = 0;
    node->is_proxy        = 0;
    node->log_index       = 0;
    node->proxy_type      = 0;
    node->children        = 0;
    node->num_children    = 0;
    node->data            = 0;
    node->topics          = 0;
    node->num_topics      = 0;
}

void free_ethcallNode(struct EthcallNode* node)
{
    if(node->transaction_hash)
        free(node->transaction_hash);
    if(node->address_from)
        free(node->address_from);
    if(node->address_to)
        free(node->address_to);
    if(node->call_type)
        free(node->call_type);        
    if(node->topic_signature)
        free(node->topic_signature);
    if(node->selector)
        free(node->selector);
    if(node->proxy_type)
        free(node->proxy_type);
    for (size_t i = 0; i < node->num_children; i++)
    {
       free(node->children[i]);
    }
    if(node->children)
        free(node->children);

    node->num_children = 0;
    for (size_t i = 0; i < node->num_topics; i++)
    {
       free(node->topics[i]);
    }
    if(node->topics)
        free(node->topics);
    node->num_topics = 0;
    if(node->data)
        json_object_put(node->data);
    
    init_EthcallNode(node);
}

struct EthcallNode* find_node_from_logindex(struct EthcallNode* node,long long logIndex)
{
     if(node==0)
        return 0;

    if(node->log_index == logIndex)
        return node;
    
    if(node->num_children>0)
    {
        for (size_t i = 0; i < node->num_children; i++)
        {
           struct EthcallNode* found = find_node_from_logindex(node->children[i],logIndex);
           if(found)
              return found;
        }
    }
    return 0;
}

struct EthcallNode* find_node_from_address(struct EthcallNode* node,const char* address)
{
    if(node==0)
        return 0;

    for (size_t i = 0; i < node->num_topics; i++)
    {
        size_t topic_len = strlen(node->topics[i]);
        if(topic_len>40)
        {
            char buffer[64] = {0};
            strncpy(&buffer[0],&(node->topics[i][topic_len-40]),40);
            buffer[40] = '\0';            
            if(strcmp(buffer,&address[2]) == 0)
                return node;
        }
    }
    

    if (strcmp(node->address_to, address) == 0) 
    {
        return node;
    }  
    if(node->num_children>0)
    {
        for (size_t i = 0; i < node->num_children; i++)
        {
           struct EthcallNode* found = find_node_from_address(node->children[i],address);
           if(found)
              return found;
        }
    }
    return 0;
}

void add_child(struct EthcallNode* parent,struct EthcallNode* child)
{
    parent->children = realloc(parent->children, sizeof(struct EthcallNode*)*(parent->num_children+1));
    parent->children[parent->num_children] = child;
    parent->num_children ++;
}

 void insertion_sort(long long *buffer, size_t size)
 {
    for (size_t i = 0; i < size; i++)
    {
        long long val = buffer[i];
        int j = i - 1;

        while (j >=0 && buffer[j] > val) {
            buffer[j +1] = buffer[j];
            j --;
        }
        buffer[j + 1 ] = val;
    }    
 }