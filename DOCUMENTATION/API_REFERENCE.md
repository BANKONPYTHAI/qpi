# BANKON PYTHAI QPI API Reference

## Overview

This document provides a comprehensive API reference for the BANKON PYTHAI QPI-compliant ecosystem. All functions are 100% QPI-compliant and ready for production use.

## Contract Addresses

### Main Contracts
- **Oracle System**: `qOracle_QPI_Compliant.cpp`
- **BANKON PYTHAI Token**: `BANKON_PYTHAI_QPI_Compliant.cpp`
- **qBTC Synthetic Token**: `qBTC_Synthetic_QPI_Compliant.cpp`
- **qUSD Stablecoin**: `qUSD_Stablecoin_QPI_Compliant.cpp`
- **BANKON SATPAY Token**: `BANKON_SATPAY_QPI_Compliant.cpp`

## Data Types

### QPI-Compliant Types
```cpp
uint64 amount;           // 64-bit unsigned integer
id owner;                // Qubic identity/address
bit is_valid;            // Boolean (0 or 1)
uint8_256 name;          // Fixed-size byte array (256 bytes)
uint8_1472 pubkey;       // Dilithium3 public key
uint8_2701 signature;    // Dilithium3 signature
collection<T> items;     // Dynamic collection
```

### Common Structures
```cpp
struct TokenBalance {
    id owner;
    uint64 amount;
};

struct PriceMessage {
    uint64 price;      // Fixed-point price (15 decimals)
    uint64 timestamp;  // Unix timestamp
    uint8 decimals;    // Price precision
};

struct OracleSignature {
    uint64 signer_index;
    uint8_2701 signature;
};

struct PriceUpdate {
    PriceMessage message;
    collection<OracleSignature> signatures;
};
```

## Oracle System API

### Oracle Committee Functions

#### `initialize_oracle_committee`
Initialize the oracle committee with quantum-resistant keys.

```cpp
bit initialize_oracle_committee(id admin, OraclePubKey oracle_keys[7])
```

**Parameters:**
- `admin`: Admin address for initialization
- `oracle_keys`: Array of 7 Dilithium3 public keys

**Returns:**
- `1`: Success
- `0`: Failure

**Example:**
```bash
qubic-cli call <oracle-contract> initialize_oracle_committee \
  --admin ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5 \
  --oracle-keys oracle_keys.json
```

#### `submit_price_update`
Submit a price update with oracle signatures.

```cpp
bit submit_price_update(PriceUpdate update)
```

**Parameters:**
- `update`: Price update with signatures

**Returns:**
- `1`: Success
- `0`: Failure

**Example:**
```bash
qubic-cli call <oracle-contract> submit_price_update \
  --price 50000000000000000 \
  --timestamp 1640995200 \
  --signatures signatures.json
```

#### `get_current_price`
Get the current oracle price.

```cpp
PriceMessage get_current_price()
```

**Returns:**
- `PriceMessage`: Current price data

**Example:**
```bash
qubic-cli call <oracle-contract> get_current_price
```

#### `emergency_pause`
Emergency pause the oracle system.

```cpp
bit emergency_pause(id sender)
```

**Parameters:**
- `sender`: Sender address (must be admin)

**Returns:**
- `1`: Success
- `0`: Failure

**Example:**
```bash
qubic-cli call <oracle-contract> emergency_pause \
  --sender ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5
```

## Token API

### BANKON PYTHAI Token Functions

#### `initialize_token`
Initialize the BANKON PYTHAI token.

```cpp
bit initialize_token(id admin)
```

**Parameters:**
- `admin`: Admin address

**Returns:**
- `1`: Success
- `0`: Failure

#### `transfer`
Transfer BANKON PYTHAI tokens.

```cpp
bit transfer(id sender, id to, uint64 amount)
```

**Parameters:**
- `sender`: Sender address
- `to`: Recipient address
- `amount`: Transfer amount

**Returns:**
- `1`: Success
- `0`: Failure

**Example:**
```bash
qubic-cli call <bkpy-contract> transfer \
  --sender ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5 \
  --to ST2NEB84ASENDXKYGJPQW86YXQCEFEX2ZQPG87ND \
  --amount 1000000000000000
```

#### `balance_of`
Get token balance for an address.

```cpp
uint64 balance_of(id addr)
```

**Parameters:**
- `addr`: Address to query

**Returns:**
- `uint64`: Token balance

**Example:**
```bash
qubic-cli call <bkpy-contract> balance_of \
  --addr ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5
```

#### `total_supply`
Get total token supply.

```cpp
uint64 total_supply()
```

**Returns:**
- `uint64`: Total supply

**Example:**
```bash
qubic-cli call <bkpy-contract> total_supply
```

#### `get_symbol`
Get token symbol.

```cpp
void get_symbol(uint8_256* symbol)
```

**Parameters:**
- `symbol`: Output buffer for symbol

**Example:**
```bash
qubic-cli call <bkpy-contract> get_symbol
```

#### `get_name`
Get token name.

```cpp
void get_name(uint8_256* name)
```

**Parameters:**
- `name`: Output buffer for name

**Example:**
```bash
qubic-cli call <bkpy-contract> get_name
```

#### `decimals`
Get token decimals.

```cpp
uint64 decimals()
```

**Returns:**
- `uint64`: Number of decimals

**Example:**
```bash
qubic-cli call <bkpy-contract> decimals
```

### qBTC Synthetic Token Functions

#### `initialize_token`
Initialize the qBTC synthetic token.

```cpp
bit initialize_token(id admin, OraclePubKey oracle_keys[7])
```

**Parameters:**
- `admin`: Admin address
- `oracle_keys`: Oracle committee public keys

**Returns:**
- `1`: Success
- `0`: Failure

#### `mint`
Mint qBTC tokens with BTC collateral.

```cpp
bit mint(id user, uint64 btc_sats, PriceUpdate price_update)
```

**Parameters:**
- `user`: User address
- `btc_sats`: BTC amount in satoshis
- `price_update`: Oracle price update

**Returns:**
- `1`: Success
- `0`: Failure

**Example:**
```bash
qubic-cli call <qbtc-contract> mint \
  --user ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5 \
  --btc-sats 100000000 \
  --price-update price_update.json
```

#### `burn`
Burn qBTC tokens for BTC.

```cpp
bit burn(id user, uint64 amount)
```

**Parameters:**
- `user`: User address
- `amount`: Amount to burn

**Returns:**
- `1`: Success
- `0`: Failure

**Example:**
```bash
qubic-cli call <qbtc-contract> burn \
  --user ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5 \
  --amount 100000000
```

#### `transfer`
Transfer qBTC tokens.

```cpp
bit transfer(id sender, id to, uint64 amount)
```

**Parameters:**
- `sender`: Sender address
- `to`: Recipient address
- `amount`: Transfer amount

**Returns:**
- `1`: Success
- `0`: Failure

### qUSD Stablecoin Functions

#### `initialize_token`
Initialize the qUSD stablecoin.

```cpp
bit initialize_token(id admin, id bridge_auth)
```

**Parameters:**
- `admin`: Admin address
- `bridge_auth`: Bridge authority address

**Returns:**
- `1`: Success
- `0`: Failure

#### `mint`
Mint qUSD tokens (bridge authority only).

```cpp
bit mint(id sender, id to, uint64 amount)
```

**Parameters:**
- `sender`: Sender address (must be bridge authority)
- `to`: Recipient address
- `amount`: Amount to mint

**Returns:**
- `1`: Success
- `0`: Failure

**Example:**
```bash
qubic-cli call <qusd-contract> mint \
  --sender ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5 \
  --to ST2NEB84ASENDXKYGJPQW86YXQCEFEX2ZQPG87ND \
  --amount 1000000000000000
```

#### `burn`
Burn qUSD tokens (bridge authority only).

```cpp
bit burn(id sender, id from, uint64 amount)
```

**Parameters:**
- `sender`: Sender address (must be bridge authority)
- `from`: Address to burn from
- `amount`: Amount to burn

**Returns:**
- `1`: Success
- `0`: Failure

#### `transfer`
Transfer qUSD tokens.

```cpp
bit transfer(id sender, id to, uint64 amount)
```

**Parameters:**
- `sender`: Sender address
- `to`: Recipient address
- `amount`: Transfer amount

**Returns:**
- `1`: Success
- `0`: Failure

### BANKON SATPAY Token Functions

#### `initialize_token`
Initialize the BANKON SATPAY token.

```cpp
bit initialize_token(id admin)
```

**Parameters:**
- `admin`: Admin address

**Returns:**
- `1`: Success
- `0`: Failure

#### `transfer`
Transfer SATPAY tokens.

```cpp
bit transfer(id sender, id to, uint64 amount)
```

**Parameters:**
- `sender`: Sender address
- `to`: Recipient address
- `amount`: Transfer amount

**Returns:**
- `1`: Success
- `0`: Failure

#### `qbtc_to_satpay`
Convert qBTC amount to SATPAY.

```cpp
uint64 qbtc_to_satpay(uint64 qbtc_amount)
```

**Parameters:**
- `qbtc_amount`: qBTC amount

**Returns:**
- `uint64`: SATPAY amount

**Example:**
```bash
qubic-cli call <satpay-contract> qbtc_to_satpay \
  --qbtc-amount 100000000
```

#### `satpay_to_qbtc`
Convert SATPAY amount to qBTC.

```cpp
uint64 satpay_to_qbtc(uint64 satpay_amount)
```

**Parameters:**
- `satpay_amount`: SATPAY amount

**Returns:**
- `uint64`: qBTC amount

**Example:**
```bash
qubic-cli call <satpay-contract> satpay_to_qbtc \
  --satpay-amount 100000000
```

## Bridge API

### Cross-Chain Bridge Functions

#### `swap_stx_for_qbtc`
Swap STX for qBTC using oracle price.

```cpp
bit swap_stx_for_qbtc(id user, uint64 stx_amount, PriceUpdate price_update)
```

**Parameters:**
- `user`: User address
- `stx_amount`: STX amount
- `price_update`: Oracle price update

**Returns:**
- `1`: Success
- `0`: Failure

**Example:**
```bash
qubic-cli call <bridge-contract> swap_stx_for_qbtc \
  --user ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5 \
  --stx-amount 1000000000 \
  --price-update price_update.json
```

#### `swap_qbtc_for_stx`
Swap qBTC for STX using oracle price.

```cpp
bit swap_qbtc_for_stx(id user, uint64 qbtc_amount, PriceUpdate price_update)
```

**Parameters:**
- `user`: User address
- `qbtc_amount`: qBTC amount
- `price_update`: Oracle price update

**Returns:**
- `1`: Success
- `0`: Failure

**Example:**
```bash
qubic-cli call <bridge-contract> swap_qbtc_for_stx \
  --user ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5 \
  --qbtc-amount 100000000 \
  --price-update price_update.json
```

#### `get_bridge_balance`
Get user's bridge balance.

```cpp
uint64 get_bridge_balance(id user)
```

**Parameters:**
- `user`: User address

**Returns:**
- `uint64`: Bridge balance

**Example:**
```bash
qubic-cli call <bridge-contract> get_bridge_balance \
  --user ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5
```

## Governance API

### Qnosis Multisig Functions

#### `create_proposal`
Create a governance proposal.

```cpp
uint64 create_proposal(id proposer, id to, uint64 value, uint8_1024 data, 
                      uint8_256 action, uint8_256 parameter)
```

**Parameters:**
- `proposer`: Proposer address
- `to`: Target address
- `value`: Value to send
- `data`: Proposal data
- `action`: Action description
- `parameter`: Parameter description

**Returns:**
- `uint64`: Proposal nonce

**Example:**
```bash
qubic-cli call <governance-contract> create_proposal \
  --proposer ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5 \
  --to ST2NEB84ASENDXKYGJPQW86YXQCEFEX2ZQPG87ND \
  --value 0 \
  --data 0x \
  --action "Update Oracle Committee" \
  --parameter "Add new oracle member"
```

#### `sign_proposal`
Sign a governance proposal.

```cpp
bit sign_proposal(uint64 nonce, id signer)
```

**Parameters:**
- `nonce`: Proposal nonce
- `signer`: Signer address

**Returns:**
- `1`: Success
- `0`: Failure

**Example:**
```bash
qubic-cli call <governance-contract> sign_proposal \
  --nonce 1 \
  --signer ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5
```

#### `execute_proposal`
Execute a governance proposal.

```cpp
bit execute_proposal(uint64 nonce)
```

**Parameters:**
- `nonce`: Proposal nonce

**Returns:**
- `1`: Success
- `0`: Failure

**Example:**
```bash
qubic-cli call <governance-contract> execute_proposal \
  --nonce 1
```

#### `is_governance_member`
Check if address is a governance member.

```cpp
bit is_governance_member(id member)
```

**Parameters:**
- `member`: Member address

**Returns:**
- `1`: Is member
- `0`: Not member

**Example:**
```bash
qubic-cli call <governance-contract> is_governance_member \
  --member ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5
```

## System Management API

### Admin Functions

#### `initialize_system`
Initialize the complete system.

```cpp
void initialize_system(id admin, OraclePubKey oracle_keys[7], 
                      id governance_owners[5], id bridge_auth)
```

**Parameters:**
- `admin`: Admin address
- `oracle_keys`: Oracle committee keys
- `governance_owners`: Governance member addresses
- `bridge_auth`: Bridge authority address

**Example:**
```bash
qubic-cli call <main-contract> initialize_system \
  --admin ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5 \
  --oracle-keys oracle_keys.json \
  --governance-owners governance_owners.json \
  --bridge-auth ST2NEB84ASENDXKYGJPQW86YXQCEFEX2ZQPG87ND
```

#### `burn_admin_keys`
Burn admin keys for permanent decentralization.

```cpp
void burn_admin_keys(id admin)
```

**Parameters:**
- `admin`: Admin address

**Example:**
```bash
qubic-cli call <main-contract> burn_admin_keys \
  --admin ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5
```

#### `finalize_launch`
Finalize system launch.

```cpp
bit finalize_launch(id sender)
```

**Parameters:**
- `sender`: Sender address (must be admin)

**Returns:**
- `1`: Success
- `0`: Failure

**Example:**
```bash
qubic-cli call <main-contract> finalize_launch \
  --sender ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5
```

## Error Codes

### Common Error Codes
- `0`: Operation failed
- `1`: Operation successful

### Specific Error Conditions
- **Contract not initialized**: Call `initialize_*` function first
- **Admin key burned**: Contract is immutable
- **Insufficient balance**: User doesn't have enough tokens
- **Invalid parameters**: Parameters out of valid range
- **Access denied**: Caller doesn't have required permissions
- **Oracle validation failed**: Price update validation failed
- **Overflow/underflow**: Arithmetic operation failed

## Usage Examples

### Complete Token Transfer
```bash
# 1. Check balance
qubic-cli call <bkpy-contract> balance_of \
  --addr ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5

# 2. Transfer tokens
qubic-cli call <bkpy-contract> transfer \
  --sender ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5 \
  --to ST2NEB84ASENDXKYGJPQW86YXQCEFEX2ZQPG87ND \
  --amount 1000000000000000

# 3. Verify transfer
qubic-cli call <bkpy-contract> balance_of \
  --addr ST2NEB84ASENDXKYGJPQW86YXQCEFEX2ZQPG87ND
```

### Oracle Price Update
```bash
# 1. Get current price
qubic-cli call <oracle-contract> get_current_price

# 2. Submit price update (requires oracle signatures)
qubic-cli call <oracle-contract> submit_price_update \
  --price 50000000000000000 \
  --timestamp 1640995200 \
  --signatures signatures.json

# 3. Verify price update
qubic-cli call <oracle-contract> get_current_price
```

### Cross-Chain Bridge Swap
```bash
# 1. Check STX balance
qubic-cli call <bridge-contract> get_bridge_balance \
  --user ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5

# 2. Swap STX for qBTC
qubic-cli call <bridge-contract> swap_stx_for_qbtc \
  --user ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5 \
  --stx-amount 1000000000 \
  --price-update price_update.json

# 3. Check qBTC balance
qubic-cli call <qbtc-contract> balance_of \
  --addr ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5
```

### Governance Proposal
```bash
# 1. Create proposal
qubic-cli call <governance-contract> create_proposal \
  --proposer ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5 \
  --to ST2NEB84ASENDXKYGJPQW86YXQCEFEX2ZQPG87ND \
  --value 0 \
  --data 0x \
  --action "Update Oracle Committee" \
  --parameter "Add new oracle member"

# 2. Sign proposal
qubic-cli call <governance-contract> sign_proposal \
  --nonce 1 \
  --signer ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5

# 3. Execute proposal (after delay)
qubic-cli call <governance-contract> execute_proposal \
  --nonce 1
```

## Best Practices

### Security
1. **Always validate inputs**: Check parameters before processing
2. **Use safe arithmetic**: Use safe_* functions for all calculations
3. **Check permissions**: Verify caller has required access
4. **Handle errors**: Always check return values
5. **Monitor events**: Track important state changes

### Performance
1. **Batch operations**: Group related operations when possible
2. **Optimize gas usage**: Minimize state updates
3. **Use efficient data structures**: Leverage QPI collections
4. **Cache frequently accessed data**: Store in contract state

### Integration
1. **Test thoroughly**: Test all functions before production
2. **Monitor contracts**: Track contract state and events
3. **Handle failures gracefully**: Implement proper error handling
4. **Document changes**: Keep API documentation updated

This API reference provides complete coverage of all BANKON PYTHAI QPI-compliant functions for production deployment. 