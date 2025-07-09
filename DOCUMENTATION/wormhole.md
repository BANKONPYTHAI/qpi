Wormhole-Based SOL–qSOL Bridge Using Mint-Burn Token Wrapper

Implementing a Solana–Qubic Wormhole Token Bridge (SOL ⇄ qSOL)
Overview: Wormhole Cross-Chain Messaging Architecture


Wormhole architecture for cross-chain communication (source chain on left, guardian network center, target chain on right). Here, a Solana program emits a message that is observed and signed by Guardians, and then verified on the Qubic side to trigger token minting. Wormhole is a decentralized generic message-passing protocol that connects multiple blockchains (including Solana and potentially Qubic) via a network of Guardians
wormhole.com
. On the source chain (Solana), a Wormhole Core Contract logs a message (in Solana’s transaction logs) when a program calls its publishMessage function
wormhole.com
. Off-chain Guardians (a committee of validators) monitor these logs on every supported chain. When a new message appears, each Guardian independently validates it and signs a verifiable action approval (VAA) – essentially a structured message payload plus a batch of guardian signatures
wormhole.com
markdamasco.medium.com
. Once a quorum of signatures (e.g. 13 of 19 Guardians for Wormhole mainnet) is reached, the VAA is finalized and can be relayed to the destination chain
markdamasco.medium.com
. On the target chain (Qubic), a contract receives the VAA (usually via a relayer or user transaction) and invokes the Wormhole core contract’s verify function
wormhole.com
. The core contract verifies the VAA’s signatures against the known Guardian public keys (ensuring the required threshold of signatures is present and valid) and checks that the VAA hasn’t been processed before (replay protection). If the VAA is valid, it allows the target-chain application (in this case, the token bridge contract) to consume the message and perform the intended action
wormhole.com
. In summary, Wormhole’s infrastructure provides a secure mailbox: Solana programs post messages, Guardians sign them, and Qubic contracts validate those signatures to trust the messages. This architecture decouples the actual token transfer logic (minting/burning) from the cross-chain transport, which is handled by the Guardians and core contracts. Wormhole VAA format and security: Every VAA includes fields identifying the source (emitting chain and contract), a sequence number, the payload, and the Guardian signatures
mirror.xyz
mirror.xyz
. For token transfers, Wormhole uses a standardized payload (payload ID 1) that encodes the transfer details: the amount, the origin token address and chain ID, the target recipient and target chain ID, and an optional fee
mirror.xyz
. For example, to bridge SOL from Solana to Qubic, the Solana side will emit a payload indicating something like “Transfer X tokens of [Solana token address] on chain 1 (Solana) to [recipient address] on chain <QubicID>”
mirror.xyz
. The Guardians attest to this message, producing a signed VAA. On Qubic, the VAA payload will be parsed to recover these fields and confirm the intent. Each VAA is uniquely identified by the (emitter_chain, emitter_address, sequence) tuple, and the Wormhole core contract on Qubic will reject any duplicate VAAs to prevent replays (each message can be consumed only once) as part of its verification routine. The security of the system hinges on the Guardians’ signatures – Qubic’s verification must ensure the signatures are authentic and meet the quorum. A critical best practice is to use the latest, audited signature verification code; notably, an early Wormhole exploit occurred on Solana when an attacker exploited an outdated Secp256k1 validation contract to forge a VAA, resulting in a fraudulent mint on the Ethereum side
mirror.xyz
. Thus, the Qubic integration must rigorously validate ECDSA signatures against the current Guardian set and reject any VAA not signed by the required majority of Guardians.
Lock-Mint Bridge Design for SOL ⇄ qSOL
The SOL–Qubic bridge will use a classic lock-and-mint approach built on Wormhole’s messaging layer. On Solana (the source for native SOL), SOL tokens will be locked in a smart contract, and a corresponding amount of qSOL (a wrapped SOL token on Qubic) will be minted for the user on the Qubic chain. Later, when the user sends qSOL back to Solana, the qSOL on Qubic is burned and the original SOL is unlocked on Solana. This design follows Wormhole’s standard token bridging mechanism: “The token is locked on the origin chain and its wrapped version is minted on the target chain”
mirror.xyz
. Below is the high-level flow of a transfer from Solana to Qubic and back:
1. Attestation (one-time setup): Before the first transfer of SOL, the Solana token’s metadata (name, symbol, decimals) should be registered on Qubic via a Wormhole Asset Meta VAA. This ensures Qubic knows the token’s properties (e.g. SOL has 9 decimal places) and can create a corresponding qSOL representation
wormhole.com
. Wormhole’s Token Bridge module normally handles this automatically the first time a new asset is seen. The Qubic token bridge contract will store SOL’s metadata so that qSOL matches SOL’s precision and is distinguishable by name/symbol as the Wormhole-wrapped SOL.
2. Locking on Solana: When a user initiates a transfer of SOL to Qubic, they will actually transfer native SOL into the custody of the Solana Wormhole Token Bridge program (a secure escrow account). Because SOL is the native currency (not an SPL token by default), the bridge program will handle wrapping it into an SPL token (often the WSOL wrapper) before locking. In practice, the front-end or the Solana program can perform a lamport transfer to a temporary WSOL token account for the user, then move those WSOL tokens into the bridge’s custody. The key is that an equivalent amount of SOL becomes held (frozen) by the Solana bridge contract – effectively removed from circulation – to back the qSOL that will be minted on Qubic. This lock is done in the same transaction that emits the Wormhole message to guarantee atomicity (either both the lock and message happen, or neither does). Wormhole doesn’t dictate how the lock is implemented – it could be a smart contract escrow or a dedicated custody address – as long as the tokens are effectively immovable except by a valid release VAA
mirror.xyz
. (The Wormhole Portal Token Bridge on Solana uses a program-owned account as the custody for each token mint.)
3. Emitting the transfer message: After locking the SOL, the Solana Token Bridge program calls the Wormhole core contract’s emitVAA (publishMessage) function to emit a log message containing the transfer payload
mirror.xyz
. The payload includes: the amount of SOL locked, the Solana token address (for SOL this might be a special marker or the WSOL mint address), the origin chain ID (Solana is chain 1 in Wormhole), the target chain ID for Qubic, the recipient’s Qubic address (32 bytes), and any fee for relayers
mirror.xyz
. This message is then picked up by the Guardians from Solana’s log. All Guardians confirm that the SOL was successfully locked on Solana (by observing the event) and then sign the VAA containing the transfer payload
mirror.xyz
. Once a sufficient number of Guardian signatures is collected (e.g. 13 of 19), the VAA becomes available (usually within a few seconds, given Solana’s fast finality and Guardian networking).
4. VAA Relay to Qubic: The signed VAA is delivered to Qubic. This can happen in two ways: (a) Automatic – a relayer service monitors the guardian network and submits the VAA to the Qubic chain on the user’s behalf, or (b) Manual – the user (or Bankon PYTHAI’s backend) fetches the VAA bytes from a Wormhole API and calls the Qubic contract to submit it
wormhole.com
wormhole.com
. In either case, the Qubic Token Bridge contract on Qubic will receive the VAA and invoke Wormhole core verification.
5. VAA Verification on Qubic: Qubic’s Wormhole Core Contract (deployed as part of integrating Wormhole into Qubic) exposes a function to verify VAAs. This function will parse the VAA header to ensure it’s addressed to Qubic (the to_chain field matches Qubic’s chain ID) and check the Guardian signatures. The Qubic core contract keeps an on-chain record of the active Guardian set (public keys or an aggregated key) and a Guardian Set Index. Using this, it verifies each signature in the VAA’s signature array (likely ECDSA secp256k1 verification, as Wormhole Guardians use secp256k1 keys for interoperability
mirror.xyz
). Qubic’s contract must verify that >=2/3 of the Guardians signed off, matching the guardian set index in the VAA
markdamasco.medium.com
. Only if all signatures check out and threshold is met will the core contract consider the VAA valid. It will then record the VAA’s sequence number as consumed (to prevent replay) and output a confirmation (or call back into the token bridge logic). Replay protection is crucial – the core contract might maintain a mapping (emitter_chain, emitter_address, sequence -> bool processed)
wormhole.com
 so that if someone tries to resubmit the same VAA, the verification step will fail the second time. With the VAA verified, the Qubic bridge contract can trust the embedded message.
6. Minting on Qubic: After verification, the Qubic Token Bridge contract mints qSOL tokens to the target recipient’s address on Qubic. The amount minted equals the original SOL amount (minus any fee) locked on Solana
mirror.xyz
. The qSOL token is the Wormhole-wrapped version of SOL on Qubic, implemented according to Qubic’s token standards. Because Qubic is not EVM, qSOL might be implemented as a smart contract ledger mapping addresses to balances (similar to ERC-20 logic but in Qubic’s C++ contract style). The minting function will increase the recipient’s qSOL balance and increment the total supply of qSOL. Qubic’s bridge contract should also store the relationship that this qSOL represents SOL from Solana (chain 1) with a 1:1 backing. From the user’s perspective, they now have qSOL on Qubic that can be used in Qubic’s DeFi or applications (e.g. Bankon PYTHAI’s platform) – effectively bringing liquidity from Solana to Qubic.
7. Burn on Qubic (Qubic → Solana): When a user on Qubic wants to send assets back to Solana, they will call the Qubic Token Bridge contract to redeem their qSOL. This triggers a burn of the qSOL amount (reducing the user’s balance and the total supply) and emits a Wormhole message on Qubic. The contract will construct a similar transfer payload: amount, token address (again referring to SOL’s origin identity), token chain (1 for Solana, since SOL is originally from chain 1), target recipient address on Solana, target chain ID = 1 (Solana), and fee. The Qubic contract then calls the Qubic Wormhole core to publish the message (this is analogous to Solana’s case but now Qubic is the emitter). The Guardians observe Qubic’s chain (the integration will require Guardians to run Qubic full nodes or monitors) and sign a VAA for this burn event. Note that because SOL is the origin asset, the payload still uses token_chain = 1 (Solana) and the original token address (Solana’s SOL representation) – this signals to the Solana side that it’s the native asset being returned
mirror.xyz
.
8. Unlocking on Solana: The signed VAA for the burn (qSOL redemption) is then submitted to Solana’s Token Bridge program. The Solana Wormhole core contract verifies the Guardian signatures (using the same guardian set and secp256k1 verification on Solana’s side) and ensures the VAA hasn’t been seen. It then passes the payload to the token bridge logic. Since the payload indicates the token’s origin is Solana itself, the Solana token bridge knows that it should release native SOL rather than minting a token. The program will retrieve the locked SOL from its custody (the amount that was locked earlier) and transfer it to the intended recipient’s Solana address
reddit.com
. In practice, if SOL was originally wrapped as an SPL (WSOL) and stored, the bridge might unwrap it: e.g. close the WSOL token account to yield SOL back to the user. The end result is the user receives real SOL in their wallet on Solana, completing the round trip. According to Wormhole’s design, the wrapped token (qSOL) only exists on the non-origin chain, and when it’s redeemed, the origin chain releases the original asset
reddit.com
.
Throughout this flow, Wormhole Guardians and VAAs ensure the two chains remain in sync: every lock on Solana corresponds to a mint on Qubic, and every burn on Qubic corresponds to an unlock on Solana
markdamasco.medium.com
. Bankon PYTHAI can leverage this to allow users to seamlessly transfer SOL into Qubic’s ecosystem (perhaps to use in AI-driven DeFi on Qubic) and later withdraw back to Solana, with confidence that the process is secure and 1:1 backed at all times.
Solana-Side Implementation: Locking SOL and Emitting Wormhole Message
On Solana, we will use Wormhole’s existing Portal Token Bridge program (or a custom program following similar principles) to handle SOL locking. Best practices for token locking on Solana include using a program-controlled account to hold the funds and making the lock and message emission in one atomic transaction. The Wormhole token bridge program on Solana already defines a custody account for each token mint. For SOL, since it isn’t an SPL mint by default, the program can use the SPL wrapper. Typically, the Portal bridge expects an SPL token, so the user’s SOL is first converted to WSOL (an SPL token with mint address So111... on mainnet). The bridging program might automatically wrap the SOL: for example, by creating an associated token account for the user and invoking Solana’s system program to transfer the desired lamports into it (thus minting WSOL). Then it transfers the WSOL from the user to the bridge’s custody. This way, the SOL is effectively held by the token program in an account the bridge controls. It’s important to close any temporary WSOL account if fully drained, to recover rent – but in most cases the bridge will transfer the exact amount, leaving no remainder. After locking, the program assembles the Wormhole transfer payload. Wormhole on Solana has a dedicated instruction (e.g. TransferWrapped or TransferNative) in the token bridge program which under the hood will: burn or lock the tokens, then call the Wormhole core’s PostMessage instruction to emit the transfer VAA payload. If implementing this from scratch, pseudocode for the Solana program’s handler might look like this:
```rust

/// Pseudocode for initiating a SOL -> Qubic transfer on Solana
fn send_sol_to_qubic(user: Pubkey, amount: u64, target_qubic_addr: [u8;32], wormhole_core: Pubkey) {
    // 1. Wrap SOL into WSOL if not already an SPL token
    let wsol_mint = Pubkey::new("So11111111111111111111111111111111111111112"); 
    let user_wsol_acc = associated_token_account(user, wsol_mint);
    if !user_wsol_acc.exists() {
        create_token_account(user_wsol_acc, user, wsol_mint);
    }

    system_program::transfer(user, user_wsol_acc, amount);  // convert SOL -> WSOL tokens
```
    //  Lock WSOL in the bridge custody
```rust
    let bridge_custody_acc = Pubkey::find_program_address([wsol_mint], bridge_program_id);
    token::transfer(user_wsol_acc, bridge_custody_acc, amount, authority=user); 
```
    //  Build Wormhole transfer payload
```rust
    let payload = TransferPayload {
        payload_id: 1u8,
        amount: amount,
        token_address: wsol_mint.to_bytes(),      // 32-byte token address (WSOL mint)
        token_chain: 1u16,                        // origin chain ID (1 = Solana)
        to: target_qubic_addr,                    // 32-byte recipient on Qubic
        to_chain: QUBIC_CHAIN_ID,                 // Wormhole chain ID for Qubic
        fee: 0u256
    };
```
    //  Post message via Wormhole Core (consistency_level can be 1 for near-instant finality on Solana)
```rust
    let sequence = wormhole::post_message(wormhole_core, payload.serialize(), consistency_level=1);
```
    // (In practice, the token bridge program would call post_message internally and return a sequence)
}
In the above pseudocode, we assume the existence of a wormhole::post_message interface to the Wormhole core contract, which the token bridge program would invoke. The custody account for WSOL is identified by a PDA (program-derived address) using the mint as seed (this is how Portal’s Solana implementation manages separate vaults per token). Only the bridge program can sign for this PDA, so the funds are secure. The program transfers from the user’s WSOL account into custody and then emits the message. If any step fails, the transaction is aborted, so we never emit a message without locking tokens (or vice versa). For Solana, after calling post_message, the program would typically log the sequence number (so the off-chain relayer can know which VAA to fetch) and return. Wormhole’s core contract on Solana will have recorded the message with an emitter address (the bridge program’s address) and a sequence. Guardians will see the event and start the signing process. One Solana-specific consideration is finality: Solana’s Wormhole integration might use consistency_level = 1 (meaning wait for 1 confirmation before Guardians sign) or higher if desired
mirror.xyz
 – this addresses the possibility of forks. In practice, Solana finality is fast, and waiting 1–2 slots is sufficient. Lastly, fee handling: in the payload, fee can be nonzero if using Wormhole’s paid relayer network (so that part of the transferred tokens are designated as payment to a relayer on the target chain). In our scenario, BANKON PYTHAI might choose to set fee = 0 and run its own relayer or require the user to submit the VAA on Qubic. If using automatic relayers, a portion of SOL could be sacrificed as a fee and later distributed to the relayer on Qubic by the token bridge contract.
Qubic-Side Implementation: Verifying VAAs and Managing qSOL
Integrating Wormhole into Qubic involves deploying two main on-chain components: Wormhole Core (Messaging) Contract and a Token Bridge Contract for Qubic. The core contract’s job is to maintain the Guardian set and verify signatures on VAAs, while the token bridge contract will handle the minting and burning of qSOL (and potentially other assets in the future) in response to Wormhole messages. Qubic’s smart contract model is different from Solana’s (Qubic uses C++ for contracts and an IPO deployment mechanism), but the fundamental logic can be implemented similarly. Wormhole Core Contract on Qubic: This contract will store an array of Guardian public keys and a current Guardian set index. It exposes two primary methods – one to verify VAAs, and one (admin-only) to update the Guardian set (triggered when the Guardians rotate, via a governance VAA from Wormhole Foundation). The verify function takes a VAA (as bytes) as input. It will parse the VAA structure (version, guardian_set_index, signatures, timestamp, etc.) and especially the body (which includes emitter_chain, emitter_address, sequence, consistency_level, payload
mirror.xyz
mirror.xyz
). The contract checks that guardian_set_index matches its current set; if not, it rejects the VAA (it might mean an outdated VAA or wrong guardian set). Then it iterates over the signatures array, and for each signature it recovers the signer’s public key (using secp256k1 elliptic curve signature verification on the message hash). In C++ on Qubic, this could be done via a crypto library or syscalls provided for verification. It tallies the number of valid signatures that correspond to known Guardians. If the count is >= the threshold (e.g., 13 of 19) and there are no invalid signatures, the VAA is considered valid
markdamasco.medium.com
. The core contract would also check for duplication: Wormhole VAAs include a sequence number that increments per emitter. The contract can keep a mapping of (emitter_chain, emitter_address, sequence) to a boolean flag. If a VAA with the same triple is presented again, it must reject it to prevent replay attacks. Typically, the first time a VAA is verified, the contract stores consumed[emitter_chain][emitter_address][sequence] = true. This ensures exactly-once processing of each message. After full verification, the core contract can either return a success flag/struct to the caller (if the caller is the token bridge contract expecting the outcome), or the token bridge contract might call an internal function of core (depending on how inter-contract calls are done in Qubic). For simplicity, imagine the token bridge contract calls Core.verifyVAA(signed_vaa) and gets back a VerifiedVAA object containing the parsed payload if all checks pass. Token Bridge Contract on Qubic: This contract implements the mint/burn logic for qSOL and ties into the core contract for message validation. It will have two key entry points: one to redeem incoming transfers (mint qSOL when a VAA from Solana is received) and one to initiate outgoing transfers (burn qSOL and emit a Wormhole message). Pseudocode for the Qubic token bridge might look like:
```cpp
/// Pseudocode for Qubic Token Bridge contract (C++ style)
class QubicTokenBridge {
    // Storage:
    Map<Address, uint64_t> qSOL_balances;
    uint64_t qSOL_total_supply = 0;
    Address wormhole_core_contract;
    // (Guardian set is stored in the core contract.)

    // Handle incoming Wormhole VAA (to mint qSOL)
    public void receiveVAA(byte[] signed_vaa) {
        VerifiedVAA vaa = WormholeCore(wormhole_core_contract).verifyVAA(signed_vaa);
        require(vaa.is_valid, "Invalid VAA or bad signatures");
        // Ensure this VAA is a token transfer for SOL -> Qubic
        TransferPayload payload = parseTransferPayload(vaa.payload);
        require(payload.payload_id == 1, "Not a token transfer message");
        require(payload.to_chain == QUBIC_CHAIN_ID, "VAA not intended for Qubic");
        // Check that the token is SOL from Solana:
        require(payload.token_chain == 1 /*Solana chain ID*/, "Origin chain mismatch");
        PubKey sol_token_addr = PubKey(payload.token_address);
        require(sol_token_addr == SOL_TOKEN_ADDR_ON_SOLANA, "Token address mismatch");
        // Now mint the qSOL:
        Address recipient = Address(payload.to);  // convert 32-byte to Qubic address type
        mintQSol(recipient, payload.amount);
        // No need to emit further message; just record mint event (for off-chain indexing)
    }

    private void mintQSol(Address recipient, uint64_t amount) {
        qSOL_balances[recipient] += amount;
        qSOL_total_supply += amount;
        // (Implement Qubic event or logging for the mint)
    }

    // Handle outgoing transfer (user burns qSOL to send back to Solana)
    public void sendToSolana(uint64_t amount, array<uint8_t,32> sol_recipient) {
        Address sender = Tx.origin;  // the user calling this function
        require(qSOL_balances[sender] >= amount, "Insufficient qSOL");
        // Burn the qSOL
        qSOL_balances[sender] -= amount;
        qSOL_total_supply -= amount;
        // Compose Wormhole transfer payload for burn (Solana as target)
        TransferPayload payload;
        payload.payload_id = 1;
        payload.amount = amount;
        payload.token_address = SOL_TOKEN_ADDR_ON_SOLANA.bytes;  // 32-byte origin address
        payload.token_chain = 1;        // origin is Solana
        payload.to = sol_recipient;     // 32-byte Solana recipient (Pubkey)
        payload.to_chain = 1;           // target chain ID 1 = Solana
        payload.fee = 0;
        // Submit message to Wormhole core (specify desired consistency, e.g., wait 1 block finality)
        WormholeCore(wormhole_core_contract).publishMessage(payload.serialize(), consistency_level=1);
        // The core contract will log the message and Guardians will pick it up.
    }

    // (Additional functions like a getter for balance, perhaps an admin function to adjust core_contract address, etc.)
}
```
In this pseudocode, we assume WormholeCore.verifyVAA throws or returns an error if signatures don’t match or VAA is replayed, so by the time we get a VerifiedVAA object we know it’s authentic. The code then parses the payload to ensure it’s the expected asset. We hardcoded a constant SOL_TOKEN_ADDR_ON_SOLANA which would be the 32-byte representation of SOL’s token address in Wormhole’s format – likely the same address used when emitting (e.g., the WSOL mint or a special constant representing native SOL). By verifying token_chain == 1 and the token address matches SOL, we ensure that we only mint qSOL in response to genuine SOL lock messages (and not some other asset). This is important if the bridge contract is extended to handle multiple tokens – you don’t want to accidentally mint qSOL for a message that wasn’t actually SOL. In a more general implementation, the contract could look up a token ID or address in a registry of attested tokens to find which local asset to mint. In our case, we assume SOL was attested and linked to qSOL. The mintQSol function simply increments the recipient’s balance. Qubic’s smart contracts don’t have a native ERC-20 standard, but this mapping serves the same purpose, and Qubic’s API likely allows querying these balances. We would also implement similar logic for other tokens if needed (possibly using a mapping from origin (chain, address) to a local token ledger). For the burn path (sendToSolana), we verify the user has enough qSOL, then deduct it. We then build a transfer payload targeted to Solana (chain 1). Note: we set token_chain = 1 and use the original SOL address again, meaning we inform Solana “this amount of SOL (originating on chain 1, address X) is returning to you”. We call a publishMessage on the Wormhole core contract with this payload. The core will emit a log event that Guardians on Qubic listen to. They will create a VAA signed by the Guardians, just like on Solana’s side. Eventually that VAA will be submitted to Solana’s bridge which will unlock the SOL as described earlier. One important aspect is ensuring Wormhole standard compatibility: The Qubic contracts should format addresses in the Wormhole 32-byte format and use the correct Wormhole chain ID constants for Solana and Qubic. (If Qubic is integrated, it will be assigned a unique Wormhole chain ID not conflicting with existing ones. For example, Solana=1, Ethereum=2, ... Qubic might be a new ID – the contract must use the agreed ID in all messages.) Also, the Qubic contract should handle the Wormhole consistency level properly. If Qubic’s consensus finality is near-instant, a low consistency level (like 1) is fine; if it had longer finality or was probabilistic, we might choose a higher level (akin to waiting N blocks before Guardians sign). The publishMessage call should include this. Guardian Network integration on Qubic: The off-chain Guardian nodes need to run Qubic full nodes or RPC clients to monitor the Qubic Wormhole core contract. Integration steps include updating the Guardian node software to support Qubic’s chain ID and parsing Qubic transactions. When the Qubic core’s publishMessage is called (e.g., in sendToSolana above), the core will emit a log or state change that Guardians detect, containing the payload and sequence number. They then follow the standard signing process to produce a VAA. The Qubic core contract’s address will serve as the emitter_address in the VAA, and its chain as emitter_chain = <QubicID>. For incoming messages, the Guardians do the reverse: they deliver the signed VAA to Qubic (likely via a transaction calling receiveVAA on the token bridge contract, or a dedicated function on the core contract). We should ensure the Qubic core contract’s code recognizes the Guardians’ signatures exactly as the official Wormhole implementation expects. This means using the same elliptic curve and message format – which is a concatenated hash of the VAA body (there’s a specific hashing scheme defined by Wormhole, typically SHA-256 over a certain structure)
mirror.xyz
. Essentially, Qubic’s verify must replicate Wormhole’s off-chain VAA digest calculation and signature verification to avoid any mismatch. Guardian set updates: Over time, the set of Guardians (and their public keys) can change. Wormhole handles this via a governance VAA (signed by the current Guardians) that instructs the core contracts on all chains to update to a new guardian set. We must implement a method in Qubic’s core contract to accept such a VAA (after verification) and update the stored guardian keys. Typically, the Wormhole core contract has an upgradeGuardianSet(new_keys, new_index) function that can only be invoked by a valid VAA from the old set. BANKON PYTHAI should ensure to deploy the Qubic contracts with the initial guardian set matching Wormhole’s current set and include the logic to rotate keys securely. This keeps Qubic’s integration in lock-step with Wormhole’s standard.
Security and Compatibility Considerations
Security is paramount in a bridge that locks and mints tokens across chains. Here are key considerations and best practices for our SOL–qSOL Wormhole bridge:
VAA Validation & Signature Verification: As emphasized, always use the correct Guardian public keys and require the threshold of signatures. The Qubic verification must guard against any forgery. The 2022 Wormhole exploit on Solana (120k ETH theft) was a result of lax signature verification
mirror.xyz
 – we avoid this by using up-to-date cryptography libraries and perhaps defense-in-depth (e.g., requiring the guardian signatures to be in a specific order or using an aggregated public key check if possible). Also, validate the chain IDs and addresses in the payload: the Qubic contract should ensure to_chain or to fields actually correspond to Qubic when processing incoming VAAs, and similarly Solana’s program ensures a returning VAA is indeed from Qubic and for Solana. Wormhole’s message format includes these fields specifically to prevent a malicious or mistaken relay from delivering a message to the wrong chain
mirror.xyz
.
Replay Protection: We have implemented replay protection on Qubic by tracking consumed sequence numbers. On Solana, the Wormhole core program similarly will not allow the same VAA to be processed twice. This prevents an attacker from re-submitting an old VAA to mint or unlock twice. The mapping of consumed VAAs should be stored persistently. Also consider wormhole message uniqueness: the (emitter_chain, emitter_address, sequence) tuple is globally unique for each message
wormhole.com
. We should also ensure that the Qubic contract cannot be tricked into accepting a different message with the same sequence from the same emitter (which shouldn’t happen unless the Guardians erroneously sign two different payloads with identical sequence, which they would not). In practice, the core contract’s check on sequence number is enough.
Custody of Locked Assets: On Solana, the SOL is held by the token bridge program. We must trust that program (Portal’s implementation is audited and open source). BANKON PYTHAI could use the official Wormhole Portal program for Solana to avoid writing custom locking code, which already has these safeguards. For instance, the Portal contract uses a PDA vault for each token and only releases funds when a valid VAA is provided
mirror.xyz
. No single validator on Solana can arbitrarily withdraw the SOL – only a Wormhole VAA (which requires collusion of >=13 Guardians) can trigger a release. This federated security model is inherently less secure than Solana’s native consensus, but it is the Wormhole trust model one accepts
mirror.xyz
. To mitigate risk, processes like monitoring guardian behavior and using Wormhole’s upcoming permissionless features (like MultiGov governance) can be considered.
Mint Authority on Qubic: The Qubic token bridge contract is the sole minter of qSOL. It should be designed such that only when a valid Solana-origin VAA is verified will it call mintQSol. There should be no other code path that mints qSOL arbitrarily. For example, we wouldn’t want an admin function that could mint qSOL at will – that would break the 1:1 backing. Similarly, the sendToSolana burn function is open to users but it can’t mint; it only calls Wormhole core to emit a message. So qSOL supply can only go up via receiveVAA (with a valid lock on Solana) and down via sendToSolana (burning when returning to Solana). This design mirrors the Portal bridge’s minted token contracts (on EVM chains, Portal uses a mintable ERC-20 where the bridge contract has mint/burn rights and users just hold the token).
Decimals and Metadata: Ensure qSOL’s decimals match SOL’s 9 decimals to avoid any rounding issues. The attestation VAA from Solana will carry the decimal = 9 (and perhaps SOL’s symbol “SOL”). The Qubic contract should store this or hardcode it so that any UI or future contract reading qSOL’s info gets the correct decimals (e.g., if Qubic has an explorer or wallet, it should show qSOL with 9 decimal places so that 1 qSOL = 1.000000000 SOL). This avoids confusion and maintains compatibility with Wormhole standards of preserved metadata
wormhole.com
wormhole.com
.
Guardian Network Integration: BANKON PYTHAI’s bridge will only be as secure as the Guardian network. There are 19 guardians (including top validators from Solana, etc.), and Wormhole is considered an external validator model bridge
mirror.xyz
. This carries some centralization risk – if the Guardians were to collude or be compromised, they could sign a fraudulent VAA. However, Wormhole’s track record (apart from the one exploit which was a smart contract bug) has been good, and it’s trusted by projects like Circle (USDC) and Uniswap for cross-chain operations
wormhole.com
. Still, one should follow Wormhole’s security updates closely. If Qubic eventually supports more trust-minimized bridges, that could be explored, but Wormhole currently provides a good balance of security and usability for this use case.
Performance and Scalability: Qubic boasts very high throughput (15M TPS) and feeless transactions
qubic.org
. The Wormhole bridge, meanwhile, will be limited by the Guardians and the source chain’s throughput (Solana can handle thousands of TPS, and the Guardians can typically sign VAAs in a couple of seconds). The SOL–qSOL transfers should be quite fast: after a user locks SOL, they might see qSOL on Qubic within a few seconds or up to a minute depending on relayer efficiency. Qubic’s contract execution for verification is offloaded to its “computors” (validators), but since Qubic allows complex computations in C++ and even aims for high parallelism, verifying 19 signatures and updating a few states is trivial in terms of performance. Bankon PYTHAI can confidently scale this bridging solution – many users can bridge concurrently; Wormhole’s Guardian network can process and sign many messages in parallel as well (the Wormhole network has processed billions of messages historically)
wormhole.com
. The feeless nature of Qubic means users don’t need QUBIC tokens to receive qSOL, which is a great UX advantage: e.g., a Solana user can move SOL to Qubic and use it immediately without acquiring a new gas token. (On Solana’s side, they of course pay a minimal SOL fee for the transaction, but Solana fees are very low.)
BANKON PYTHAI Integration: With this bridge in place, BANKON PYTHAI can incorporate cross-chain SOL transfers into its platform. For instance, if BANKON PYTHAI is an AI-driven DeFi app on Qubic that wants to tap Solana’s liquidity, users can “Bank on PyThai” by bringing SOL over as qSOL, use it on Qubic for whatever strategies or AI models the platform offers, and withdraw back to Solana seamlessly. The platform should provide a clear UI for deposits and withdrawals that underneath call the described contract functions. It can use Wormhole’s monitoring tools or the Wormhole SDK to track transfer status (e.g., waiting for Guardian signatures, etc.). Additionally, architecture diagrams similar to the ones above should be used in developer docs to illustrate the flow for maintainability and auditing. The code should be written clearly and include unit tests, for example: simulate a lock on Solana and ensure a corresponding mint on Qubic, test that a duplicate VAA fails, test that an invalid Guardian signature set is rejected, etc. All these steps will ensure the bridge operates reliably and securely.
In summary, implementing a mint-burn token bridge for SOL between Solana and Qubic using Wormhole involves: locking SOL on Solana in exchange for minting qSOL on Qubic, utilizing Wormhole’s cross-chain message (VAA) for consensus on the token transfers, and carefully verifying those VAAs on each side with Guardian signatures and replay protection. By following Wormhole’s standard protocol for token bridges
markdamasco.medium.com
 – lock on source, mint on destination, burn on source, unlock on destination – and adhering to the security best practices outlined, BANKON PYTHAI’s SOL–qSOL bridge will enable secure and scalable transfers. The end result is a developer-friendly integration: Solana’s high-speed network and Qubic’s high-TPS, feeless environment connected through Wormhole, allowing SOL to fluidly move into Qubic’s ecosystem and back, unlocking new liquidity and opportunities on both chains. Sources: The design and pseudocode above are based on Wormhole’s official documentation and prior implementations of cross-chain bridges, notably the Wormhole Portal token bridge
mirror.xyz
wormhole.com
, as well as security insights from the Wormhole exploit post-mortem
mirror.xyz
. The architecture aligns with Wormhole’s lock-and-mint model
markdamasco.medium.com
 and ensures compatibility with their Guardian network and VAA format
mirror.xyz
. All cross-chain actions are verified by Wormhole’s decentralized guardians, preserving the integrity of SOL and qSOL transfers across Solana and Qubic.
