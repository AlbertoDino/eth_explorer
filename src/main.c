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

static char doc[]      = "A simple program to explore a evm blockchain network.";
static char args_doc[] = "!...";


struct rpc_program rpc_modules[] = {
    { "eth_simulate"           , execute_eth_simulate , "Given a TxHas replays the call on the previous block."  },
    { "eth_simulate_latest"    , execute_eth_simulate , "Given a TxHas replays the call on the latest block."  },
    { "eth_trace"              , execute_eth_trace    , "Visualize how a transaction interacts with multiple contracts."  },
};

struct evm_signature_topic common_evm_topics[] = {
{"0xddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef", "Transfer(address,address,uint256)"},
{"0x8c5be1e5ebec7d5bd14f71427d1e84f3dd0314c0f7b2291e5b200ac8c7c3b925", "Approval(address,address,uint256)"},
{"0x17307eab39ab6107e8899845ad3d59bd9653f200f220920489ca2b5937696c31", "ApprovalForAll(address,address,bool)"},
{"0xc3d58168c5ae7397731d063d5bbf3d657854427343f4c083240f7aacaa2d0f62", "TransferSingle(address,address,address,uint256,uint256)"},
{"0x4a39dc06d4c0dbc64b70af90fd698a233a518aa5d07e595d983b8c0526c8f7fb", "TransferBatch(address,address,address,uint256[],uint256[])"},
{"0x6bb7ff708619ba0610cba295a58592e0451dee2622938c8755667688daf3529b", "URI(string,uint256)"},
{"0xd78ad95fa46c994b6551d0da85fc275fe613ce37657fb8d5e3d130840159d822", "Swap(address,uint256,uint256,uint256,uint256,address)"},
{"0x0d3648bd0f6ba80134a33ba9275ac585d9d315f0ad8355cddefde31afa28d0e9", "PairCreated(address,address,address,uint256)"},
{"0x4c209b5fc8ad50758f13e2e1088ba56a560dff690a1c6fef26394f4c03821c4f", "Mint(address,uint256,uint256)"},
{"0xdccd412f0b1252819cb1fd330b93224ca42612892bb3f4f789976e6d81936496", "Burn(address,uint256,uint256,address)"},
{"0x1c411e9a96e071241c2f21f7726b17ae89e3cab4c78be50e062b03a9fffbbad1", "Sync(uint112,uint112)"},
{"0xc42079f94a6350d7e6235f29174924f928cc2ac818eb64fed8004e115fbcca67", "Swap(address,address,int256,int256,uint160,uint128,int24)"},
{"0x783cca1c0412dd0d695e784568c96da2e9c22ff989357a2e8b1d9b2b4e6b7118", "PoolCreated(address,address,uint24,int24,address)"},
{"0x7a53080ba414158be7ec69b987b5fb7d07dee101fe85488f0853ae16239d0bde", "Mint(address,address,int24,int24,uint128,uint256,uint256)"},
{"0x0c396cd989a39f4459b5fa1aed6a9a8dcdbc45908acfd67e028cd568da98982c", "Burn(address,int24,int24,uint128,uint256,uint256)"},
{"0x3067048beee31b25b2f1681f88dac838c8bba36af25bfb2b7cf7473a5847e35f", "Flash(address,address,uint256,uint256,uint256,uint256)"},
{"0x2f8788117e7eff1d82e926ec794901d17c78024a50270940304540a733656f0d", "RoleAdded(bytes32,address,address)"},
{"0xf6391f5c32d9c69d2a47ea670b442974b53935d1edc7fd64eb21e047a839171b", "RoleRevoked(bytes32,address,address)"},
{"0xbd79b86ffe0ab8e8776151514217cd7cacd52c909f66475c3af44e129f0b00ff", "RoleGranted(bytes32,address,address)"},
{"0x8be0079c531659141344cd1fd0a4f28419497f9722a3daafe3b4186f6b6457e0", "OwnershipTransferred(address,address)"},
{"0x62e78cea01bee320cd4e420270b5ea74000d11b0c9f74754ebdbfc544b05a258", "Paused(address)"},
{"0x5db9ee0a495bf2e6ff9c91a7834c1ba4fdd244a5e8aa4e537bd38aeae4b073aa", "Unpaused(address)"},
{"0xcc16f5dbb4873280815c1ee09dbd06736cffcc184412cf7a71a0fdb75d397ca5", "Burn(address,uint256)"},
{"0x0f6798a560793a54c3bcfe86a93cde1e73087d944c0ea20544137d4121396885", "Mint(address,uint256)"},
{"0x9a2e42fd6722813d69113e7d0079d3d940171428df7373df9c7f7617cfda2892", "ProposalCreated(uint256,address,address[],uint256[],string[],bytes[],uint256,uint256,string)"},
{"0x712ae1383f79ac853f8d882153778e0260ef8f03b504e2866e0593e04d2b291f", "ProposalCanceled(uint256)"},
{"0x789cf55be980739dad1d0699b93b58e806b51c9d96619bfa8fe0a28abaa7b30c", "ProposalExecuted(uint256)"},
{"0xb8e138887d0aa13bab447e82de9d5c1777041ecd21ca36ba824ff1e6c07ddda4", "VoteCast(address,uint256,uint8,uint256,string)"},
{"0x4cf4410cc57040e44862ef0f45f3dd5a5e02db8eb8add648d4b0e236f1d07dca", "CallScheduled(bytes32,uint256,address,uint256,bytes,bytes32,uint256)"},
{"0xc2617efa69bab66782fa219543714338489c4e9e178271560a91b82c3f612b58", "CallExecuted(bytes32,uint256,address,uint256,bytes)"},
{"0xbaa1eb22f2a492ba1a5fea61b8df4d27c6c8b5f3971e63bb58fa14ff72eedb70", "Cancelled(bytes32)"},
{"0xe1fffcc4923d04b559f4d29a8bfc6cda04eb5b0d3c460751c2402c5c5cc9109c", "Deposit(address,uint256)"},
{"0x7fcf532c15f0a6db0bd6d0e038bea71d30d808c7d98cb3bf7268a95bf5081b65", "Withdrawal(address,uint256)"},
{"0xe5b754fb1abb7f01b499791d0b820ae3b6af3424ac1c59768edb53f4ec31a929", "Redeem(address,uint256,uint256)"},
{"0x13ed6866d4e1ee6da46f845c46d7e54120883d75c5ea9a2dacc1c4ca8984ab80", "Borrow(address,uint256,uint256,uint256)"},
{"0x1a2a22cb034d26d1854bdc6666a5b91fe25efbbb5dcad3b0355478d6f5c362a1", "RepayBorrow(address,address,uint256,uint256,uint256)"},
{"0x298637f684da70674f26509b10f07ec2fbc77a335ab1e7d6215a4b2484d8bb52", "LiquidateBorrow(address,address,uint256,address,uint256)"},
{"0xde6857219544bb5b7746f48ed30be6386fefc61b2f864cacf559893bf50fd951", "Deposit(address,address,uint256,uint16)"},
{"0x3115d1449a7b732c986cba18244e897a450f61e1bb8d589cd2e69e6c8924f9f7", "Withdraw(address,address,address,uint256)"},
{"0xc6a898309e823ee50bac64e45ca8adba6690e99e7841c45d754e2a38e9019d9b", "Borrow(address,address,address,uint256,uint256,uint256,uint16)"},
{"0xa534c8dbe71f871f9f3530e97a74601fea17b426cae02e1c5aee42c96c784051", "Repay(address,address,address,uint256)"},
{"0x7cc135e0cebb02c3480ae5d74d377283180a2601f8f644edf7987b009316c63a", "ChainlinkRequested(bytes32)"},
{"0x7c2822423ee9387f1a5e88d1a4e6c5e20538f47c0d8c9e25d6e5e04e9c60f5ac", "ChainlinkFulfilled(bytes32)"},
{"0xe1c52dc63b719ade82e8bea94cc41a0d5d28e4aaf536adb5e9cccc9ff8c1aeda", "ChainlinkCancelled(bytes32)"},
{"0x6895c13664aa4f67288b25d7a21d7aaa34916e355fb9b6fae0a139a9085becb8", "AddedOwner(address)"},
{"0xf8d49fc529812e9a7c5c50e69c20f0dccc0db8fa95c98bc58cc9a4f1c1299eaf", "RemovedOwner(address)"},
{"0x9465fa0c962cc76958e6373a993326400c1c94f8be2fe3a952adfa7f60b2ea26", "ApproveHash(bytes32,address)"},
{"0x442e715f626346e8c54381002da614f62bee8d27386535b2521ec8540898556e", "SignMsg(bytes32)"},
{"0xce0457fe73731f824cc272376169235128c118b49d344817417c6d108d155e82", "NewOwner(bytes32,bytes32,address)"},
{"0x335721b01866dc23fbee8b6b2c7b1e14d6f05c28cd35a2c934239f94095602a0", "Transfer(bytes32,address)"},
{"0xd4735d920b0f87494915f556dd9b54c8f309026070caea5c737245152564d266", "NewResolver(bytes32,address)"},
{"0x0424b6fe0d9c3bdbece0e7879dc241bb0c22e900be8b6c168b4ee08bd9bf83bc", "NewTTL(bytes32,uint64)"},
{"0x26f55a85081d24974e85c6c00045d0f0453991e95873f52bff0d21af4079a768", "TokenExchange(address,int128,uint256,int128,uint256)"},
{"0x3f1915775e0c9a38a57a7bb7f1f9005f486fb904e1f84aa215364d567319a58d", "AddLiquidity(address,uint256[],uint256[],uint256,uint256)"},
{"0x5ad056f2e28a8cec232015406b843668c1e36cda598127ec3b8c59b8c72773a0", "RemoveLiquidity(address,uint256[],uint256[],uint256)"},
{"0x2b5508378d7e19e0d5fa338419034731416c4f5b219a10379956f764317fd47e", "RemoveLiquidityOne(address,uint256,uint256,uint256)"},
{"0x908fb5ee8f16c6bc9bc3690973819f32a4d4b10188134543c88706e0e1d43378", "PoolBalanceChanged(bytes32,address,address[],int256[],uint256[])"},
{"0xe5ce249087ce04f05a957192435400fd97868dba0e6a4b4c049abf8af80dae78", "Swap(bytes32,address,address,uint256,uint256)"},
{"0x3c13bc30b8e878c53fd2a36b679409c073afd75950be43d8858768e956fbc20e", "PoolRegistered(bytes32,address,uint8)"},
{"0xd6d4f5681c246c9f42c203e287975af1601f8df8035a9251f79aab5c8f09e2f8", "Swapped(address,address,address,uint256,uint256)"},
{"0x7c98e64bd943448b4e24ef8c2cdec7b8b1275970cfe10daf2a9bfa4b04dce905", "OrderFilled(bytes32,uint256)"},
{"0x06b541ddaa720db2b10a4d0cdac39b8d360425fc073085fac19bc82614677987", "Sent(address,address,address,uint256,bytes,bytes)"},
{"0xf4caeb2d6ca8932a215a353d0703c326ec2d81fc68170f320eb2ab49e9df61f9", "Minted(address,address,uint256,bytes,bytes)"},
{"0xa78a9be3a7b862d26933ad85fb11d80ef66b8f972d7cbba06621d583943a4098", "Burned(address,address,uint256,bytes,bytes)"},
{"0x50546e66e5f44d728365dc3908c63bc5cfeeab470722c1677e3073a6ac294aa1", "RevokedOperator(address,address)"},
{"0x6ef95f06320e7a25a04a175ca677b7052bdd97131872c2192525a629f51be770", "PayeeAdded(address,uint256)"},
{"0xdf20fd1e76bc69d672e4814fafb2c449bba3a5369d8359adf9e05e6fde87b056", "PaymentReleased(address,uint256)"},
{"0x8a74821950f8fcf7c1fcf3b0d2f3f77a44e2c9792f8ac3cc022e9ce9769c8da6", "ERC20PaymentReleased(address,address,uint256)"},
{"0x280ca99bb7c14d3e170471401ed5b0bfc95e4a8c69b3e362c9ab2f95c285c4be", "LogDeposit(address,uint256,uint256,uint256)"},
{"0x5c4eeb02dabf8976016ab414d617f9a162936dcace3cdef8c69ef6e262ad5ae7", "LogWithdrawal(address,uint256,uint256)"},
{"0x2849b43074093a05396b6f2a937dee8565e4a1f46b9f11fc50c7c1fe51b26b0e", "DepositFinalized(address,address,uint256)"},
{"0x73d170910aba9e6d50b102db522b1dbcd796216f5128b445aa2135272886497e", "WithdrawalInitiated(address,address,uint256)"},
{"0x7f26b83ff96e1f2b6a682f133852f6798a09c465da95921460cefb3847402498", "Initialized(uint8)"},
{"0x8a8bae378cb731c5c40b632330c6836c2f916f48edb967699c86736f9a6a76ef", "BeaconUpgraded(address)"},
{"0xbc7cd75a20ee27fd9adebab32041f755214dbc6bffa90cc0225b39da2e5c2d3b", "Upgraded(address)"},
{"0x4f51faf6c4561ff95f067657e43439f0f856d97c04d9ec9070a6199ad418e235", "AdminChanged(address,address)"},
{"0xf279e6a1f5e320cca91135676d9cb6e44ca8a08c0b88342bcdb1144f6511b568", "TokensPurchased(address,address,uint256,uint256)"},
{"0xe2403640ba68fed3a2f88b7557551d1993f84b99bb10ff833f0cf8db0c5e0486", "Staked(address,uint256)"},
{"0x7084f5476618d8e60b11ef0d7d3f06914655adb8793e28ff7f018d4c76d505d5", "Withdrawn(address,uint256)"},
{"0x8030e83b04691597bafa81915d9639f6634c42bb1f8d13e6a29bdb02b0f7c51b", "Snapshot(uint256)"},
{"0xdec2bacdd2f05b59de34da9b523dff8be42e5e38e818c82fdb0bae774387a724", "DelegateChanged(address,address,address)"},
{"0x3134e8a2e6d97e929a7e54011ea5485d7d196dd5f0ba4d4ef95803e8e3fc257f", "DelegateVotesChanged(address,uint256,uint256)"},
{"0xf4caeb2d6ca8932a215a353d0703c326ec2d81fc68170f320eb2ab49e9df61f9", "AuthorizedOperator(address,address)"},
{"0x6ef95f06320e7a25a04a175ca677b7052bdd97131872c2192525a629f51be770", "PaymentReceived(address,uint256)"},
{"0xe6497e3ee548a3372136af2fcb0696db31fc6cf20260707645068bd3fe97f3c4", "Approval(address,address)"},
{"0xbb35783b676aee31b9f38743e0ece4884ba1e251c842a8bcee3f3c8d31fce22c", "Deposit(address,uint256)"},
{"0xf279e6a1f5e320cca91135676d9cb6e44ca8a08c0b88342bcdb1144f6511b568", "Withdraw(address,uint256)"},
{"0x2717ead6b9200dd235aad468c9809ea400fe33ac69b5bfaa6d3e90fc922b6398", "RoleAdminChanged(bytes32,bytes32,bytes32)"},
{"0xe2403640ba68fed3a2f88b7557551d1993f84b99bb10ff833f0cf8db0c5e0486", "RewardPaid(address,uint256)"},
{"0x17307eab39ab6107e8899845ad3d59bd9653f200f220920489ca2b5937696c31", "Log(string)"}
};

char * get_common_signature(const char * hash)
{
    char * topic_name = 0;    
    size_t size = sizeof(common_evm_topics) / sizeof(struct evm_signature_topic);
    for (size_t i = 0; i < size; i++)
    {
       if(strcmp(common_evm_topics[i].hash,hash) == 0)
        return common_evm_topics[i].signature_name;
    }

    return topic_name;
}

/* Networks */ 

/*Parse Arguments*/
#define MODULESS_KEY   2
#define JSONRPC_KEY   'j'
#define CALL_KEY      'c'
#define PARAMETER_KEY 'p'
#define REQUESTID_KEY 'i'
#define VERBOSE_KEY   'v'

/* Arguments */
        
struct argp_option options[] ={
    /*
    1st  @ - long option
    2nd  @ - short option, if a number it will be the associated key of the long option
    3rd  @ - there is a mandatory argument to the option
    4th  @ - option flags 0=mandatory , OPTION_ARG_OPTIONAL * if long options required a = between the option and the argumnt
    5th  @ - description to show in the --help
    */
    {"builtin-modules"  , MODULESS_KEY  , 0           , 0                    , "Gets the list of modules embedded in the program" },
    {"jsonrpc"          , JSONRPC_KEY   , "URL"       , 0                    , "Specify Json Rpc url" },
    {"call"             , CALL_KEY      , "MODULENAME", 0                    , "Call a specific module via RPC" },
    {"parameters"       , PARAMETER_KEY , "PARAMS"    , 0                    , "Module's arguments in JSON format, default is empty []. In Unix bash remember to quote the argument." },
    {"requestId"        , REQUESTID_KEY , "ID"        , OPTION_ARG_OPTIONAL  , "Request Id, default is 'eth-explorer'" },
    {0                  , VERBOSE_KEY   , 0           , OPTION_ARG_OPTIONAL  , "enable verbose logging" },
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
             printf("name: %*s  - desc: %*s\n", -30, rpc_modules[i].id.name, -20, rpc_modules[i].desc);
        } 
        break;

    case JSONRPC_KEY:
        arguments->json_rpc_url = arg;
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

    if(strcmp(arguments.action,"call")==0)
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
            result = rpcResponse.data.value;
        }

        if(rpcResponse.type == RPC_ERROR)
        {
            fprintf(stderr, "code: %d, message: %s\n", rpcResponse.data.error_info.code, rpcResponse.data.error_info.message);
        }
    }
   
    return result;
}
