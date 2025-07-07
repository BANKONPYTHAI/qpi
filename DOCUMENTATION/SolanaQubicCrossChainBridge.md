Solana–Qubic Cross-Chain Bridge: Architecture, Implementation, and Security Analysis
Introduction
A cross-chain bridge between Solana and Qubic would enable seamless transfer of tokens and arbitrary messages between these two distinct blockchains. Solana is a high-performance Proof-of-Stake chain with fast block times and a Rust-based BPF smart contract platform, while Qubic is a novel Layer-1 that integrates smart contract execution into its consensus (quorum of 451 Computors) for sub-second finality
qubic.org
. Bridging them requires careful design using existing interoperability protocols, ensuring finality assumptions on each chain are respected. This report presents a technically detailed architecture for a Solana–Qubic bridge using Wormhole and LayerZero cross-chain messaging frameworks, with example code snippets and diagrams. We also evaluate the suitability, extensibility, and security of these technologies, and analyze past bridge exploits (especially the Wormhole 2022 hack) to inform a robust security design.
Cross-Chain Communication Technologies Overview
Cross-chain bridges rely on passing verifiable messages between chains. In general, there are two broad approaches to validate cross-chain events
medium.com
:
External validators/oracles: A set of off-chain entities observes events on the source chain and produces a proof or signed attestation that can be verified on the destination chain
medium.com
. This category includes multi-sig guardian networks (e.g. Wormhole)
medium.com
, dedicated PoS validator networks (e.g. Axelar, Celer’s SGN)
medium.com
, or oracle+relayer schemes (e.g. LayerZero). These solutions trade some decentralization for efficiency by not running full light clients on-chain.
Light clients & native verification: The destination chain directly verifies source chain proofs (transaction inclusion proofs in source chain blocks) without external trust. This often requires running a light node on-chain, which can be resource-intensive. Some protocols (like LayerZero) mimic light clients by outsourcing the heavy proof verification to off-chain oracles and relayers
medium.com
layerzero.network
. Fully trustless light client bridges between Solana and Qubic would be complex, so we focus on the first approach.
We consider two leading cross-chain messaging technologies for Solana–Qubic: Wormhole and LayerZero.
Wormhole Guardian Network
Wormhole is a generic message-passing bridge that connects multiple heterogeneous chains (including non-EVM chains like Solana, Aptos, Sui, etc.)
blog.li.fi
. It uses a decentralized Guardian network of 19 validators operating a Tendermint-based consensus off-chain to attest to events
markdamasco.medium.com
markdamasco.medium.com
. When a contract on a source chain emits a cross-chain message via the Wormhole core contract, the guardians observe the event and sign a Verifiable Action Approval (VAA) – essentially a standardized signed message containing the event data
wormhole.com
markdamasco.medium.com
. Once a supermajority (currently 13 of 19) guardians have signed, the VAA is considered valid
markdamasco.medium.com
markdamasco.medium.com
. Relayers (which can be anyone, including users) then transmit the VAA to the target chain, where a Wormhole contract verifies the signatures against the stored guardian public keys and, if valid, executes the message payload
wormhole.com
wormhole.com
. The process is summarized below: Wormhole cross-chain message flow: A user transaction on the source chain emits an event by calling the Wormhole Core Contract, which logs a message
wormhole.com
. The off-chain Guardian Network observes the log and signs a VAA attesting to the event. A relayer then submits the VAA to the destination chain’s core contract, which verifies the guardians’ multisignature (13/19 signatures required)
markdamasco.medium.com
markdamasco.medium.com
. Upon validation, the target chain’s contract processes the message (e.g. minting tokens or calling a function)
wormhole.com
. Wormhole’s design is chain-agnostic and extensible: adding support for Qubic would involve writing a Qubic smart contract (or integration at the protocol level) to serve as the Wormhole core contract on Qubic, and updating guardian nodes to run Qubic full nodes. Wormhole already connects many chains via the same guardian set
docs.moonbeam.network
, enabling arbitrary message passing (not just token transfers) across networks
docs.moonbeam.network
. This makes it suitable for complex cross-chain interactions. However, its security hinges on the guardians’ honesty and secure key management (it’s effectively a 19-of-19 multisig by independent validators)
markdamasco.medium.com
markdamasco.medium.com
. Wormhole is partially centralized in that it relies on a fixed set of validators; this has raised concerns that compromising a majority of guardians (or their keys) could break the system
medium.com
. The Wormhole 2022 hack on Solana, discussed later, was not a guardian key compromise but a smart contract bug – highlighting that implementation security is as critical as the trust model.
LayerZero Oracle–Relayer Model
LayerZero is an “omnichain” interoperability protocol aiming to connect every contract on every chain via ultra-light nodes
research.nansen.ai
. Unlike Wormhole’s single guardian network, LayerZero uses a pair of independent off-chain entities: an Oracle and a Relayer
layerzero.network
. The Oracle (often a Chainlink oracle network by default) delivers the source chain block header to the destination chain, while the Relayer provides a proof that a specific transaction/event exists in that block
layerzero.network
medium.com
. The LayerZero on-chain contract (UltraLightNode, part of the Endpoint on each chain) checks that the transaction proof matches the oracle-delivered block header. If both pieces corroborate, the message is considered valid and delivered
layerzero.network
medium.com
. This design ensures that only if two distinct parties (oracle and relayer) agree on the transaction’s validity will the target chain act on the message, reducing the chance of fraud
layerzero.network
. LayerZero’s Endpoint contracts must be deployed on each chain. Each user application (UA) contract interacts with the Endpoint to send or receive messages
medium.com
medium.com
. For example, on Solana (which LayerZero integrated in 2024
support.bitrue.com
), an application would call into the LayerZero endpoint with a target chain ID and message payload; the Endpoint emits a Packet event off-chain
medium.com
. Off-chain, after a configurable number of confirmations, the oracle and relayer for that UA deliver the header and proof to the target chain’s Endpoint (updateHash by Oracle, validateTransactionProof by Relayer)
medium.com
medium.com
. The Endpoint then hands off the payload to the destination UA contract (or stores it for retrieval) once validation passes
medium.com
medium.com
. LayerZero’s model is modular and extensible – new chains can join by deploying the endpoint and having oracles support them (Chainlink or another oracle service must be able to read Qubic blocks)
layerzero.network
. It already supports 70+ networks including Solana and many L1/L2s
support.bitrue.com
. For Qubic, we would need to implement a LayerZero Endpoint in Qubic’s environment (likely a Qubic smart contract or built-in module) and integrate an oracle that monitors Qubic (possibly Qubic’s own oracles or a Chainlink integration) to provide block data. The security model relies on at least one of the two off-chain parties being honest (non-colluding). If the Oracle and Relayer were to collude, they could theoretically forge a non-existent transaction by presenting a fake header and a matching fake proof – so trust is not fully eliminated, just distributed
medium.com
. Currently, LayerZero’s default is a single relayer (run by LayerZero Labs) and a small set of oracle nodes
medium.com
, which introduces centralization concerns. The team has announced plans to decentralize this by using 11 independent validators for oracle/relayer duties, requiring 7-of-11 signatures to approve a message
medium.com
. This moves LayerZero towards a security model more like Wormhole’s (a distributed multisig), trading some simplicity for censorship-resistance and safety. In summary, LayerZero is highly extensible and developer-friendly, but its effective security depends on the chosen oracle/relayer configuration for each application
medium.com
.
Bridge Architecture for Solana ↔ Qubic
Designing a functional Solana–Qubic bridge involves on-chain components on both networks and off-chain infrastructure that connects them. We present an architecture that can be implemented either via a Wormhole-style guardian network or via LayerZero’s oracle-relayer system. In both cases, the core principles are similar: lock or burn assets on the source chain, generate a verifiable message, and mint or unlock assets on the destination chain. Below, we detail the on-chain modules for Solana and Qubic, the message flow, and how validators/oracles facilitate communication. We assume bridging of a fungible token as a concrete example, but the design can carry arbitrary data payloads as well.
Solana On-Chain Module (Bridge Program)
On Solana, the bridge logic would be implemented as a Rust-based smart contract (program) deployed to the Solana runtime. Solana’s existing Wormhole integration already provides a Core Bridge Program (for generic messaging) and a Token Bridge Program (for token lock/mint) on Solana’s mainnet, which we can leverage. For a custom integration or using LayerZero, a new program may be written. Key responsibilities of the Solana-side contract:
Locking tokens and emitting messages: When a user wants to send assets from Solana to Qubic, they will call an instruction on the Solana bridge program (possibly via an associated interface or directly). This instruction will transfer the specified tokens (e.g., an SPL token or SOL) into the custody of the bridge (often held in a PDA account or a vault) and then emit a cross-chain message containing the transfer details. For Wormhole, this is done by calling the Wormhole core contract’s post_message instruction with the payload and a desired consistency level (which determines how many Solana blocks to wait before guardians sign)
npmjs.com
wormhole.com
. The payload typically includes information like: origin chain ID, destination chain ID, token ID or address, amount, and recipient on the destination chain. The Wormhole core program will log an event that guardians observe
wormhole.com
. For LayerZero, the Solana program would instead call the LayerZero Endpoint’s send function (via CPI – cross-program invocation), providing the payload and target chain ID
medium.com
. In either case, Solana’s fast block times (400ms slots) mean the event is available to off-chain watchers almost immediately; Wormhole guardians usually treat one Solana confirmation (~1 block, ~0.5s) as sufficient finality given Solana’s design
wormhole.com
.
Releasing tokens upon verification: When assets or messages come into Solana from Qubic, the Solana program must verify the cross-chain proof and then release tokens to the user. With Wormhole, the user (or an automated relayer) will submit the VAA from Qubic’s transfer to Solana’s Token Bridge contract. The Solana program calls the Wormhole core library to verify the VAA’s guardian signatures against the stored guardian public keys (ensuring the guardian set and quorum are valid)
wormhole.com
. If verified, the program mints the equivalent wrapped asset to the user’s address (for a token transfer), or executes an application-specific action if it’s a generic message. With LayerZero, the inbound flow is handled by the LayerZero Endpoint on Solana: the Qubic side Oracle and Relayer update the ULN contract on Solana with Qubic’s block header and proof, and the Endpoint then invokes a callback on the user’s receiving contract (or allows it to fetch the message)
medium.com
. The UA contract would then call the bridge program’s function to mint or unlock tokens to the user.
Below is a simplified Rust pseudocode sketch for the Solana bridge program using a Wormhole-like approach (for illustration):
rust
Copy
Edit
// Pseudocode for Solana Bridge Program (simplified)
use wormhole_sdk::{post_message, verify_vaa};  // Hypothetical SDK imports

// Instruction: Send tokens from Solana to Qubic
fn send_to_qubic(user: Pubkey, amount: u64, token_mint: Pubkey, qubic_recipient: [u8;32]) {
    // 1. Transfer tokens from user to bridge vault (escrow lock)
    token::transfer(user_token_account, bridge_vault_account, user, amount)?;
    // 2. Create payload: structure with chain IDs, recipient, amount, etc.
    let payload = BridgePayload {
        action: "Send",
        token: token_mint,
        amount: amount,
        recipient: qubic_recipient,        // Qubic-side recipient identifier
        chain_id: QUBIC_CHAIN_ID
    };
    // 3. Emit Wormhole message (consistency 1 = instant) 
    post_message(&payload, ConsistencyLevel::Confirmed)?;
    // Guardians will observe this and sign a VAA off-chain
}

// Instruction: Complete transfer from Qubic to Solana
fn receive_from_qubic(caller: Pubkey, vaa_bytes: &[u8]) {
    // 1. Verify VAA (checks guardian signatures)
    let vaa = verify_vaa(vaa_bytes, EXPECTED_GUARDIAN_SET)?;
    assert!(vaa.emitter_chain == QUBIC_CHAIN_ID);
    // 2. Parse payload from VAA
    let payload: BridgePayload = vaa.decode_payload()?;
    assert!(payload.action == "Send" && payload.chain_id == SOLANA_CHAIN_ID);
    // 3. Mint or unlock tokens to the recipient on Solana
    let token_mint = payload.token;
    let recipient = Pubkey::new(&payload.recipient);
    token::mint_to(token_mint, recipient, bridge_authority, payload.amount)?;
}
In this pseudocode, post_message would internally call the Wormhole core contract to log the message for guardians
npmjs.com
, and verify_vaa would call into Wormhole’s verification routine (which in Solana’s Wormhole uses Secp256k1 signature verification on the guardian signatures). The actual Solana Wormhole contracts are more complex (managing guardian sets, sequence numbers, etc.), but this illustrates the flow. The Solana program must handle message deduplication and replay protection (e.g., ensuring a VAA is only processed once by tracking its sequence number) and enforce any necessary fees for the operation. For a LayerZero implementation, the Solana code would be slightly different – using the LayerZero Endpoint CPI calls rather than a post_message. For example, a Solana UA program might call layerzero::send(target_chain_id, payload, relayer_fee, oracle_fee) which triggers the Packet event
medium.com
. Receiving would rely on the ULN contract to validate the proof before calling the UA’s handler.
Qubic On-Chain Module (Smart Contract or Built-In)
Qubic’s smart contract model differs significantly from Solana’s. Qubic does not use a traditional virtual machine or gas metering; instead, contracts (written in a low-level language via the Qubic Programming Interface, QPI) are incorporated directly into consensus “ticks” and executed deterministically by all 451 Computor nodes
qubic.org
qubic.org
. There are no user-paid fees and execution is part of the block finalization process, yielding sub-second finality for contract calls
qubic.org
. However, deploying a Qubic smart contract is a governed process – contracts must be approved by quorum and launched via an IPO model
medium.com
 (meaning the code is reviewed and agreed upon by the network before activation). For the bridge, we assume a Qubic smart contract (SC) is written and adopted by the network to handle bridging logic. The Qubic bridge contract would handle two main scenarios analogous to Solana’s: locking/burning Qubic-side assets and emitting a message, and minting/releasing assets when a valid message from Solana is received.
Lock Qubic assets & emit message: To send QUBIC (or a Qubic token) to Solana, a user would invoke the Qubic bridge SC (by sending a transaction to the contract’s address
medium.com
 with appropriate input data). The contract would deduct the specified amount from the user’s balance or lock it in its state. Then it needs to emit a cross-chain message for Solana. Qubic’s SCs don’t have an explicit event log mechanism like Ethereum; instead, the contract might write the event data into a reserved portion of state or output that off-chain watchers (guardians/oracles) can monitor. One approach is that the Qubic contract could call a QPI function to compute a message hash (e.g., using K12() which might be a hashing function) and store it alongside the payload in its state
medium.com
. Off-chain Guardians would read the contract state or the transaction result to retrieve this payload. Because Qubic finalizes ticks near-instantly, the watchers can obtain the message right away – we might not even need to wait beyond the current tick. The guardians/oracle then generate the cross-chain proof (VAA or block proof) as described later.
Verify message & release Qubic assets: When tokens are coming from Solana to Qubic, the user (or an automated relayer service) will provide the proof of the Solana event to the Qubic contract. In a Wormhole scenario, this is a VAA signed by the guardians. The Qubic contract must verify the VAA’s signatures against the known guardian public keys. Implementing ECDSA/secp256k1 signature verification inside a Qubic SC is feasible but non-trivial – since QPI does not allow importing external libraries, the contract code must include the verification logic or use built-in primitives. Qubic’s 1 GB memory allowance for contracts
medium.com
 means it can handle computationally heavy tasks, and the Useful Proof-of-Work consensus implies Computors have significant CPU resources for tasks like cryptographic verification. The contract could include an implementation of the secp256k1 verification algorithm (perhaps using a predefined elliptic curve library code embedded in the source). QPI provides a function signatureValidity()
medium.com
, but that likely pertains to verifying the transaction’s signature (by the invoker) rather than arbitrary data signatures – so the contract must do custom verification for guardian signatures. The contract would loop through the guardian public keys, verify each signature on the VAA hash, and count a threshold of valid signatures (e.g., >=13 of 19). Only if the threshold is met and the message payload has not been processed before (prevent replay) will it proceed to unlock or mint the asset to the Qubic user. Qubic’s state model allows each SC to maintain data; for example, the bridge SC can store a map of sequence numbers or message IDs it has executed, and the balances of any wrapped tokens it manages for Solana assets.
Below is a conceptual Qubic SC pseudocode (in a C++-like pseudocode) handling an incoming Wormhole VAA from Solana:
cpp
Copy
Edit
// Pseudocode for Qubic Bridge Smart Contract (conceptual)
#include "QPI.h"  // hypothetical include for Qubic types and functions

// Assume guardian_pubkeys is a predefined array of 19 secp256k1 public keys
const PubKey guardian_pubkeys[19] = { /* ... */ };

bit verify_guardian_signatures(byte[] vaa, byte[][] sigs) {
    byte vaa_hash[32] = sha256(vaa);  // Hash the VAA body for signing
    int valid_count = 0;
    for (int i = 0; i < 19; ++i) {
        if (secp256k1_verify(guardian_pubkeys[i], sigs[i], vaa_hash)) {
            valid_count++;
        }
    }
    return (valid_count >= 13);
}

void on_receive_vaa(byte[] vaa_bytes) {
    // 1. Parse VAA (assume format: header with guardian set and signatures, payload)
    VAA vaa = parseVAA(vaa_bytes);
    assert(vaa.guardian_set == EXPECTED_SET);           // correct guardian set
    assert(!processed_before(vaa.hash));                // prevent replay
    // 2. Verify signatures
    bit ok = verify_guardian_signatures(vaa.body, vaa.signatures);
    assert(ok == true);
    // 3. Process payload (assuming a token transfer message)
    BridgePayload payload = decodePayload(vaa.body);
    if (payload.action == "Send" && payload.chain_id == QUBIC_CHAIN_ID) {
        // Mint or unlock tokens for the recipient on Qubic
        balances[payload.recipient_id] += payload.amount;
    }
    mark_processed(vaa.hash);
}
In this pseudocode, secp256k1_verify would be a function (to be implemented as part of the SC code) that checks a signature using the curve’s algorithm. The contract ensures the VAA hasn’t been seen before and that the payload is destined for Qubic. If the VAA is valid, the contract updates its balances mapping to credit the user. Qubic’s contract state could treat balances similarly to an internal ledger for a wrapped asset (representing, say, SOL or an SPL token from Solana). Because Qubic contracts run as part of consensus, all 451 Computors would execute this code in lockstep – if any signature check or assertion fails, the transaction would be invalid and dropped. The deterministic execution and consensus agreement on the result means that once a transaction is finalized in a tick, it’s irreversibly part of Qubic’s state (instant finality). For the Qubic-to-Solana direction, a similar function would handle a user’s request to send tokens out: subtract from balances, prepare an outbound payload, possibly compute a hash or signature that guardians can use. The contract might not directly “emit” a VAA (since VAA is an off-chain construct), but it could, for example, output the payload and a contract-generated signature by Qubic’s consensus (if Qubic had a notion of an oracle or arbitrator signature
medium.com
). An intriguing possibility is leveraging Qubic’s quorum itself as a signing entity: Qubic’s consensus could sign the message (via an arbitrator or combined Computor signature) to prove finality. However, absent native support for that, it’s simplest to rely on the Wormhole guardians: they will see the Qubic contract’s state change or an RCF (Remote Call Function) message
medium.com
 and then produce the VAA externally. Finality assumptions: On Solana, finality is probabilistic but strong – Wormhole guardians currently wait roughly 1 block (~0.5s) for consistency level 1 on Solana
wormhole.com
, or a few blocks if a higher safety margin is desired (Solana’s recent improvements yield fast confirmations with low rollback risk). On Qubic, each tick is finalized by the quorum immediately
qubic.org
; there is no concept of chain re-orgs given the quorum-based Useful Proof-of-Work consensus. Therefore, the off-chain agents (guardians or oracles) can treat a Qubic transaction as final as soon as it’s included in a tick (sub-second). This near-instant finality on both sides means the Solana–Qubic bridge could operate with very low latency – effectively the bottleneck is network communication and signature aggregation. Care must be taken that Solana’s bridge contract does not act on a Qubic message until it’s sure the Qubic tick was final (which is essentially immediately, barring a catastrophic consensus failure in Qubic). Off-chain relayer/validator roles: If we use Wormhole, the same Guardian network would be extended to support Qubic. Guardians would run Qubic full nodes or light clients to monitor the Qubic bridge contract. When a user sends tokens from Qubic, the guardians see the Qubic SC’s state update (or a special transaction output) and then collectively sign a VAA containing the details. The emitter address in the VAA would correspond to the Qubic bridge contract (each chain’s contracts have a unique Wormhole emitter identifier). Guardians would enforce a finality lag if needed (perhaps 0-1 ticks for Qubic) before signing. Conversely, when a Solana-to-Qubic transfer happens, guardians sign a VAA on Solana’s side which the user passes into Qubic SC. In that direction, the off-chain role is just observing Solana (which they already do). LayerZero would require an Oracle service watching Qubic: for instance, Chainlink nodes could be configured to follow Qubic and provide block headers to Solana’s ULN contract, while a specialized Relayer service provides the proof of the Qubic transaction. On the Qubic side, since Qubic does not natively run EVM contracts, the LayerZero endpoint logic might have to be integrated differently – possibly as a precompiled contract in Qubic’s protocol or a heavily customized Qubic SC that can verify foreign block proofs. This is more complex than the Wormhole approach, which treats Qubic like any other chain via an off-chain guardian network. Thus, Wormhole’s approach may be more straightforward for integrating a non-EVM chain like Qubic
blog.li.fi
docs.moonbeam.network
, as Wormhole is already designed for Solana and other unique chains.
End-to-End Message Flow
Bringing it together, here is the step-by-step flow for a token transfer from Solana to Qubic, and back, highlighting validator/oracle involvement, messaging formats, and finality at each step:
User Initiation on Source Chain: The user calls the bridge interface on the source chain. For example, to send 100 QUBIC to Solana, the user calls send_to_solana(100, solana_recipient) on the Qubic bridge contract. The contract burns or locks 100 QUBIC and prepares an outgoing message containing (amount=100, token=QUBIC, recipient=solana_recipient). In Qubic’s case, this message might be stored in state or output for guardians. Finality on Qubic occurs in the same tick, so this state change is final in <1s
qubic.org
.
Off-Chain Message Observation: Off-chain validators pick up the event. Wormhole guardians monitoring Qubic will detect the contract invocation and extract the payload. They may wait a tiny buffer (perhaps 1–2 seconds or ticks) to ensure the Computors’ consensus is stable, then sign a VAA. The VAA includes: emitter chain ID (Qubic’s unique ID), emitter address (Qubic contract ID), a sequence number, and the payload, all covered by the multisig of guardians
wormhole.com
markdamasco.medium.com
. If using LayerZero, instead the Chainlink oracle waits for the Qubic block, then delivers the Qubic block header to Solana’s ULN contract, while the Relayer generates a Merkle proof of the contract call transaction in that block.
Transfer of Message to Target Chain: The VAA or proof is relayed to the target chain (this can be done by the user’s wallet, a dApp frontend, or an automated relayer service). In Wormhole’s case, the user might call an instruction CompleteTransfer on Solana’s Token Bridge program, providing the VAA. This triggers on-chain verification of the VAA. In LayerZero’s case, the Relayer would call Solana’s Endpoint (validateTransactionProof) and the Oracle would have already called updateHash; together these calls deliver the message into Solana’s LayerZero Endpoint contract
medium.com
medium.com
.
Verification on Destination Chain: The destination chain’s bridge logic now verifies the message’s authenticity:
Wormhole: The Solana Token Bridge program uses the Wormhole core library to check the guardian signatures in the VAA. This involves verifying each ECDSA signature against the known guardian set public keys (stored in the Wormhole core contract). Only if the signatures form a valid quorum (≥13/19) and the message has not been seen before, it proceeds
markdamasco.medium.com
docs.moonbeam.network
. Wormhole’s Solana verification uses a special sysvar instruction to verify secp256k1 sigs; notably, a bug in this process led to the 2022 exploit, underlining the need for correct implementation
halborn.com
halborn.com
.
LayerZero: The Solana ULN contract will verify that the block header (from Oracle) indeed corresponds to a block containing the transaction with the expected payload (proven by the Relayer’s Merkle proof)
medium.com
. If the hash and proof match, the message is considered valid
medium.com
. This effectively means Solana has accepted that “Qubic transaction X of block Y locked 100 QUBIC to send to you.” The finality assumption here is that Qubic’s block Y is final – which is reasonable given Qubic’s consensus finality.
Message Execution on Destination: After verification, the bridge contract on the destination executes the payload. In our example, the Solana program would mint 100 wrapped-QUBIC tokens (representing QUBIC on Solana) into the user’s Solana wallet. If it were an arbitrary message (not just a token transfer), the destination contract would invoke a function call or handle the data accordingly. For a Qubic-to-Solana transfer, this concludes the process. The user now has an equivalent asset on Solana. The bridging system would typically record the transfer (sequence number processed) to avoid replays.
Reverse direction (Solana to Qubic): The user can also transfer from Solana back to Qubic by locking the wrapped tokens on Solana and emitting a message that causes the Qubic contract to unlock the original QUBIC. The flow is symmetric: user calls Solana’s send_to_qubic, Solana program locks tokens and emits Wormhole message (payload: “unlock 100 to recipient X”), guardians sign VAA, user (or relayer) presents VAA to Qubic contract via on_receive_vaa, Qubic contract verifies signatures and then adds 100 QUBIC to the user’s balance
medium.com
. The user regains QUBIC on the Qubic chain, while the wrapped tokens on Solana are burned or kept locked in the vault. Throughout, each chain’s finality is respected (Solana finality ~ instant to a few seconds; Qubic instant) to ensure messages are only acted upon when irreversible.
Messaging formats: Wormhole’s VAA is a binary blob with a header (version, guardian set, signatures, timestamp) and the payload. The payload for token transfers follows a specific schema (including origin/destination chain IDs, token info, amounts)
docs.moonbeam.network
, whereas generic message payloads are arbitrary byte sequences up to some size. LayerZero’s message format is essentially the user-defined payload, but the proof consists of a block header (with block hash) and a Merkle proof (transaction inclusion proof in that block) delivered to the ULN contract
medium.com
. Both approaches include chain identifiers so contracts know how to interpret the message (e.g., Wormhole uses consistent chain IDs across all networks
wormhole.com
). In summary, the architecture uses on-chain smart contracts on Solana and Qubic to custody assets and validate messages, and off-chain validator infrastructure (either Wormhole Guardians or LayerZero oracles/relayers) to carry robust proofs of events across chain boundaries. Next, we analyze security considerations and how this design addresses known risks.
Security Considerations and Exploit Mitigations
Bridges are high-value targets and have suffered some of the largest crypto hacks in history. A secure Solana–Qubic bridge must incorporate lessons from past exploits to mitigate vulnerabilities. Here we examine notable bridge exploits (with Wormhole 2022 as a primary case study) and then outline how our design counters similar risks through careful architecture and implementation.
Lessons from Past Bridge Exploits
Wormhole Solana–Ethereum Hack (Feb 2022): In this incident, an attacker stole 120,000 WETH (worth $326 million) from the Solana side of the Wormhole bridge
halborn.com
. The vulnerability was in the Wormhole Solana smart contract’s signature verification logic. To accept a cross-chain message, the contract needed to verify a Secp256k1 signature from guardians (since guardians use Ethereum-style keys). Wormhole’s developers used a Solana syscall load_instruction_at to ensure that the secp256k1 signature verification instruction was called earlier in the transaction
halborn.com
. However, this function was deprecated and insecure – it did not verify that the instruction came from the genuine system secp256k1 program. The attacker cleverly crafted a fake sysvar account that mimicked the signature verification call, allowing a forged VAA to pass as valid
halborn.com
. Essentially, the contract was tricked into believing a valid guardian signature set existed when it did not. The attacker then invoked Wormhole’s complete_wrapped function to mint 120k WETH on Solana without locking any ETH on Ethereum
halborn.com
, and then cashed it out. Wormhole’s team (Jump Crypto) had to step in to refill the lost ETH and patched the contract. Root cause: insecure use of a deprecated verification method and inadequate checks on sysvar addresses
halborn.com
halborn.com
. Lesson: Never trust unvalidated external inputs within contract logic (like sysvars), use up-to-date secure primitives for cryptography, and have thorough audits. In our Solana program, we would avoid load_instruction_at and use the newer SysvarInstruction checks or direct secp256k1 verification functions provided by Solana’s runtime, ensuring the instructions come from the system program. The fix for Wormhole was to properly verify the Instructions sysvar account is the legitimate one and to use Solana’s native secp256k1 verify syscall with correct parameters.
Ronin Bridge Hack (March 2022): Attackers compromised private keys controlling 5 of 9 multisig validators of the Axie Infinity (Ronin) bridge, enabling them to sign fake withdrawals of 173k ETH and 25M USDC ($600M) without any actual deposits
medium.com
. This was a pure key management failure / insufficient decentralization – the bridge trusted a small set of validator nodes, and the attacker gained control of a majority. Lesson: Use a sufficiently decentralized and secure validator set. Our bridge leverages either Wormhole’s 19 guardians or LayerZero’s oracle/relayer; in both cases, no single key compromise should suffice. Guardians are run by reputable institutions and the threshold is 13, making it much harder to compromise a majority than in Ronin’s 5-of-9 scenario. That said, 19 is not extremely large; expanding the guardian set or using rotating committees could improve security. Additionally, guardians should use HSMs or multisigs for their keys to reduce risk of individual key theft.
Nomad Bridge Exploit (August 2022): Nomad (an optimistic rollup-based bridge) was hacked for ~$190M due to an initialization flaw where a critical storage value (the trusted root of a Merkle tree) was set to zero by default, allowing anyone to bypass proof verification
medium.com
. Attackers simply replayed a valid message call with different parameters, and the contract erroneously accepted it as valid since it read the zero value as a valid root. This hack was dubbed “decentralized robbery” as many copycat attackers joined once the vulnerability became public
ledger.com
. Lesson: Don’t skip proper verification even in initialization; use immutable or properly set trusted roots and require proofs. In our design, we ensure that every cross-chain message is verified either by multisig (Wormhole VAA) or by actual block proofs (LayerZero). There is no concept of a default trust that can be exploited; the guardians’ signatures or the proof must check out for each message. We will also carefully initialize config values like guardian public keys or Chain IDs in the contracts to correct values.
Multichain (Anyswap) Incident (2023): Multichain, a bridge protocol, had several issues – one being that MPC node operators (essentially a set of servers controlling the bridge) were run in a semi-centralized way, and when the CEO disappeared, parts of the network ceased to function, later leading to a hack of ~$120M likely due to a compromised server
medium.com
. Lesson: Operational security and decentralization are important — avoid reliance on one person or entity for critical operations. For our Solana–Qubic bridge, the Wormhole guardian set is operated by multiple independent organizations, and LayerZero’s oracle/relayer can be chosen from independent providers. No single individual should have the power to pause the bridge or access funds; upgrades should be governed (Wormhole uses governance to update guardian sets and contracts). Qubic’s contract approval mechanism (quorum approval for new SCs) adds an extra layer of scrutiny — the bridge code would be vetted by core contributors and the community before deployment, making a sudden malicious upgrade unlikely.
In total, bridge exploits teach us to secure both the off-chain and on-chain components: off-chain validators need strong key security and decentralization, on-chain contracts need rigorous verification logic and fail-safes. We also learn to implement monitoring and emergency response: e.g., Wormhole had Jump Crypto ready to step in with funds; ideally, our design should include a pause mechanism or rate limit in case of anomalies (like extremely large or frequent transfers that could indicate an exploit). For instance, the bridge contracts could have an emergency halt callable by a supermajority of guardians or by Qubic’s governance if something goes wrong, to stop further fund movement until a fix is applied.
Security Features in the Solana–Qubic Bridge Design
Building on those lessons, here’s how our proposed architecture addresses security and mitigates risks:
Robust Signature Verification: We will use well-audited cryptographic libraries and syscalls for verifying guardian signatures. On Solana, the Wormhole program now correctly verifies the secp256k1 signatures by checking the sysvar and using Solana’s native crypto API (the Wormhole bug fix)
halborn.com
. Our Solana code will follow the patched approach – ensuring the Secp256k1Program was indeed invoked and the signatures are verified against the exact VAA data. On Qubic, the contract’s custom verification code will be carefully tested. We might implement multiple checks: for example, verifying the ED25519 signature of the transaction (via signatureValidity()
medium.com
) to ensure the call is from a legitimate user, and separately verifying guardian ECDSA signatures on messages. We could even cross-verify using Qubic’s Arbitrator or Computor IDs
medium.com
 if they provide a source of randomness or identity, though primarily it’s about the guardians. Given Qubic’s computational power, we could include redundant verification (e.g., verify the aggregate BLS signature if guardians ever support that, or verify both the existence of 13 distinct signatures and maybe a quorum certificate if provided).
Decentralized Validators & Oracles: Our design does not rely on any single validator. Wormhole’s 19 guardians provide decentralization; compromising them would require a coordinated attack on many independent nodes (often institutional validators). We would encourage Qubic’s community or foundation to possibly join the guardian set if feasible, to have insight into Qubic events. With LayerZero, to avoid the risk of a single relayer, we can configure multiple relayers or use the upcoming multi-validator model
medium.com
. For instance, the Qubic bridge UA could require 2 out of 3 relayers/oracles to agree (though LayerZero’s base design is 1-of-1 for each, we can simulate redundancy at the application level by requiring multiple independent message deliveries and cross-verifying). If using Chainlink as oracle, we trust their decentralized network of nodes to provide correct block headers. As an extra precaution, one could program the Qubic contract to only accept VAAs that have a certain guardian set index (matching a known set) to avoid an attacker injecting a VAA from an outdated or fake guardian set.
Finality and Double-Spend Protection: By waiting for finality on each chain (which is quick here), we prevent double-spending across chains. Solana’s optimistic confirmation means in rare cases a fork could revert a transaction after a few seconds, which is why Wormhole guardians might use a short delay (e.g., consistency level 1 or 2 corresponds to waiting ~1-2 slots)
wormhole.com
wormhole.com
. This is built-in to the protocol. For Qubic, since finality is immediate by quorum, double-spend on Qubic is not a concern unless the consensus itself fails. The bridge can always enforce that one deposit on source only yields one mint on destination and vice versa, by marking processed sequences. If a chain were to roll back (unlikely for Qubic given its design, and Solana rarely does beyond one or two slots), guardians/oracles would notice a mismatch (the block hash would change) and either not sign or the proof would fail on target.
Replay and Reentrancy Safety: Each message has a unique sequence or nonce (Wormhole VAAs carry a sequence per emitter) so the contracts will ignore duplicates
docs.moonbeam.network
. Qubic SC can maintain a list of processed VAA hashes (using nextId() or similar QPI functions to iterate stored IDs
medium.com
) to ensure it never processes the same message twice. We will also design the contracts to be simple state machines (lock -> emit, verify -> unlock) to minimize reentrancy or complex state issues. Solana programs run atomically in one transaction, so reentrancy is not an issue there, and Qubic SC execution is atomic per tick as well.
Audits and Formal Verification: Both Solana’s and Qubic’s bridging code should undergo thorough security audits by independent firms (as Wormhole was audited, though the bug was subtle). For Qubic, this is especially important since its SCs run at consensus level – a bug could affect the whole network. Formal verification of critical routines (like signature verification logic) could be pursued: e.g., using a formal spec to prove that a fake sysvar attack is impossible in the Solana program, or that the Qubic SC’s verify_guardian_signatures function correctly identifies only valid signatures. Given Qubic’s C++-like environment, static analysis and formal tools might be applied to the QPI code.
Operational Security: Guardians should use secure enclaves or multi-party computation for key signing to reduce risk of key theft (Jump Crypto and others have indicated strong op-sec practices for Wormhole guardians). For the Qubic integration, if any new guardians are added, they must meet the same security standards. If LayerZero is used, the oracle (Chainlink) already has a robust reputation system and nodes run in secure environments. The custom relayer component could be run by multiple parties (including maybe the Qubic core team, Solana community members, etc.) to avoid reliance on one entity.
Monitoring and Incident Response: The bridge should be monitored in real-time. Any anomaly, such as a sudden large mint on one side without a corresponding lock on the other, should trigger alerts. In our design, if a Wormhole-like bug occurred on Qubic SC (allowing a forged VAA), it would likely require a hard fork or quorum intervention to fix – thus we want to catch issues early. We can incorporate a governance or emergency pause: for instance, the Qubic SC could have an arbitrator() (perhaps Qubic’s special account
medium.com
) that under consensus approval can disable the contract temporarily. Solana’s program could similarly be governed by the Wormhole governance (which can update contracts via on-chain governance votes by guardians). In Wormhole’s actual design, guardians can collectively sign a governance VAA to upgrade contracts or change parameters
markdamasco.medium.com
. We would use that mechanism to quickly patch any vulnerability discovered. Moreover, bug bounty programs can incentivize whitehats to find bugs before exploits – Wormhole offers a $2M bug bounty
markdamasco.medium.com
, which should be advertised for the Qubic integration as well.
Economic Security and Limits: We might implement rate limits or deposit limits initially. For example, to protect against unknown issues, the bridge could cap the daily volume or the maximum single transfer of a new token. This ensures that even if a bug exists, the damage is limited while the system is young. Over time, as confidence grows, these limits can be raised or removed via governance.
In particular, the Wormhole 2022 hack teaches us to double-check cross-program invocation logic. In Solana’s new bridging program code, we will ensure the verify_signatures instruction uses the correct sysvar (Instructions) by checking the account key matches the Sysvar ID, and we’ll avoid any deprecated calls
halborn.com
. On Qubic’s side, because the entire verification is custom, we’ll implement it straightforwardly and test with both valid and invalid signatures extensively.
Deployment and Infrastructure Considerations
Smart Contract Deployment: On Solana, deploying the bridge programs (Wormhole’s core and token bridge, or LayerZero endpoint and UA contracts) is a standard on-chain upgrade process. If using Wormhole’s existing contracts, we just need to register Qubic in the Wormhole contracts (assign it a Chain ID and possibly deploy a “stub” contract on Solana for Qubic assets). If using new contracts, we’ll use the Solana CLI to deploy the BPF binaries. On Qubic, deploying the smart contract involves the IPO process: the contract code (likely written in a C-like QPI format) must be integrated into a Qubic core update and approved by the quorum
medium.com
. This means the Qubic community needs to agree on adding the bridge SC. The contract’s code, being perhaps a few thousand lines including crypto, will be part of the Qubic node software after approval. This is a heavier process than on Solana, but it ensures consensus knows about the contract. Off-Chain Relayer Network: For Wormhole, the existing guardian network can be reused. We would work with Wormhole Foundation to integrate Qubic node support into the guardian software. Guardians run “spy” nodes that listen to chain events
wormhole.com
; implementing a Qubic spy might require parsing Qubic’s node RPC or message feed. We might create a lightweight Qubic block parser that feeds new bridge events into the guardian node. For LayerZero, we need off-chain infrastructure: at minimum one Oracle node (Chainlink likely) and one Relayer process. Chainlink oracles would need a Qubic blockchain adapter – possibly a simple one if Qubic provides an RPC for block headers. The Relayer can be a script or service that monitors Qubic mempool or block stream for Packet events analogously (in LZ’s case, to send from Qubic to Solana, Qubic would have to have an endpoint that emits something akin to a Packet event as well, which complicates LayerZero’s use on Qubic since it’s not EVM; hence we lean towards Wormhole). Performance and Scalability: Both Solana and Qubic are high-throughput chains. Wormhole claims to handle large volumes of messages (over a billion messages transferred across all chains)
gate.com
. Qubic’s SC can process up to 1024 transactions per tick and potentially 500 million sub-transfers in one tick for an airdrop scenario
medium.com
. The bridge itself will not be a throughput bottleneck. The guardians/oracles infrastructure would need to handle the message rate, but cross-chain transfers are typically far fewer than on-chain TPS. We should ensure the bridging SC on Qubic is optimized (static memory, no unbounded loops except maybe looping 19 signatures which is fine). The 1 GB memory limit on Qubic SC means we could even precompute certain tables if needed for speed. Extensibility: Once a Solana–Qubic bridge is live, it sets the stage for omnichain applications using Solana’s DeFi liquidity and Qubic’s AI-focused computing power in tandem. The same framework could extend to other Qubic bridges (e.g., Qubic to Ethereum via Wormhole would become possible by routing through the guardians). The design supports not just token transfers but also general message calls – for instance, a contract on Solana could invoke an AI computation contract on Qubic by sending a cross-chain message with input data, and Qubic could return the result as another message. By evaluating Wormhole vs LayerZero: Wormhole provides a single unified pipeline for all apps (shared guardian set, shared contracts), whereas LayerZero gives each application fine-grained control (each app can pick its oracle/relayer and confirmation settings
medium.com
medium.com
). For a public bridge, a Wormhole-like shared infrastructure might be simpler for users (one guardian set to trust). LayerZero could be used by specific projects that want custom trust assumptions. Both approaches can coexist. In conclusion, a Solana–Qubic bridge is technically feasible using today’s cross-chain protocols. By reusing proven components (like Wormhole’s core contracts and guardian network, or LayerZero’s endpoint contracts and Chainlink oracles) and by being mindful of past security pitfalls, we can build a bridge that is efficient, extensible, and secure. The differing architecture of Qubic (no VM, quorum execution) introduces some challenges in integration, but also advantages in speed and determinism. Through careful implementation (with example code as provided) and rigorous security measures (audits, testing, monitoring), the bridge can safely connect Solana’s vibrant ecosystem with Qubic’s emerging high-performance network – unlocking new possibilities for asset transfer and cross-chain smart contract interactions in a multi-chain future
medium.com
markdamasco.medium.com
. Sources: The design and analysis above reference Wormhole and LayerZero official documentation, security post-mortems of bridge hacks, and Qubic’s technical materials to ensure accuracy and currency. Key references include Wormhole’s architecture overview
wormhole.com
wormhole.com
, the Halborn report on the Wormhole hack
halborn.com
halborn.com
, LayerZero’s whitepaper
layerzero.network
 and a detailed Medium analysis
medium.com
medium.com
, as well as Qubic’s own documentation on smart contracts
qubic.org
qubic.org
 and the QPI interface
medium.com
medium.com
. These provide the foundation for the architecture and security recommendations presented.
