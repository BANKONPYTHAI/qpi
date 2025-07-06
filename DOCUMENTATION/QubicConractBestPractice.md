Best Practices for Qubic Smart Contracts
Smart Contract Initialization

Always initialize all state variables explicitly
Managing Smart Contract Balances

Fetch SC balance dynamically using qpi.getEntity()
Never assume a fixed amount—always track incoming and outgoing funds
Handling Transactions Correctly

Ensure transactions are properly validated before modifying state
Using State Variables Correctly

Store all necessary data in state—Qubic SCs do not retain local variables between calls
Security Best Practices

Use require() conditions before modifying state
Validate input values
Use proper balance fetching method


examples in go
https://github.com/qubic/qubic-hackathon/tree/main/qubic-integration-examples

QubicLiveTree
https://qubic.github.io/integration/Partners/qubic-rpc-doc.html?urls.primaryName=Qubic%20RPC%20Live%20Tree


############### rpc ####################3

Available RPC Services
The Qubic RPC API offers a variety of endpoints for different use cases, as detailed in the official documentation


curl https://rpc.qubic.org/v1/status
You can build applications that interact with Qubic using these RPC endpoints, without the need to run your own node.and Swagger archive. Key endpoints include:

General Network Information

GET /v1/status: Retrieves the latest RPC server statistics, including:
timestamp: Current timestamp (e.g., "1724325433")
circulatingSupply: Total QUs in circulation (e.g., "109929085175710")
activeAddresses: Number of active addresses (e.g., 477228)
price: Current QU price in USD (e.g., 0.000001743)
marketCap: Market capitalization in USD (e.g., "191606393")
GET /v1/tick: Returns the current tick (block height) of the network.
Transaction Handling

POST /v1/broadcast-transaction: Broadcasts a transaction to the network.
GET /v1/tick-transactions/{tick}: Retrieves a list of approved transactions for a given tick.
GET /v1/transaction/{txId}: Fetches details of a specific transaction.
GET /v1/transaction-status/{txId}: Checks the status of a specific transaction.
Account and Balance Queries

GET /v1/balances/{addressId}: Retrieves the balance for a specified address ID.
GET /v1/transfers/{identity}/{fromTick}/{toTick}: Lists transfer transactions for an identity within a tick range.
Advanced Blockchain Data

GET /v1/tick-info/{tick}: Provides tick information, including timestamp, epoch, and included transaction IDs.
GET /v1/chain-hash/{tick}: Returns the chain hash for a specific tick.
GET /v1/quorum-tick/{tick}: Retrieves quorum tick data for a specific tick.
GET /v1/store-hash/{tick}: Gets the store hash for a specific tick.
Using RPC Endpoints
These endpoints can be accessed via HTTP requests (e.g., using curl, a TypeScript library, or any HTTP client). Example using curl:



For detailed specifications and response formats, refer to the Qubic RPC Swagger Documentation.

Smart Contract Interaction Limitations
Important: The RPC endpoints have varying levels of support for smart contract interactions:

Pre-deployed HM25 Template: The example frontend application demonstrates how to interact with the pre-deployed HM25 template contract using RPC endpoints like /v1/querySmartContract for reading data and /v1/broadcast-transaction for executing procedures.

Your Custom Contracts: When you modify and deploy your own version of the smart contract, initially you may need to use the Qubic CLI to test your custom functions. After verifying your contract is working correctly, you can integrate it with the frontend following the patterns shown in the example application.

RPC vs CLI Usage: For most teams, the example frontend application provides all necessary components to interact with your smart contract through RPC. The CLI is most useful for debugging and initial testing.
