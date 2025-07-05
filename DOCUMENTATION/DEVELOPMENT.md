# BANKON PYTHAI Development Guide

## Development Environment Setup

### Prerequisites

1. **Qubic CLI** - Latest version for contract compilation and deployment
2. **QPI Documentation** - Qubic Programming Interface reference
3. **Development Tools** - Text editor with C++ syntax highlighting
4. **Test Environment** - Qubic testnet access
5. **Version Control** - Git for code management and collaboration

### Directory Structure

```
qOracle/production/qpi/
├── qOracle_QPI_Compliant.cpp          # Main oracle system (25KB, 841 lines)
├── BANKON_PYTHAI_QPI_Compliant.cpp    # Native token (10KB, 374 lines)
├── qBTC_Synthetic_QPI_Compliant.cpp   # Synthetic BTC (15KB, 570 lines)
├── qUSD_Stablecoin_QPI_Compliant.cpp  # Stablecoin (12KB, 451 lines)
├── BANKON_SATPAY_QPI_Compliant.cpp    # Micro-unit token (12KB, 436 lines)
├── test_qpi_contracts_new.sh          # Compliance test script (6.8KB, 198 lines)
├── QPI_COMPLIANCE_TEST_RESULTS.txt    # Test results (465B, 16 lines)
├── EXPLANATION.md                     # System documentation
└── DEVELOPMENT.md                     # This file
```

## QPI Coding Standards

### Required Compliance Rules

1. **No C++ Arrays**
   ```cpp
   // ❌ Forbidden
   uint64_t balances[100];
   
   // ✅ Required
   collection<TokenBalance> balances;
   ```

2. **No Pointers or Casting**
   ```cpp
   // ❌ Forbidden
   uint64_t* ptr = &value;
   uint64_t cast = (uint64_t)other_value;
   
   // ✅ Required
   uint64 value = other_value;
   ```

3. **No Native Data Types**
   ```cpp
   // ❌ Forbidden
   int, long, char, bool, float, double
   
   // ✅ Required
   uint64, id, bit, uint8_*
   ```

4. **No #include Statements**
   ```cpp
   // ❌ Forbidden
   #include <stdio.h>
   #include "myheader.h"
   
   // ✅ Required
   // All dependencies provided by QPI
   ```

5. **No % or / Operators**
   ```cpp
   // ❌ Forbidden
   uint64_t result = a % b;
   uint64_t quotient = a / b;
   
   // ✅ Required
   uint64 result = mod(a, b);
   uint64 quotient = div(a, b);
   ```

6. **No Local Variable Declarations**
   ```cpp
   // ❌ Forbidden
   void function() {
       uint64 local_var = 0;
   }
   
   // ✅ Required
   struct ContractState {
       uint64 temp_var;
   };
   ```

### Data Type Usage

#### Basic Types
```cpp
uint64 amount;        // 64-bit unsigned integer for amounts
id owner;             // Qubic identity/address for ownership
bit is_valid;         // Boolean (0 or 1) for flags
uint8_256 name;       // Fixed-size byte array for strings
uint8 precision;      // 8-bit unsigned for precision flags
```

#### Collections
```cpp
collection<TokenBalance> balances;           // Dynamic collection for balances
collection<OracleSignature> signatures;      // Oracle signatures collection
collection<Proposal> proposals;              // Governance proposals
collection<PriceMessage> price_history;      // Price history tracking
```

#### Structs
```cpp
struct TokenBalance {
    id owner;
    uint64 amount;
};

struct OracleSignature {
    uint64 signer_index;
    uint8_2701 signature; // Dilithium3 signature
};
```

### Function Structure

#### Standard Function Template
```cpp
bit function_name(id sender, uint64 amount) {
    // Input validation
    if (!state.initialized) {
        return 0;
    }
    
    // Business logic using safe arithmetic
    uint64 new_amount;
    if (!safe_add(state.current_amount, amount, &new_amount)) {
        return 0; // Overflow
    }
    
    // State updates
    state.current_amount = new_amount;
    
    return 1; // Success
}
```

#### Safe Arithmetic Functions
```cpp
// Always use these for arithmetic operations
bit safe_add(uint64 a, uint64 b, uint64* result);
bit safe_sub(uint64 a, uint64 b, uint64* result);
bit safe_mul(uint64 a, uint64 b, uint64* result);
bit safe_div(uint64 a, uint64 b, uint64* result);
bit safe_mod(uint64 a, uint64 b, uint64* result);
```

## Development Workflow

### 1. Contract Development

1. **Create Contract File**
   ```bash
   touch MyContract_QPI_Compliant.cpp
   ```

2. **Add Standard Header**
   ```cpp
   /*
    * Contract Name - QPI Compliant
    * Code is Law – Qubic Anti-Military License
    * Qubic Programming Interface (QPI) Compliant Implementation
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
    * (c) BANKON All Rights Reserved
    */
   ```

3. **Define Data Structures**
   ```cpp
   struct Config {
       static constexpr uint64_t DECIMALS = 12;
       static constexpr uint64_t TOTAL_SUPPLY = 1000000000000000ULL;
   };
   
   struct ContractState {
       collection<TokenBalance> balances;
       bit initialized;
       id admin_address;
       struct {
           uint64 temp_amount;
           uint64 temp_balance;
       } locals;
   };
   ```

4. **Implement Functions**
   ```cpp
   bit initialize(id admin) {
       if (state.initialized) {
           return 0;
       }
       state.admin_address = admin;
       state.initialized = 1;
       return 1;
   }
   ```

5. **Register Functions**
   ```cpp
   void REGISTER_USER_FUNCTIONS_AND_PROCEDURES() {
       // Register all public functions here
   }
   ```

### 2. Testing

#### Run QPI Compliance Tests
```bash
cd qOracle/production/qpi
./test_qpi_contracts_new.sh
```

#### Test Coverage Checklist
- [ ] QPI registration found
- [ ] No forbidden types detected
- [ ] QPI collection types used
- [ ] QPI data types used
- [ ] Contract file exists and valid size
- [ ] All functions properly registered

#### Manual Testing
```bash
# Test individual contract
grep -vE '^[[:space:]]*//|^[[:space:]]*/\*|^[[:space:]]*\*|^[[:space:]]*\*/|^[[:space:]]*$' MyContract.cpp | sed 's|//.*||' | grep -vE '^[[:space:]]*$' | grep -P '(^|[;\s])int[\s\*]'
```

### 3. Code Review

#### Compliance Checklist
- [ ] No C++ arrays used
- [ ] No pointers or casting
- [ ] No native data types
- [ ] No #include statements
- [ ] No % or / operators
- [ ] No local variable declarations
- [ ] No typedef/union
- [ ] No floating point types
- [ ] All functions registered
- [ ] Safe arithmetic used
- [ ] Input validation implemented
- [ ] Error handling present

#### Security Review
- [ ] Overflow protection
- [ ] Access control
- [ ] Input validation
- [ ] State consistency
- [ ] No single points of failure
- [ ] Oracle integration (if applicable)
- [ ] Bridge security (if applicable)

## Implementation Patterns

### Token Transfer Pattern
```cpp
bit transfer(id sender, id to, uint64 amount) {
    if (!state.initialized) return 0;
    if (sender == to || amount == 0) return 0;
    
    uint64 sender_balance = find_balance(sender);
    if (sender_balance < amount) return 0;
    
    uint64 to_balance = find_balance(to);
    
    uint64 new_sender_balance;
    if (!safe_sub(sender_balance, amount, &new_sender_balance)) return 0;
    update_balance(sender, new_sender_balance);
    
    uint64 new_to_balance;
    if (!safe_add(to_balance, amount, &new_to_balance)) return 0;
    update_balance(to, new_to_balance);
    
    return 1;
}
```

### Oracle Validation Pattern
```cpp
bit validate_oracle_update(PriceUpdate update) {
    if (update.signatures.population() < OracleConfig::QUORUM_THRESHOLD) {
        return 0;
    }
    
    for (uint64 i = 0; i < update.signatures.population(); i++) {
        OracleSignature sig = update.signatures.element(i);
        OraclePubKey pubkey = state.trusted_oracles[sig.signer_index];
        
        if (!validate_signature(pubkey, update.message, sig.signature)) {
            return 0;
        }
    }
    
    return 1;
}
```

### Governance Pattern
```cpp
bit create_proposal(id proposer, uint8_256 action) {
    if (!is_governance_member(proposer)) return 0;
    
    Proposal proposal;
    proposal.action = action;
    proposal.nonce = state.proposal_nonce;
    proposal.executed = 0;
    
    state.proposals.add(0, proposal, 0);
    state.proposal_nonce++;
    
    return 1;
}
```

### Bridge Integration Pattern
```cpp
bit bridge_mint(id sender, id to, uint64 amount) {
    if (sender != state.bridge_authority) return 0;
    
    uint64 new_total_supply;
    if (!safe_add(state.total_supply, amount, &new_total_supply)) return 0;
    
    if (new_total_supply > TokenConfig::MAX_SUPPLY) return 0;
    
    uint64 to_balance = find_token_balance(to);
    uint64 new_balance;
    if (!safe_add(to_balance, amount, &new_balance)) return 0;
    
    update_token_balance(to, new_balance);
    state.total_supply = new_total_supply;
    
    return 1;
}
```

### Price Message Serialization Pattern
```cpp
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
```

## Contract-Specific Guidelines

### Oracle System Development
- Implement Dilithium3 signature verification
- Use 7-of-7 oracle committee with 4-of-7 quorum
- Include timestamp skew protection (10 minutes)
- Maintain price history for audit trails
- Implement emergency pause functionality

### Token Development
- Use collection-based balance storage
- Implement safe arithmetic for all operations
- Include admin key burning mechanism
- Provide ERC20-compatible interface functions
- Add formatting utilities for human readability

### Bridge Development
- Implement bridge authority controls
- Use oracle price validation for swaps
- Include supply caps and limits
- Provide balance tracking and verification
- Implement zero-fee operations

### Governance Development
- Use configurable consensus thresholds
- Implement proposal nonce tracking
- Include execution delay mechanisms
- Provide signature collection and validation
- Support multiple governance actions

## Testing Procedures

### Automated Testing
```bash
# Run full test suite
./test_qpi_contracts_new.sh

# Test individual contract
./test_qpi_contracts_new.sh | grep "Contract Name"

# Check test results
cat QPI_COMPLIANCE_TEST_RESULTS.txt
```

### Manual Testing
```bash
# Check for forbidden types
grep -r "int\|long\|char\|bool" *.cpp

# Verify QPI registration
grep -r "REGISTER_USER_FUNCTIONS_AND_PROCEDURES" *.cpp

# Check collection usage
grep -r "collection<" *.cpp

# Verify safe arithmetic
grep -r "safe_add\|safe_sub\|safe_mul\|safe_div\|safe_mod" *.cpp
```

### Compilation Testing
```bash
# Test compilation (if qubic-cli available)
qubic-cli compile MyContract_QPI_Compliant.cpp

# Check for compilation errors
qubic-cli compile MyContract_QPI_Compliant.cpp 2>&1 | grep -i error
```

## Deployment Process

### 1. Pre-Deployment Checklist

- [ ] All QPI compliance tests pass
- [ ] Code review completed
- [ ] Security audit passed
- [ ] Testnet testing completed
- [ ] Documentation updated
- [ ] Function registration verified
- [ ] Safe arithmetic implemented
- [ ] Error handling comprehensive

### 2. Compilation

```bash
# Using qubic-cli
qubic-cli compile MyContract_QPI_Compliant.cpp

# Verify compilation success
if [ $? -eq 0 ]; then
    echo "Compilation successful"
else
    echo "Compilation failed"
    exit 1
fi
```

### 3. Deployment

```bash
# Deploy to testnet first
qubic-cli deploy MyContract_QPI_Compliant.cpp --testnet

# Verify deployment
qubic-cli get-contract <contract-address>

# Deploy to mainnet after testing
qubic-cli deploy MyContract_QPI_Compliant.cpp --mainnet
```

### 4. Post-Deployment

- [ ] Verify contract address
- [ ] Test all functions
- [ ] Update documentation
- [ ] Monitor for issues
- [ ] Verify oracle integration (if applicable)
- [ ] Test bridge operations (if applicable)
- [ ] Validate governance functions (if applicable)

## Troubleshooting

### Common Issues

1. **QPI Registration Missing**
   - Ensure `REGISTER_USER_FUNCTIONS_AND_PROCEDURES()` is implemented
   - Check that all public functions are registered
   - Verify function names match exactly

2. **Forbidden Types Detected**
   - Remove any `int`, `long`, `char`, `bool` declarations
   - Replace with `uint64`, `id`, `bit`, `uint8_*`
   - Check comments for forbidden type mentions

3. **Compilation Errors**
   - Check for missing semicolons
   - Verify all structs are properly defined
   - Ensure no local variable declarations
   - Check for proper QPI type usage

4. **Runtime Errors**
   - Add more input validation
   - Check for overflow conditions
   - Verify state consistency
   - Implement proper error handling

### Debugging Tips

1. **Use Safe Arithmetic**
   - Always check return values from safe_* functions
   - Handle overflow/underflow gracefully
   - Log arithmetic operation failures

2. **Validate Inputs**
   - Check for null/zero values
   - Validate parameter ranges
   - Ensure state is initialized
   - Verify access permissions

3. **Test Incrementally**
   - Test each function individually
   - Use small test cases first
   - Verify edge cases
   - Test error conditions

## Best Practices

### Code Organization
- Group related functions together
- Use clear, descriptive names
- Add comments for complex logic
- Keep functions small and focused
- Follow consistent naming conventions

### Error Handling
- Always return meaningful error codes
- Validate all inputs thoroughly
- Handle edge cases gracefully
- Use safe arithmetic operations
- Implement comprehensive logging

### Security
- Implement proper access control
- Validate all external inputs
- Use quantum-resistant cryptography
- Avoid single points of failure
- Include emergency controls

### Performance
- Minimize state updates
- Use efficient data structures
- Avoid unnecessary computations
- Optimize for gas efficiency
- Cache frequently accessed data

### Testing
- Write comprehensive test suites
- Test all edge cases
- Verify error conditions
- Test integration points
- Validate security measures

## Resources

### Documentation
- QPI Reference Manual
- Qubic Network Documentation
- BANKON PYTHAI Whitepaper
- Dilithium3 Cryptography Documentation

### Tools
- Qubic CLI
- QPI Compliance Checker
- Test Scripts
- Code Review Tools

### Community
- Qubic Discord
- BANKON Community
- Developer Forums
- Security Auditors

## Conclusion

Following these development guidelines ensures that your contracts are QPI-compliant, secure, and maintainable. Always test thoroughly and review code carefully before deployment. The BANKON PYTHAI ecosystem provides a solid foundation for building quantum-resistant DeFi applications on the Qubic network.

**Key Success Factors:**
- 100% QPI compliance across all contracts
- Comprehensive security implementation
- Thorough testing and validation
- Clear documentation and guidelines
- Production-ready deployment process

This development guide provides the framework for building secure, compliant, and efficient smart contracts on the Qubic network. 