# ETH-Explorer

A lightweight CLI utility for making RPC calls to Ethereum networks. Returns the complete JSON response to `stdout`.

## Prerequisites

### Debian/Ubuntu

```bash
sudo apt-get update
sudo apt-get install build-essential cmake pkg-config
sudo apt-get install libcurl4-openssl-dev libjson-c-dev
```

## Building

Compile the project using GCC:

```bash
gcc main.c common_topics.c eth.c w_curl.c rpc_parser.c eth_simulate.c eth_trace.c \
    -lcurl -ljson-c -o eth-explorer -g
```

## Usage

### Standard JSON-RPC 2.0 Methods

**Get current block number:**

```bash
./eth-explorer -j https://ethereum-sepolia-rpc.publicnode.com -c eth_blockNumber -v
```

**Get transaction by hash:**

```bash
./eth-explorer -j https://ethereum-sepolia-rpc.publicnode.com \
    -c eth_getTransactionByHash \
    -p '["0x6c1b120f327b35e0e6592cfd767e2f43703f4838da3f0811a2c11e97d7867f30"]' -v
```

### Built-in Programs

**Trace transaction execution (`eth_trace`):**

```bash
./eth-explorer -j https://ethereum-sepolia-rpc.publicnode.com \
    -c eth_trace \
    -p '["0x6c1b120f327b35e0e6592cfd767e2f43703f4838da3f0811a2c11e97d7867f30"]' -v
```

#### Example Output

```
from: 
 └── 0x12e60f3971a3cb26b1a65388d0b0ab2e4eadf5a1 
to: 
   └── 0xe7db6d870747d152f8442e7cc7598d480935c7db | 🔋 Gas: 852062 
      ├── 0x4b793496f1ce172f5b46b032b801fd07695607df [0x1cf3b03a] 129 🔄 Beacon Proxy (Call)
      │  ┞╾─ 0x9352c3b99ccec38116a9a393a01a1ac5e9a1a533 🔗 Implementation 
      │  ├── 0x4b793496f1ce172f5b46b032b801fd07695607df [0xf3855dc2] 130 🔄 Beacon Proxy (Call)
      │  │  ┞╾─ 0x9352c3b99ccec38116a9a393a01a1ac5e9a1a533 🔗 Implementation 
      │  │  └── 0xf5266c485947a3ac4f6bdecd984e96997aab4e65 [0x996612e7] 143 🔄 Beacon Proxy (Call)
      │  │    ╰╾─ 0x9e2534786c1bed03719abe64658a0842bcbc866c 🔗 Implementation 
      │  ├── 0x4b793496f1ce172f5b46b032b801fd07695607df [0x2f878811] 137 🔄 Beacon Proxy (RoleAdded)
      │  │    ╰╾─ 0x9352c3b99ccec38116a9a393a01a1ac5e9a1a533 🔗 Implementation 
      │  └── ...
      ├── 0xb266bd388eb45be9c3fab21c68bf0713dc9d7e84 [0x1cf3b03a] 131 🔄 Beacon Proxy (Call)
      │  ┞╾─ 0x8e2b843a221c155cf390ed854c9d75639c70a989 🔗 Implementation 
      │  └── ...
      └── 0xe7db6d870747d152f8442e7cc7598d480935c7db [0x842fb24a] 144 (Call)
```

## Command-Line Options

| Option | Description |
|--------|-------------|
| `--builtin-modules` | Gets the list of modules embedded in the program |
| `-j` | JSON-RPC endpoint URL |
| `-c` | RPC method or built-in command to execute |
| `-p` | Parameters as a JSON array |
| `-v` | Verbose output |

