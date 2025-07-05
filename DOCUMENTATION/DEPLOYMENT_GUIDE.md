# BANKON PYTHAI QPI Deployment Guide

## Overview

This guide provides step-by-step instructions for deploying the complete BANKON PYTHAI QPI-compliant ecosystem to the Qubic network. All contracts are 100% QPI-compliant and ready for mainnet deployment.

## Prerequisites

### Required Software
- **Qubic CLI**: Latest version for contract compilation and deployment
- **Git**: For version control and repository management
- **Bash**: For running deployment scripts
- **Text Editor**: For configuration file editing

### Required Access
- **Qubic Testnet Access**: For initial testing
- **Qubic Mainnet Access**: For production deployment
- **Stacks Network Access**: For cross-chain bridge deployment

### Required Keys
- **Oracle Committee Keys**: 7 Dilithium3 key pairs
- **Governance Keys**: Multisig wallet keys
- **Bridge Authority Keys**: Cross-chain bridge keys

## Deployment Architecture

### System Components
1. **Oracle Committee**: 7-of-7 quantum-resistant oracle system
2. **Token Contracts**: BKPY, qBTC, qUSD, SATPAY tokens
3. **Cross-Chain Bridge**: Trustless STX ↔ qBTC swaps
4. **Governance System**: Qnosis v1 multisig governance
5. **Monitoring System**: Real-time system monitoring

### Deployment Order
1. Oracle Committee (Foundation)
2. Token Contracts (Core Assets)
3. Cross-Chain Bridge (Interoperability)
4. Governance System (Management)
5. Integration Testing (Validation)

## Step 1: Environment Setup

### 1.1 Clone Repository
```bash
git clone https://github.com/BANKONPYTHAI/qOracle.git
cd qOracle/production/qpi
```

### 1.2 Verify Qubic CLI
```bash
qubic-cli --version
qubic-cli --help
```

### 1.3 Check QPI Compliance
```bash
./test_qpi_contracts_new.sh
```

Expected output:
```
Tests Passed: 5
Tests Failed: 0
Total Tests: 5
```

## Step 2: Oracle Committee Deployment

### 2.1 Generate Oracle Keys
```bash
cd KEYS/oracle_keys
./generate_oracle_keys.sh
```

This creates 7 Dilithium3 key pairs for the oracle committee.

### 2.2 Deploy Oracle Committee
```bash
cd ../../DEPLOYMENT
./deploy_oracle_committee.sh
```

### 2.3 Verify Oracle Deployment
```bash
qubic-cli get-contract <oracle-contract-address>
```

### 2.4 Initialize Oracle Committee
```bash
qubic-cli call <oracle-contract-address> initialize_oracle_committee \
  --oracle-keys oracle_keys.json
```

## Step 3: Token Contract Deployment

### 3.1 Deploy BANKON PYTHAI Token
```bash
./deploy_tokens.sh --token BKPY
```

### 3.2 Deploy qBTC Synthetic Token
```bash
./deploy_tokens.sh --token QBTC
```

### 3.3 Deploy qUSD Stablecoin
```bash
./deploy_tokens.sh --token QUSD
```

### 3.4 Deploy BANKON SATPAY Token
```bash
./deploy_tokens.sh --token SATPAY
```

### 3.5 Verify Token Deployments
```bash
./verify_tokens.sh
```

## Step 4: Cross-Chain Bridge Deployment

### 4.1 Deploy Bridge Contract
```bash
./deploy_bridge.sh
```

### 4.2 Configure Bridge Parameters
```bash
qubic-cli call <bridge-contract-address> configure_bridge \
  --oracle-address <oracle-contract-address> \
  --qbtc-address <qbtc-contract-address> \
  --qusd-address <qusd-contract-address>
```

### 4.3 Test Bridge Operations
```bash
./test_bridge.sh
```

## Step 5: Governance System Deployment

### 5.1 Deploy Qnosis Multisig
```bash
./deploy_governance.sh
```

### 5.2 Configure Governance Parameters
```bash
qubic-cli call <governance-contract-address> configure_governance \
  --threshold 3 \
  --proposal-duration 604800 \
  --execution-delay 86400
```

### 5.3 Add Governance Members
```bash
qubic-cli call <governance-contract-address> add_member \
  --member <member-address>
```

## Step 6: System Integration

### 6.1 Connect All Components
```bash
./integrate_system.sh
```

### 6.2 Initialize System State
```bash
qubic-cli call <main-contract-address> initialize_system \
  --admin <admin-address> \
  --oracle-address <oracle-contract-address> \
  --bridge-address <bridge-contract-address> \
  --governance-address <governance-contract-address>
```

### 6.3 Verify Integration
```bash
./verify_integration.sh
```

## Step 7: Testing & Validation

### 7.1 Run Integration Tests
```bash
cd ../TESTING
./test_integration.sh
```

### 7.2 Run Security Tests
```bash
./test_security.sh
```

### 7.3 Run Performance Tests
```bash
./test_performance.sh
```

### 7.4 Validate All Functions
```bash
./validate_functions.sh
```

## Step 8: Monitoring Setup

### 8.1 Configure Monitoring
```bash
cd ../MONITORING
./setup_monitoring.sh
```

### 8.2 Start Oracle Monitoring
```bash
./monitor_oracle.sh &
```

### 8.3 Start Bridge Monitoring
```bash
./monitor_bridge.sh &
```

### 8.4 Start Governance Monitoring
```bash
./monitor_governance.sh &
```

## Step 9: Production Launch

### 9.1 Final Verification
```bash
./final_verification.sh
```

### 9.2 Burn Admin Keys
```bash
qubic-cli call <main-contract-address> burn_admin_keys \
  --admin <admin-address>
```

### 9.3 Activate System
```bash
qubic-cli call <main-contract-address> activate_system
```

### 9.4 Launch Announcement
```bash
./announce_launch.sh
```

## Configuration Files

### Oracle Configuration (`configs/oracle_config.txt`)
```
Oracle Committee Configuration
=============================

Committee Size: 7
Quorum Threshold: 4-of-7
Signature Algorithm: Dilithium3
Public Key Size: 1472 bytes
Signature Size: 2701 bytes

Oracle Members:
Oracle 1: <oracle-1-pubkey>
Oracle 2: <oracle-2-pubkey>
Oracle 3: <oracle-3-pubkey>
Oracle 4: <oracle-4-pubkey>
Oracle 5: <oracle-5-pubkey>
Oracle 6: <oracle-6-pubkey>
Oracle 7: <oracle-7-pubkey>

Deployment Date: <deployment-date>
Status: ACTIVE
```

### Bridge Configuration (`configs/bridge_config.txt`)
```
Cross-Chain Bridge Configuration
===============================

Supported Pairs:
- STX ↔ qBTC
- qBTC ↔ STX

Bridge Features:
- Trustless swaps
- Oracle integration
- Zero protocol fees
- Integer-only math
- Overflow protection
- Security delays: 1 hour

Bridge Authority: <bridge-authority-address>
Deployment Date: <deployment-date>
Status: ACTIVE
```

### Governance Configuration (`configs/governance_config.txt`)
```
Governance Multisig Configuration
=================================

Threshold: 3-of-N signatures
Proposal Duration: 7 days
Execution Delay: 24 hours
Minimum Quorum: 2 proposals

Governance Members:
- Primary: <primary-member-address>
- Secondary: <secondary-member-address>
- Tertiary: <tertiary-member-address>

Emergency Procedures:
- Circuit breaker: ACTIVE
- Emergency multisig: READY
- Volume limits: 1M STX equivalent

Deployment Date: <deployment-date>
Status: ACTIVE
```

## Contract Addresses

After deployment, record all contract addresses:

```bash
# Contract Addresses
ORACLE_CONTRACT="<oracle-contract-address>"
BKPY_CONTRACT="<bkpy-contract-address>"
QBTC_CONTRACT="<qbtc-contract-address>"
QUSD_CONTRACT="<qusd-contract-address>"
SATPAY_CONTRACT="<satpay-contract-address>"
BRIDGE_CONTRACT="<bridge-contract-address>"
GOVERNANCE_CONTRACT="<governance-contract-address>"
MAIN_CONTRACT="<main-contract-address>"

# Save to file
echo "ORACLE_CONTRACT=$ORACLE_CONTRACT" > contract_addresses.txt
echo "BKPY_CONTRACT=$BKPY_CONTRACT" >> contract_addresses.txt
echo "QBTC_CONTRACT=$QBTC_CONTRACT" >> contract_addresses.txt
echo "QUSD_CONTRACT=$QUSD_CONTRACT" >> contract_addresses.txt
echo "SATPAY_CONTRACT=$SATPAY_CONTRACT" >> contract_addresses.txt
echo "BRIDGE_CONTRACT=$BRIDGE_CONTRACT" >> contract_addresses.txt
echo "GOVERNANCE_CONTRACT=$GOVERNANCE_CONTRACT" >> contract_addresses.txt
echo "MAIN_CONTRACT=$MAIN_CONTRACT" >> contract_addresses.txt
```

## Verification Checklist

### Pre-Deployment
- [ ] QPI compliance tests pass (5/5)
- [ ] Security audit completed
- [ ] Testnet deployment successful
- [ ] All configuration files prepared
- [ ] Oracle keys generated and secured
- [ ] Governance keys prepared
- [ ] Bridge authority keys ready

### Deployment
- [ ] Oracle committee deployed and initialized
- [ ] All token contracts deployed
- [ ] Bridge contract deployed and configured
- [ ] Governance system deployed and configured
- [ ] System integration completed
- [ ] All contract addresses recorded

### Post-Deployment
- [ ] Integration tests pass
- [ ] Security tests pass
- [ ] Performance tests pass
- [ ] Monitoring systems active
- [ ] Emergency procedures tested
- [ ] Documentation updated
- [ ] Admin keys burned (optional)

## Troubleshooting

### Common Issues

1. **QPI Compliance Failures**
   ```bash
   # Re-run compliance tests
   ./test_qpi_contracts_new.sh
   
   # Check specific violations
   grep -r "int\|long\|char\|bool" CONTRACTS/
   ```

2. **Deployment Failures**
   ```bash
   # Check qubic-cli status
   qubic-cli status
   
   # Check network connectivity
   qubic-cli ping
   
   # Review deployment logs
   tail -f logs/deployment.log
   ```

3. **Integration Issues**
   ```bash
   # Verify contract addresses
   ./verify_addresses.sh
   
   # Check contract state
   qubic-cli get-contract <contract-address>
   
   # Test individual functions
   ./test_functions.sh
   ```

### Emergency Procedures

1. **System Pause**
   ```bash
   qubic-cli call <main-contract-address> emergency_pause
   ```

2. **Oracle Committee Update**
   ```bash
   qubic-cli call <governance-contract-address> propose_oracle_update
   ```

3. **Bridge Emergency Stop**
   ```bash
   qubic-cli call <bridge-contract-address> emergency_stop
   ```

## Security Considerations

### Key Management
- Store private keys securely
- Use hardware security modules (HSMs) for production
- Implement key rotation procedures
- Backup keys in secure locations

### Access Control
- Limit admin access to essential personnel
- Implement multi-signature requirements
- Use time-locked governance actions
- Monitor all administrative actions

### Monitoring
- Monitor oracle price updates
- Track bridge operations
- Monitor governance proposals
- Alert on suspicious activity

## Performance Optimization

### Oracle Updates
- Optimize signature verification
- Implement batch processing
- Use efficient data structures
- Monitor gas usage

### Bridge Operations
- Optimize swap calculations
- Implement price protection
- Monitor liquidity pools
- Track cross-chain synchronization

### Governance
- Optimize proposal processing
- Implement efficient voting
- Monitor governance activity
- Track execution delays

## Maintenance

### Regular Tasks
- Monitor system performance
- Update oracle committee keys
- Review governance proposals
- Backup system state
- Update documentation

### Updates
- Plan contract upgrades
- Test new features
- Deploy security patches
- Update monitoring systems
- Train operators

## Support

### Documentation
- API Reference: `DOCUMENTATION/API_REFERENCE.md`
- User Guide: `DOCUMENTATION/USER_GUIDE.md`
- Development Guide: `DOCUMENTATION/DEVELOPMENT.md`
- Security Audit: `DOCUMENTATION/SECURITY_AUDIT.md`

### Community
- GitHub Issues: Report bugs and feature requests
- Discord: Community support and discussions
- Documentation: Comprehensive guides and tutorials
- Security: Responsible disclosure procedures

This deployment guide ensures a secure, compliant, and successful deployment of the BANKON PYTHAI QPI-compliant ecosystem. 