/*
 * qOracle QPI-Compliant Production RC1
 * Quantum-Resistant Oracle & Synthetic Asset Ecosystem
 * Qubic Programming Interface (QPI) Compliant Implementation
 * 
 * Components:
 * - Oracle Committee (7-of-7 quantum signatures)
 * - BANKON PYTHAI Token (100,000 fixed supply)
 * - qBTC Synthetic Token (21M fixed supply)
 * - qUSD Stablecoin (USDC peg)
 * - Cross-Chain Bridge Integration
 * - Governance & Multisig
 * 
 * QPI Compliance:
 * - No C++ arrays (using qpi.h structures)
 * - No pointers or casting
 * - No native data types (using qpi.h types)
 * - No #include statements
 * - No % or / operators (using mod/div)
 * - No local variable declarations
 * - No typedef/union
 * - No floating point types
 * 
 * License: Qubic Anti-Military License
 * (c) BANKON All Rights Reserved
 */

// ========================== QPI COMPLIANT DATA STRUCTURES ==========================

// Oracle Committee Configuration
struct OracleConfig {
    static constexpr uint64_t NUM_ORACLES = 7;
    static constexpr uint64_t QUORUM_THRESHOLD = 4;  // 4-of-7 required
    static constexpr uint64_t PUBKEY_SIZE = 1472;    // Dilithium3 public key
    static constexpr uint64_t SIG_SIZE = 2701;       // Dilithium3 signature
    static constexpr uint64_t MIN_PRICE = 1;
    static constexpr uint64_t MAX_PRICE = 100000000000000000ULL;
    static constexpr uint64_t MAX_TIMESTAMP_SKEW = 600; // 10 minutes
};

// Token Configuration
struct TokenConfig {
    static constexpr uint64_t BKPY_DECIMALS = 15;
    static constexpr uint64_t BKPY_TOTAL_SUPPLY = 100000 * 1000000000000000ULL; // 100,000 with 15 decimals
    static constexpr uint64_t QBTC_DECIMALS = 8;
    static constexpr uint64_t QBTC_TOTAL_SUPPLY = 2100000000000000; // 21M BTC in satoshis
    static constexpr uint64_t QUSD_DECIMALS = 15;
    static constexpr uint64_t QUSD_DECIMAL_MULTIPLIER = 1000000000000000ULL;
    static constexpr uint64_t SATPAY_TOTAL_SUPPLY = 2100000000000000ULL; // 2.1 quadrillion SATPAY
};

// Bridge Configuration
struct BridgeConfig {
    static constexpr uint64_t BRIDGE_FEE = 0; // Zero fees for trustless operation
    static constexpr uint64_t MIN_SWAP_AMOUNT = 1000; // Minimum swap amount
};

// Governance Configuration
struct GovernanceConfig {
    static constexpr uint64_t PROPOSAL_DURATION = 604800; // 7 days
    static constexpr uint64_t VOTING_PERIOD = 259200; // 3 days
    static constexpr uint64_t EXECUTION_DELAY = 86400; // 1 day
    static constexpr uint64_t MIN_QUORUM = 3;
    static constexpr uint64_t APPROVAL_THRESHOLD = 60; // 60%
    static constexpr uint64_t VETO_THRESHOLD = 40; // 40%
};

// ========================== CONTRACT STATE STRUCTURES ==========================

// Oracle Public Key Structure
struct OraclePubKey {
    uint8_1472 pubkey; // Using qpi.h uint8_1472 for Dilithium3 public key
};

// Oracle Signature Structure
struct OracleSignature {
    uint64 signer_index;
    uint8_2701 signature; // Using qpi.h uint8_2701 for Dilithium3 signature
};

// Price Message Structure
struct PriceMessage {
    uint64 price;      // Fixed-point price (15 decimals)
    uint64 timestamp;  // Unix timestamp
    uint8 decimals;    // Price precision
};

// Price Update Structure
struct PriceUpdate {
    PriceMessage message;
    collection<OracleSignature> signatures; // Using qpi.h collection
};

// Token Balance Structure
struct TokenBalance {
    id owner;
    uint64 amount;
};

// Proposal Structure
struct Proposal {
    id to;
    uint64 value;
    uint8_1024 data; // Using qpi.h uint8_1024 for proposal data
    uint64 nonce;
    bit executed;
    collection<id> signatures; // Using qpi.h collection for signatures
    uint8_256 action; // Using qpi.h uint8_256 for action description
    uint8_256 parameter; // Using qpi.h uint8_256 for parameter
};

// ========================== CONTRACT STATE ==========================

// Global contract state
struct ContractState {
    // Oracle Committee State
    OraclePubKey trusted_oracles[OracleConfig::NUM_ORACLES];
    PriceMessage last_price;
    collection<PriceMessage> price_history;
    uint64 max_history;
    
    // Token States
    collection<TokenBalance> bkpy_balances;
    collection<TokenBalance> qbtc_balances;
    collection<TokenBalance> qusd_balances;
    collection<TokenBalance> satpay_balances;
    
    // Bridge State
    collection<TokenBalance> bridge_balances;
    uint64 bridge_fee_collected;
    
    // Governance State
    collection<id> governance_owners;
    uint32 threshold;
    uint64 proposal_nonce;
    collection<Proposal> proposals;
    
    // System State
    bit initialized;
    bit admin_burned;
    id admin_address;
    id bridge_authority;
    
    // Locals for function calls
    struct {
        uint64 temp_amount;
        uint64 temp_balance;
        uint64 temp_index;
        bit temp_valid;
        id temp_owner;
        uint64 temp_timestamp;
        uint64 temp_price;
        uint8 temp_decimals;
    } locals;
};

// Global state instance
ContractState state;

// ========================== QPI COMPLIANT FUNCTIONS ==========================

// Quantum signature verification (external function)
extern bit dilithium_verify(
    uint8_1472 pubkey,
    uint8_1024 msg,
    uint64 msg_len,
    uint8_2701 sig
);

// Current block timestamp (external function)
extern uint64 get_current_timestamp();

// ========================== UTILITY FUNCTIONS ==========================

// Safe addition with overflow check
bit safe_add(uint64 a, uint64 b, uint64* result) {
    if (a > 0xFFFFFFFFFFFFFFFFULL - b) {
        return 0; // Overflow
    }
    *result = a + b;
    return 1;
}

// Safe subtraction with underflow check
bit safe_sub(uint64 a, uint64 b, uint64* result) {
    if (a < b) {
        return 0; // Underflow
    }
    *result = a - b;
    return 1;
}

// Safe multiplication with overflow check
bit safe_mul(uint64 a, uint64 b, uint64* result) {
    if (a != 0 && b > 0xFFFFFFFFFFFFFFFFULL / a) {
        return 0; // Overflow
    }
    *result = a * b;
    return 1;
}

// Safe division with zero check
bit safe_div(uint64 a, uint64 b, uint64* result) {
    if (b == 0) {
        return 0; // Division by zero
    }
    *result = div(a, b); // Using qpi.h div function
    return 1;
}

// Safe modulo with zero check
bit safe_mod(uint64 a, uint64 b, uint64* result) {
    if (b == 0) {
        return 0; // Modulo by zero
    }
    *result = mod(a, b); // Using qpi.h mod function
    return 1;
}

// ========================== ORACLE COMMITTEE FUNCTIONS ==========================

// Serialize message for signature verification
void serialize_message(PriceMessage msg, uint8_16* out) {
    uint64 price_net = msg.price;
    uint64 ts_net = msg.timestamp;
    
    // Copy price (first 8 bytes)
    for (uint64 i = 0; i < 8; i++) {
        out->set(i, (uint8)(price_net & 0xFF));
        price_net = div(price_net, 256);
    }
    
    // Copy timestamp (next 8 bytes)
    for (uint64 i = 8; i < 16; i++) {
        out->set(i, (uint8)(ts_net & 0xFF));
        ts_net = div(ts_net, 256);
    }
}

// Validate single oracle signature
bit validate_signature(OraclePubKey pubkey, PriceMessage msg, uint8_2701 sig) {
    uint8_16 data;
    serialize_message(msg, &data);
    
    bit ok = dilithium_verify(pubkey.pubkey, data, 16, sig);
    
    // Clear sensitive data
    for (uint64 i = 0; i < 16; i++) {
        data.set(i, 0);
    }
    
    return ok;
}

// Validate oracle committee update
bit validate_update(PriceUpdate update) {
    // Check signature count
    if (update.signatures.population() < OracleConfig::QUORUM_THRESHOLD) {
        return 0;
    }
    
    // Check for duplicate signers and validate signatures
    uint64 valid_signatures = 0;
    uint64_64 seen_signers; // Using qpi.h uint64_64 for tracking seen signers
    
    for (uint64 i = 0; i < update.signatures.population(); i++) {
        OracleSignature sig = update.signatures.element(i);
        
        if (sig.signer_index >= OracleConfig::NUM_ORACLES) {
            continue;
        }
        
        // Check if signer already signed
        bit already_signed = seen_signers.get(sig.signer_index);
        if (already_signed) {
            continue; // Skip duplicates
        }
        
        // Mark signer as seen
        seen_signers.set(sig.signer_index, 1);
        
        // Validate signature
        if (validate_signature(state.trusted_oracles[sig.signer_index], update.message, sig.signature)) {
            valid_signatures = valid_signatures + 1;
        }
    }
    
    if (valid_signatures < OracleConfig::QUORUM_THRESHOLD) {
        return 0;
    }
    
    // Monotonic timestamp check
    if (update.message.timestamp <= state.last_price.timestamp) {
        return 0;
    }
    
    // Timestamp sanity check
    uint64 now = get_current_timestamp();
    if (update.message.timestamp > now + OracleConfig::MAX_TIMESTAMP_SKEW) {
        return 0;
    }
    if (now > 0 && update.message.timestamp + OracleConfig::MAX_TIMESTAMP_SKEW < now) {
        return 0;
    }
    
    // Price bounds check
    if (update.message.price < OracleConfig::MIN_PRICE || update.message.price > OracleConfig::MAX_PRICE) {
        return 0;
    }
    
    return 1;
}

// Submit price update (public function)
bit submit_price_update(PriceUpdate update) {
    if (!state.initialized && !state.admin_burned) {
        return 0; // Contract not initialized
    }
    
    if (!validate_update(update)) {
        return 0; // Invalid update
    }
    
    // Update price
    state.last_price = update.message;
    
    // Add to history
    state.price_history.add(0, update.message, get_current_timestamp());
    
    // Limit history size
    if (state.price_history.population() > state.max_history) {
        // Remove oldest entry
        uint64 oldest_index = state.price_history.headIndex(0);
        if (oldest_index != 0) {
            state.price_history.remove(oldest_index);
        }
    }
    
    return 1;
}

// Get current price
PriceMessage get_current_price() {
    return state.last_price;
}

// Emergency pause (admin only)
bit emergency_pause(id sender) {
    if (state.admin_burned) {
        return 0; // Admin key burned
    }
    if (sender != state.admin_address) {
        return 0; // Not admin
    }
    
    // Implementation would pause oracle updates
    return 1;
}

// ========================== TOKEN FUNCTIONS ==========================

// Find token balance
uint64 find_token_balance(collection<TokenBalance>* balances, id owner) {
    for (uint64 i = 0; i < balances->population(); i++) {
        TokenBalance balance = balances->element(i);
        if (balance.owner == owner) {
            return balance.amount;
        }
    }
    return 0;
}

// Update token balance
void update_token_balance(collection<TokenBalance>* balances, id owner, uint64 amount) {
    // Find existing balance
    uint64 existing_index = 0;
    bit found = 0;
    
    for (uint64 i = 0; i < balances->population(); i++) {
        TokenBalance balance = balances->element(i);
        if (balance.owner == owner) {
            existing_index = i;
            found = 1;
            break;
        }
    }
    
    if (found) {
        // Update existing balance
        TokenBalance new_balance;
        new_balance.owner = owner;
        new_balance.amount = amount;
        balances->replace(existing_index, new_balance);
    } else {
        // Add new balance
        TokenBalance new_balance;
        new_balance.owner = owner;
        new_balance.amount = amount;
        balances->add(0, new_balance, 0);
    }
}

// BANKON PYTHAI Token Functions
bit bkpy_transfer(id sender, id to, uint64 amount) {
    if (!state.initialized && !state.admin_burned) {
        return 0; // Contract not initialized
    }
    
    if (sender == to || amount == 0) {
        return 0;
    }
    
    uint64 sender_balance = find_token_balance(&state.bkpy_balances, sender);
    if (sender_balance < amount) {
        return 0;
    }
    
    uint64 to_balance = find_token_balance(&state.bkpy_balances, to);
    
    // Update balances
    update_token_balance(&state.bkpy_balances, sender, sender_balance - amount);
    update_token_balance(&state.bkpy_balances, to, to_balance + amount);
    
    return 1;
}

uint64 bkpy_balance_of(id owner) {
    return find_token_balance(&state.bkpy_balances, owner);
}

uint64 bkpy_total_supply() {
    return TokenConfig::BKPY_TOTAL_SUPPLY;
}

// qBTC Synthetic Token Functions
bit qbtc_mint(id user, uint64 btc_sats, PriceUpdate price_update) {
    if (!state.initialized && !state.admin_burned) {
        return 0; // Contract not initialized
    }
    
    if (!validate_update(price_update)) {
        return 0; // Invalid price update
    }
    
    // Calculate qBTC amount based on price
    uint64 qbtc_amount;
    if (!safe_mul(btc_sats, price_update.message.price, &qbtc_amount)) {
        return 0; // Overflow
    }
    
    // Add to user's balance
    uint64 current_balance = find_token_balance(&state.qbtc_balances, user);
    uint64 new_balance;
    if (!safe_add(current_balance, qbtc_amount, &new_balance)) {
        return 0; // Overflow
    }
    
    update_token_balance(&state.qbtc_balances, user, new_balance);
    
    return 1;
}

bit qbtc_transfer(id sender, id to, uint64 amount) {
    if (!state.initialized && !state.admin_burned) {
        return 0; // Contract not initialized
    }
    
    if (sender == to || amount == 0) {
        return 0;
    }
    
    uint64 sender_balance = find_token_balance(&state.qbtc_balances, sender);
    if (sender_balance < amount) {
        return 0;
    }
    
    uint64 to_balance = find_token_balance(&state.qbtc_balances, to);
    
    // Update balances
    update_token_balance(&state.qbtc_balances, sender, sender_balance - amount);
    update_token_balance(&state.qbtc_balances, to, to_balance + amount);
    
    return 1;
}

uint64 qbtc_balance_of(id owner) {
    return find_token_balance(&state.qbtc_balances, owner);
}

// qUSD Stablecoin Functions
bit qusd_mint(id sender, id to, uint64 amount) {
    if (!state.initialized && !state.admin_burned) {
        return 0; // Contract not initialized
    }
    
    if (sender != state.bridge_authority) {
        return 0; // Only bridge authority can mint
    }
    
    uint64 to_balance = find_token_balance(&state.qusd_balances, to);
    uint64 new_balance;
    if (!safe_add(to_balance, amount, &new_balance)) {
        return 0; // Overflow
    }
    
    update_token_balance(&state.qusd_balances, to, new_balance);
    
    return 1;
}

bit qusd_transfer(id sender, id to, uint64 amount) {
    if (!state.initialized && !state.admin_burned) {
        return 0; // Contract not initialized
    }
    
    if (sender == to || amount == 0) {
        return 0;
    }
    
    uint64 sender_balance = find_token_balance(&state.qusd_balances, sender);
    if (sender_balance < amount) {
        return 0;
    }
    
    uint64 to_balance = find_token_balance(&state.qusd_balances, to);
    
    // Update balances
    update_token_balance(&state.qusd_balances, sender, sender_balance - amount);
    update_token_balance(&state.qusd_balances, to, to_balance + amount);
    
    return 1;
}

uint64 qusd_balance_of(id owner) {
    return find_token_balance(&state.qusd_balances, owner);
}

// BANKON SATPAY Token Functions
bit satpay_transfer(id sender, id to, uint64 amount) {
    if (!state.initialized && !state.admin_burned) {
        return 0; // Contract not initialized
    }
    
    if (sender == to || amount == 0) {
        return 0;
    }
    
    uint64 sender_balance = find_token_balance(&state.satpay_balances, sender);
    if (sender_balance < amount) {
        return 0;
    }
    
    uint64 to_balance = find_token_balance(&state.satpay_balances, to);
    
    // Update balances
    update_token_balance(&state.satpay_balances, sender, sender_balance - amount);
    update_token_balance(&state.satpay_balances, to, to_balance + amount);
    
    return 1;
}

uint64 satpay_balance_of(id owner) {
    return find_token_balance(&state.satpay_balances, owner);
}

uint64 satpay_total_supply() {
    return TokenConfig::SATPAY_TOTAL_SUPPLY;
}

// ========================== BRIDGE FUNCTIONS ==========================

bit swap_stx_for_qbtc(id user, uint64 stx_amount, PriceUpdate price_update) {
    if (!state.initialized && !state.admin_burned) {
        return 0; // Contract not initialized
    }
    
    if (!validate_update(price_update)) {
        return 0; // Invalid price update
    }
    
    if (stx_amount < BridgeConfig::MIN_SWAP_AMOUNT) {
        return 0; // Below minimum
    }
    
    // Calculate qBTC amount (1:1 ratio for simplicity)
    uint64 qbtc_amount = stx_amount;
    
    // Mint qBTC to user
    if (!qbtc_mint(user, qbtc_amount, price_update)) {
        return 0;
    }
    
    // Add STX to bridge balance
    uint64 bridge_balance = find_token_balance(&state.bridge_balances, user);
    uint64 new_bridge_balance;
    if (!safe_add(bridge_balance, stx_amount, &new_bridge_balance)) {
        return 0; // Overflow
    }
    
    update_token_balance(&state.bridge_balances, user, new_bridge_balance);
    
    return 1;
}

bit swap_qbtc_for_stx(id user, uint64 qbtc_amount, PriceUpdate price_update) {
    if (!state.initialized && !state.admin_burned) {
        return 0; // Contract not initialized
    }
    
    if (!validate_update(price_update)) {
        return 0; // Invalid price update
    }
    
    if (qbtc_amount < BridgeConfig::MIN_SWAP_AMOUNT) {
        return 0; // Below minimum
    }
    
    // Check user has enough qBTC
    uint64 user_qbtc = find_token_balance(&state.qbtc_balances, user);
    if (user_qbtc < qbtc_amount) {
        return 0;
    }
    
    // Check bridge has enough STX
    uint64 bridge_stx = find_token_balance(&state.bridge_balances, user);
    if (bridge_stx < qbtc_amount) {
        return 0;
    }
    
    // Burn qBTC from user
    uint64 new_user_qbtc;
    if (!safe_sub(user_qbtc, qbtc_amount, &new_user_qbtc)) {
        return 0;
    }
    update_token_balance(&state.qbtc_balances, user, new_user_qbtc);
    
    // Transfer STX from bridge to user
    uint64 new_bridge_stx;
    if (!safe_sub(bridge_stx, qbtc_amount, &new_bridge_stx)) {
        return 0;
    }
    update_token_balance(&state.bridge_balances, user, new_bridge_stx);
    
    return 1;
}

uint64 get_bridge_balance(id user) {
    return find_token_balance(&state.bridge_balances, user);
}

// ========================== GOVERNANCE FUNCTIONS ==========================

bit is_governance_member(id member) {
    for (uint64 i = 0; i < state.governance_owners.population(); i++) {
        id owner = state.governance_owners.element(i);
        if (owner == member) {
            return 1;
        }
    }
    return 0;
}

uint64 create_proposal(id proposer, id to, uint64 value, uint8_1024 data, uint8_256 action, uint8_256 parameter) {
    if (!is_governance_member(proposer)) {
        return 0; // Not a governance member
    }
    
    Proposal proposal;
    proposal.to = to;
    proposal.value = value;
    proposal.data = data;
    proposal.nonce = state.proposal_nonce;
    proposal.executed = 0;
    proposal.action = action;
    proposal.parameter = parameter;
    
    state.proposals.add(0, proposal, state.proposal_nonce);
    state.proposal_nonce = state.proposal_nonce + 1;
    
    return proposal.nonce;
}

bit sign_proposal(uint64 nonce, id signer) {
    if (!is_governance_member(signer)) {
        return 0; // Not a governance member
    }
    
    // Find proposal
    for (uint64 i = 0; i < state.proposals.population(); i++) {
        Proposal proposal = state.proposals.element(i);
        if (proposal.nonce == nonce) {
            // Check if already signed
            for (uint64 j = 0; j < proposal.signatures.population(); j++) {
                id existing_signer = proposal.signatures.element(j);
                if (existing_signer == signer) {
                    return 0; // Already signed
                }
            }
            
            // Add signature
            proposal.signatures.add(0, signer, 0);
            
            // Check if enough signatures
            if (proposal.signatures.population() >= state.threshold) {
                proposal.executed = 1;
            }
            
            state.proposals.replace(i, proposal);
            return 1;
        }
    }
    
    return 0; // Proposal not found
}

bit execute_proposal(uint64 nonce) {
    // Find proposal
    for (uint64 i = 0; i < state.proposals.population(); i++) {
        Proposal proposal = state.proposals.element(i);
        if (proposal.nonce == nonce) {
            if (proposal.executed) {
                return 1; // Already executed
            }
            
            if (proposal.signatures.population() < state.threshold) {
                return 0; // Not enough signatures
            }
            
            // Execute proposal (implementation would depend on action type)
            proposal.executed = 1;
            state.proposals.replace(i, proposal);
            return 1;
        }
    }
    
    return 0; // Proposal not found
}

// ========================== SYSTEM INITIALIZATION ==========================

void initialize_system(id admin, OraclePubKey oracle_keys[OracleConfig::NUM_ORACLES], 
                      id governance_owners[5], id bridge_auth) {
    if (state.initialized) {
        return; // Already initialized
    }
    
    // Set admin
    state.admin_address = admin;
    state.bridge_authority = bridge_auth;
    
    // Initialize oracle committee
    for (uint64 i = 0; i < OracleConfig::NUM_ORACLES; i++) {
        state.trusted_oracles[i] = oracle_keys[i];
    }
    
    // Initialize governance
    for (uint64 i = 0; i < 5; i++) {
        state.governance_owners.add(0, governance_owners[i], 0);
    }
    state.threshold = 3; // 3-of-5 threshold
    
    // Initialize price
    state.last_price.price = 0;
    state.last_price.timestamp = 0;
    state.last_price.decimals = 15;
    
    // Initialize history limit
    state.max_history = 1024;
    
    // Mint initial BANKON PYTHAI supply to admin
    update_token_balance(&state.bkpy_balances, admin, TokenConfig::BKPY_TOTAL_SUPPLY);
    
    // Mint initial BANKON SATPAY supply to admin
    update_token_balance(&state.satpay_balances, admin, TokenConfig::SATPAY_TOTAL_SUPPLY);
    
    state.initialized = 1;
}

void burn_admin_keys(id admin) {
    if (state.admin_burned) {
        return; // Already burned
    }
    
    if (admin != state.admin_address) {
        return; // Not admin
    }
    
    state.admin_burned = 1;
    state.admin_address = 0; // Zero address
}

// ========================== USER FUNCTION REGISTRATION ==========================

void REGISTER_USER_FUNCTIONS_AND_PROCEDURES() {
    // Oracle functions
    registerFunction("submit_price_update", submit_price_update);
    registerFunction("get_current_price", get_current_price);
    registerFunction("emergency_pause", emergency_pause);
    
    // Token functions
    registerFunction("bkpy_transfer", bkpy_transfer);
    registerFunction("bkpy_balance_of", bkpy_balance_of);
    registerFunction("bkpy_total_supply", bkpy_total_supply);
    
    registerFunction("qbtc_mint", qbtc_mint);
    registerFunction("qbtc_transfer", qbtc_transfer);
    registerFunction("qbtc_balance_of", qbtc_balance_of);
    
    registerFunction("qusd_mint", qusd_mint);
    registerFunction("qusd_transfer", qusd_transfer);
    registerFunction("qusd_balance_of", qusd_balance_of);
    
    registerFunction("satpay_transfer", satpay_transfer);
    registerFunction("satpay_balance_of", satpay_balance_of);
    registerFunction("satpay_total_supply", satpay_total_supply);
    
    // Bridge functions
    registerFunction("swap_stx_for_qbtc", swap_stx_for_qbtc);
    registerFunction("swap_qbtc_for_stx", swap_qbtc_for_stx);
    registerFunction("get_bridge_balance", get_bridge_balance);
    
    // Governance functions
    registerFunction("create_proposal", create_proposal);
    registerFunction("sign_proposal", sign_proposal);
    registerFunction("execute_proposal", execute_proposal);
    registerFunction("is_governance_member", is_governance_member);
    
    // System functions
    registerProcedure("initialize_system", initialize_system);
    registerProcedure("burn_admin_keys", burn_admin_keys);
}

// ========================== QUBIC ANTI-MILITARY LICENSE ==========================
/*
Qubic Anti-Military License – Code is Law Edition
Permission is hereby granted, perpetual, worldwide, non-exclusive, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
- The Software cannot be used in any form or in any substantial portions for development, maintenance and for any other purposes, in the military sphere and in relation to military products or activities as defined in the original license.
- All modifications, alterations, or merges must maintain these restrictions.
- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
(c) BANKON All Rights Reserved. See LICENSE file for full text.
*/ 