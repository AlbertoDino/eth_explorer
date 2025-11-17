sudo apt-get update
sudo apt-get install build-essential cmake pkg-config
sudo apt-get install libcurl4-openssl-dev libjson-c-dev


# Library Dependency

for http/https

> sudo apt-get install libcurl4-openssl-dev libjson-c-dev


Compile 
> gcc main.c eth.c w_curl.c rpc_parser.c eth_simulate.c eth_trace.c -lcurl -ljson-c -o ./eth-explorer -g


Example
> ./eth-explorer -j https://ethereum-sepolia-rpc.publicnode.com -c eth_blockNumber -v
> ./eth-explorer -j https://ethereum-sepolia-rpc.publicnode.com -c eth_getTransactionByHash -p '["0x6c1b120f327b35e0e6592cfd767e2f43703f4838da3f0811a2c11e97d7867f30"]' -v
> ./eth-explorer -j https://ethereum-sepolia-rpc.publicnode.com -c eth_trace -p '["0x6c1b120f327b35e0e6592cfd767e2f43703f4838da3f0811a2c11e97d7867f30"]' -v
# JSON RPC 2.0

### eth_call

invoke a smart contract function locally for read only queries

>TODO

### eth_blockNumber

 Retrieve the latest Ethereum block number using eth_blockNumber.

> curl -X POST -H "Content-Type: application/json" --data @rpc_requests/eth_blockNumber.json https://eth.llamarpc.com

### eth_accounts

retrieve a list of Ethereum accounts owned by the client

> curl -X POST -H "Content-Type: application/json" --data @rpc_requests/eth_accounts.json https://ethereum-sepolia-rpc.publicnode.com


### eth_coinbase

retrieve the primary Ethereum client address using eth_coinbase

> curl -X POST -H "Content-Type: application/json" --data @rpc_requests/eth_coinbase.json https://ethereum-sepolia-rpc.publicnode.com

## rpc_modules

returns all rpc_modules enabled JSON-RPC APIs in the Ethereum node.

> curl -X POST -H "Content-Type: application/json" --data @rpc_requests/rpc_modules.json https://ethereum-sepolia-rpc.publicnode.com