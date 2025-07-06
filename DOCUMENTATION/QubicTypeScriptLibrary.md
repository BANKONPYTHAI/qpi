TypeScript Library
Qubic provides an official TypeScript library to interact with the Qubic network programmatically:

Installation
yarn add @qubic-lib/qubic-ts-library
or

npm install @qubic-lib/qubic-ts-library
Basic Usage
// Import helper
import { QubicHelper } from 'qubic-ts-library/dist/qubicHelper'

// Create an ID Package with private/public key and human readable address
const id = await helper.createIdPackage("your-seed-phrase");

// Connect to a node
import { QubicConnector } from 'qubic-ts-library/dist/qubicConnector'
const connector = new QubicConnector("https://rpc.qubic.org");

// Get balance
const balance = await connector.getBalance("IDENTITY_HERE");
Check the examples directory for more usage scenarios, including:

Creating transactions
Fetching balances
Managing multiple users
Handling deposits/withdrawals
Frontend Application
Frontend Overview
We've built a frontend application that demonstrates how to interact with the Qubic blockchain and specifically with the HM25 template smart contract deployed on the testnet. This application serves as an example of how to build user interfaces for Qubic-based dApps.

Source Code: https://github.com/icyblob/hm25-frontend - Clone this repository to run the app locally, detailed setup instructions are provided in the repository README.

Frontend Features
Connect to a Qubic node through its RPC endpoint
Integrated wallet support:
MetaMask Snap integration
WalletConnect support
Seed phrase login
Vault file authentication
Interact with the HM25 template smart contract
Call Echo function (returns funds to the sender)
Call Burn function (burns funds permanently)
View contract statistics
Connecting to a Node
By default, the app connects to a testnet node at http://91.210.226.146.

Important Notes:

The frontend app connects to a shared testnet node by default
Each team will be assigned their own dedicated node (accessible via SSH) where they'll deploy their custom contract
After running the deploy.sh script on your assigned node, you'll need to connect the frontend to your node
The RPC endpoint format has changed: Previously it was http://<HOST_IP>:8000, but now it's simply http://<HOST_IP> without the port
You can connect to your dedicated node:

Open the app in your browser
Click on the ConnectLink at the top right
Select "Connect to Server"
Enter your node URL (e.g., http://your-node-ip) without any port
Refresh the page
How Transactions Work: The wallet is only used for signing transactions. After a transaction is signed, it will be broadcast to the network that the dapp is connecting to, as determined by the HTTP endpoint in the configuration.

The app interacts with the HM25 template smart contract that provides Echo, Burn, and GetStats functionality.

Smart Contract Interaction Examples
The frontend interacts with the HM25 smart contract through several key functions defined in src/components/api/HM25Api.jsx. Here's how it works:

1. Contract Constants
First, we define constants for interacting with the contract:

export const HM25_CONTRACT_INDEX = 12  // The index of the HM25 contract on the testnet

// Function and procedure types
export const PROC_ECHO = 1     // Echo procedure type ID
export const PROC_BURN = 2     // Burn procedure type ID
export const FUNC_GET_STATS = 1  // GetStats function type ID
2. Reading Contract State (GetStats)
To read data from the smart contract (like statistics), we use the /v1/querySmartContract endpoint:

export async function fetchHM25Stats(httpEndpoint) {
    // Create the query data for the GetStats function (type 1)
    const queryData = makeJsonData(HM25_CONTRACT_INDEX, FUNC_GET_STATS, 0, '')
    
    try {
        // Make HTTP POST request to the node's RPC endpoint
        const response = await fetch(`${httpEndpoint}/v1/querySmartContract`, {
            method: 'POST',
            headers: HEADERS,
            body: JSON.stringify(queryData),
        })
        
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`)
        }
        
        const json = await response.json()
        // Decode the response data from base64
        const raw = base64.decode(json.responseData)
        const buf = Buffer.from(raw, 'binary')
        
        // Parse the 16-byte response (2 uint64 values)
        return {
            numberOfEchoCalls: buf.readBigUInt64LE(0),  // First 8 bytes
            numberOfBurnCalls: buf.readBigUInt64LE(8),  // Second 8 bytes
        }
    } catch (error) {
        console.error('Error fetching HM25 stats:', error)
        return {
            numberOfEchoCalls: 0n,
            numberOfBurnCalls: 0n,
        }
    }
}
3. Creating Transaction for Echo Procedure
To call the Echo procedure (which returns funds to the sender), we build a transaction:

export async function buildEchoTx(qHelper, sourcePublicKey, tick, amount) {
    const finalTick = tick + TICK_OFFSET  // Add offset to current tick
    const INPUT_SIZE = 0  // No input data for Echo
    const TX_SIZE = qHelper.TRANSACTION_SIZE + INPUT_SIZE
    const tx = new Uint8Array(TX_SIZE).fill(0)
    const dv = new DataView(tx.buffer)

    // Build the transaction
    let offset = 0
    // Source address
    tx.set(sourcePublicKey, offset)
    offset += qHelper.PUBLIC_KEY_LENGTH
    // Target contract (HM25)
    tx[offset] = HM25_CONTRACT_INDEX
    offset += qHelper.PUBLIC_KEY_LENGTH
    // Amount to send
    dv.setBigInt64(offset, BigInt(amount), true)
    offset += 8
    // Tick for the transaction
    dv.setUint32(offset, finalTick, true)
    offset += 4
    // Procedure type (Echo = 1)
    dv.setUint16(offset, PROC_ECHO, true)
    offset += 2
    // Input size
    dv.setUint16(offset, INPUT_SIZE, true)

    return tx
}
4. Creating Transaction for Burn Procedure
Similarly, to call the Burn procedure (which permanently burns funds):

export async function buildBurnTx(qHelper, sourcePublicKey, tick, amount) {
    const finalTick = tick + TICK_OFFSET
    const INPUT_SIZE = 0
    const TX_SIZE = qHelper.TRANSACTION_SIZE + INPUT_SIZE
    const tx = new Uint8Array(TX_SIZE).fill(0)
    const dv = new DataView(tx.buffer)

    // Build the transaction (similar structure to Echo but different procedure type)
    let offset = 0
    tx.set(sourcePublicKey, offset)
    offset += qHelper.PUBLIC_KEY_LENGTH
    tx[offset] = HM25_CONTRACT_INDEX
    offset += qHelper.PUBLIC_KEY_LENGTH
    dv.setBigInt64(offset, BigInt(amount), true)
    offset += 8
    dv.setUint32(offset, finalTick, true)
    offset += 4
    // Use PROC_BURN (2) instead of PROC_ECHO
    dv.setUint16(offset, PROC_BURN, true)
    offset += 2
    dv.setUint16(offset, INPUT_SIZE, true)

    return tx
}
5. Complete Flow for Executing a Transaction
When a user interacts with the HM25 contract through the UI:

The frontend fetches the current tick from the node
It builds a transaction using buildEchoTx or buildBurnTx
The transaction is signed by the wallet
The signed transaction is broadcast to the node via the /v1/broadcast-transaction endpoint
The app shows the user the transaction status
This approach separates:

The wallet (responsible only for signing transactions)
The node connection (determined by the HTTP endpoint setting)
The business logic (in the smart contract)
The frontend simply orchestrates these components to provide a seamless user experience.

Testing Smart Contracts
There are two primary ways to test your smart contracts on Qubic: using the CLI or using RPC endpoints. Each method has its own advantages and use cases.

CLI Testing
The Qubic CLI provides a direct way to interact with your deployed smart contracts. This is often the easiest method for initial testing and debugging. For complete CLI command reference, see the Qubic CLI Commands Reference section above.

Advantages of CLI Testing
Direct communication with the node
Easier for debugging initial contract functionality
No need to handle encoding/decoding manually
Comprehensive error messages
RPC Testing
RPC (Remote Procedure Call) testing involves making HTTP requests to your node's RPC endpoint. This method is more suitable for frontend applications and automated testing.

Key RPC Endpoints for Smart Contract Testing
POST /v1/querySmartContract - For reading contract state (functions)
POST /v1/broadcast-transaction - For executing contract procedures
GET /v1/tick-info - For getting current tick information
Base64 Encoding/Decoding
When using RPC endpoints, you need to handle Base64 encoding and decoding:

For Input Data: Encode your input parameters in Base64 before sending to the contract For Output Data: Decode the Base64 response from the contract to extract readable data

Example of decoding response data:

const response = await fetch(`${httpEndpoint}/v1/querySmartContract`, {
    method: 'POST',
    headers: HEADERS,
    body: JSON.stringify(queryData),
})

const json = await response.json()
// Decode the response data from base64
const raw = base64.decode(json.responseData)
const buf = Buffer.from(raw, 'binary')

// Parse the decoded data according to your contract's output structure
const result = {
    field1: buf.readBigUInt64LE(0),  // First 8 bytes
    field2: buf.readBigUInt64LE(8),  // Second 8 bytes
}
Advantages of RPC Testing
Better for frontend integration
Allows for automated testing scripts
Can be used from any programming language
More suitable for production applications
Event Decoding
Qubic provides an endpoint to decode events that occur during smart contract execution. This is useful for understanding what happened during transaction processing.

Event Decoding Endpoint
POST https://api.qubic.org/v1/events/decodeEvent
Example Usage
{  
  "eventType": 0,
  "eventData": "502YpKmHdvcsWG8wdVSXWdMm7aX3DmaRnqb7fwg+2jHt81Dpwpkpl7PJ6+W0KQxRwUGu55o1nGrHBs59ZdzuPOwTAAAAAAAAp1lmAQEAMwAweDA5MDM3OGE5YzgwYzVFMUNlZDg1ZTU2QjIxMjhjMWU1MTRFNzUzNTfsEwAAAAAAAAFEoI/b27Ev7KpF7C8/x+VXJvprP801z9VmZ/C4jCDTybVi2ldtkRJBIZfQbpKq0hWl/ynKZGkfwJsDf1y9XhIA"
}
Response
{
    "decodedEvent": {
        "quTransferEvent": {
            "sourceId": "NBMITJYAKMHWEHQWUULCUCRBXQPCFNFLNAHRRONUFEEHDNZDMQZUSQLBGBIN",
            "destId": "PXABYVDPJRRDKELEYSHZWJCBEFJCNERNKKUWXHANCDPQEFGDIUGUGAUBBCYK",
            "amount": "5100"
        }
    }
}
This endpoint is particularly useful for:

Debugging transaction failures
Understanding money flows in your smart contract
Tracking specific events for analytics
Verifying that your contract is behaving as expected
Project Structure
When you fork the Qubic core repository, you'll be working with this structure:

/
├── README.md                  # General information about Qubic core
├── src/                       # Source code directory
│   ├── contracts/             # Smart contract directory 
│   │   └── HM25.h             # Template smart contract file you'll modify
│   ├── contract_core/         # Core contract functionality
│   └── ...                    # Other Qubic core components
The key file you'll be modifying is src/contracts/HM25.h, which contains the template smart contract for the hackathon. Remember that after making changes to this file, you must run the cleanup script and then redeploy to see your changes take effect.

On the testnet node that we provide, you'll find the deployment scripts in this location:

/root/qubic/qubic-docker/      # Docker scripts directory on the testnet node
├── deploy.sh                  # Script to deploy your SC to a node
└── cleanup.sh                 # Script to clean up your deployment
These are the scripts you'll execute after connecting to the testnet node via SSH. Remember that after making changes to your smart contract, you must run the cleanup script and then redeploy to see your changes take effect.

FAQs
Q: Do I need to set up my own node?
A: No. We provide a test node that you can access via SSH. You'll be modifying the HM25.h template file on this node and deploying it using the provided scripts.

Q: How do I develop my smart contract?
A: You should fork the Qubic core repository, switch to the madrid-2025 branch, and modify the HM25.h template file with your smart contract code. You can also modify files in the contract_core and contracts folders if needed, but this is not recommended for beginners.

Q: How do I test my smart contract?
A: After developing your smart contract, contact us on the #dev channel on Discord. We'll provide you with SSH access to a test node where you can run the deployment script. After successful deployment, you can use the Qubic CLI to interact with your contract.

Q: Can I update my smart contract after it's deployed?
A: Yes. Simply make your changes to the HM25.h file in your forked repository, and then run the deployment script again on the test node.

Q: How do I know if my smart contract has issues?
A: When you run the deployment script, it will compile your code. If there are any issues, the deployment will fail, and the logs will provide information about what went wrong.

Q: Can I build a frontend app that interacts with my smart contract?
A: Yes! After deploying your smart contract, you'll have access to an RPC endpoint that you can use to build a frontend application.

Q: Do I have to use C++ for my smart contract?
A: Yes. At this time, Qubic smart contracts must be written in C++.

Q: Will I get seeds/faucet for testing?
A: Yes! The testnet node is pre-configured with multiple pre-funded seeds, each containing 1 billion Qubic tokens. See the Available Testnet Seeds section for the complete list of seeds you can use.

Q: How do I verify my smart contract is working?
A: After deployment, use the Qubic CLI to send commands to your contract. For example:

./qubic-cli -nodeip YOUR_NODE_IP -nodeport YOUR_NODE_PORT -seed YOUR_SEED -somecommand
Q: What is the IPO process for smart contracts in Qubic?
A: On the mainnet, each smart contract needs to be IPOed and approved by computors. This is a security mechanism to ensure the quality and integrity of smart contracts. For this hackathon, we've simplified the process by providing a pre-approved template contract (HM25.h) that you can modify. You can learn more about the IPO process at https://docs.qubic.org/learn/smart-contracts.
