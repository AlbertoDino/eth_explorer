# ETH-Explorer

A simple CLI utility for making RPC calls to Ethereum networks, featuring powerful transaction tracing with smart contract identification.

## Highlights

- **Full JSON-RPC 2.0 support** — Call any standard Ethereum RPC method
- **Transaction tracing** — Visualize the complete execution flow of a transaction
- **Smart contract detection** — Automatically identifies proxy patterns (Beacon, UUPS, etc.)
- **Event decoding** — Recognizes well-known function signatures and event names
- **Hierarchical output** — See nested calls and delegate calls in an intuitive tree format

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

## Transaction Tracing (`eth_trace`)

The standout feature of ETH-Explorer. Given any transaction hash, `eth_trace` reconstructs the full execution flow and displays:

- **All internal calls and events** in chronological order
- **Smart contract types** — detects Beacon Proxies, UUPS Proxies, and other patterns
- **Implementation addresses** for proxy contracts
- **Decoded signatures** for well-known functions and events (e.g., `RoleAdded(bytes32,address,address)`)
- **Gas consumption** at the top-level call

```bash
./eth-explorer -j https://ethereum-sepolia-rpc.publicnode.com \
    -c eth_trace \
    -p '["0x6c1b120f327b35e0e6592cfd767e2f43703f4838da3f0811a2c11e97d7867f30"]' -v
```

### Example Output

```
from: 
 └── 0x12e60f3971a3cb26b1a65388d0b0ab2e4eadf5a1 
to: 
   └── 0xe7db6d870747d152f8442e7cc7598d480935c7db | 🔋Gas: 852062 | BlockNumber 0x93540f 
      ├── 0x4b793496f1ce172f5b46b032b801fd07695607df [0x1cf3b03a] 129 🔄 Beacon Proxy (Call, args: 1)
      │  ┞╾─ 0x9352c3b99ccec38116a9a393a01a1ac5e9a1a533 🛠️  Implementation 
      │  └── 0x4b793496f1ce172f5b46b032b801fd07695607df [0xf3855dc2] 130 🔄 Beacon Proxy (Call, args: 3)
      ├── 0xb266bd388eb45be9c3fab21c68bf0713dc9d7e84 [0x1cf3b03a] 131 🔄 Beacon Proxy (Call, args: 1)
      │  ┞╾─ 0x8e2b843a221c155cf390ed854c9d75639c70a989 🛠️  Implementation 
      │  └── 0xb266bd388eb45be9c3fab21c68bf0713dc9d7e84 [0x2f878811] 132 🔄 Beacon Proxy (RoleAdded(bytes32,address,address))
      ├── 0xb266bd388eb45be9c3fab21c68bf0713dc9d7e84 [0x2f878811] 133 🔄 Beacon Proxy (RoleAdded(bytes32,address,address))
      ├── 0xb266bd388eb45be9c3fab21c68bf0713dc9d7e84 [0x2f878811] 134 🔄 Beacon Proxy (RoleAdded(bytes32,address,address))
      ├── 0xb266bd388eb45be9c3fab21c68bf0713dc9d7e84 [0xc7f505b2] 135 🔄 Beacon Proxy (Call, args: 0)
      ├── 0x30d0c863da6e11884a304ded10b3550332b8f29a [0x41d21945] 136 🔄 Proxy (Call, args: 0)
      │    ╰╾─ 0x521433f18d35276ecd0681142389f19dfe4c9978 🛠️  Implementation 
      ├── 0x4b793496f1ce172f5b46b032b801fd07695607df [0x2f878811] 137 🔄 Beacon Proxy (RoleAdded(bytes32,address,address))
      ├── 0x4b793496f1ce172f5b46b032b801fd07695607df [0x2f878811] 138 🔄 Beacon Proxy (RoleAdded(bytes32,address,address))
      ├── 0x4b793496f1ce172f5b46b032b801fd07695607df [0x2f878811] 139 🔄 Beacon Proxy (RoleAdded(bytes32,address,address))
      ├── 0x4b793496f1ce172f5b46b032b801fd07695607df [0x2f878811] 140 🔄 Beacon Proxy (RoleAdded(bytes32,address,address))
      ├── 0x4b793496f1ce172f5b46b032b801fd07695607df [0x2f878811] 141 🔄 Beacon Proxy (RoleAdded(bytes32,address,address))
      ├── 0x4b793496f1ce172f5b46b032b801fd07695607df [0xc7f505b2] 142 🔄 Beacon Proxy (Call, args: 0)
      ├── 0xf5266c485947a3ac4f6bdecd984e96997aab4e65 [0x996612e7] 143 🔄 Beacon Proxy (Call, args: 0)
      │    ╰╾─ 0x9e2534786c1bed03719abe64658a0842bcbc866c 🛠️  Implementation 
      └── 0xe7db6d870747d152f8442e7cc7598d480935c7db [0x842fb24a] 144 (Call, args: 1)
```

**Legend:**
| Symbol | Meaning |
|--------|---------|
| 🔄 | Proxy contract detected |
| 🛠️ | Resolved implementation address |
| 🔋 | Gas consumed |
| `[0x...]` | Function selector (4-byte signature) |
| `123` | LogIndex event |

## Command-Line Options

| Option | Description |
|--------|-------------|
| `--builtin-modules` | Shows a list of built-in programs |
| `-j` | JSON-RPC endpoint URL |
| `-c` | RPC method or built-in command to execute |
| `-p` | Parameters as a JSON array |
| `-v` | Verbose output |
