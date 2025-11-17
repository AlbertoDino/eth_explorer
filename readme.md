## Eth-Explorer
A simple CLI utility for making RPC calls to an Ethereum network.
returns the entire json response in the `stdout`
#### Dependencies
 - Debian:
 
>`sudo apt-get update`
`sudo apt-get install build-essential cmake pkg-config`
`sudo apt-get install libcurl4-openssl-dev libjson-c-dev`

#### Build
> gcc main.c common_topics.c eth.c w_curl.c rpc_parser.c eth_simulate.c eth_trace.c -lcurl -ljson-c -o ./eth-explorer -g

#### Use
##### With standard JSON RPC 2.0 or built-in programs

| eth_blockNumber
 `./eth-explorer -j https://ethereum-sepolia-rpc.publicnode.com -c eth_blockNumber -v`

| eth_getTransactionByHash
 `./eth-explorer -j https://ethereum-sepolia-rpc.publicnode.com -c eth_getTransactionByHash -p 
'["0x6c1b120f327b35e0e6592cfd767e2f43703f4838da3f0811a2c11e97d7867f30"]' -v`

| eth_trace (built-in program)
 `./eth-explorer -j https://ethereum-sepolia-rpc.publicnode.com -c eth_trace -p '["0x6c1b120f327b35e0e6592cfd767e2f43703f4838da3f0811a2c11e97d7867f30"]' -v`
output:
```
from: 
 └── 0x12e60f3971a3cb26b1a65388d0b0ab2e4eadf5a1 
to: 
 └── 0xe7db6d870747d152f8442e7cc7598d480935c7db | 🔋 Gas: 400538 
      ├── 0x102299aa5570868384530a4eee02909bb87aff53 [0x828499e9] 🔄 Beacon Proxy (Call)
      ├── 0xea86ce1cfb0e2c60d155822495edd7003534e351 [0xddf252ad] 🔄 Beacon Proxy (Transfer(address,address,uint256))
      │  └── 0xea86ce1cfb0e2c60d155822495edd7003534e351 [0x8c5be1e5] 🔄 Beacon Proxy (Approval(address,address,uint256))
      └── 0xe7db6d870747d152f8442e7cc7598d480935c7db [0x842fb24a] (Call)
```
