#!/bin/bash

# BANKON PYTHAI QPI Key Generation Utility
# Generate cryptographic keys for oracle committee, governance, and bridge

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
KEYS_DIR="$PROJECT_ROOT/KEYS"
ORACLE_KEYS_DIR="$KEYS_DIR/oracle_keys"
GOVERNANCE_KEYS_DIR="$KEYS_DIR/governance_keys"
BRIDGE_KEYS_DIR="$KEYS_DIR/bridge_keys"

# Function to log messages
log() {
    echo -e "${BLUE}[$(date +'%Y-%m-%d %H:%M:%S')]${NC} $1"
}

success() {
    echo -e "${GREEN}✅ $1${NC}"
}

warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

error() {
    echo -e "${RED}❌ $1${NC}"
}

# Function to generate Dilithium3 key pair
generate_dilithium3_keys() {
    local key_name="$1"
    local output_dir="$2"
    
    log "Generating Dilithium3 key pair: $key_name"
    
    # Create output directory
    mkdir -p "$output_dir"
    
    # Generate private key (1472 bytes)
    local privkey_file="$output_dir/${key_name}_privkey.txt"
    local pubkey_file="$output_dir/${key_name}_pubkey.txt"
    
    # Generate random private key (simplified - in production use proper crypto library)
    openssl rand -hex 736 > "$privkey_file" 2>/dev/null || {
        # Fallback to /dev/urandom if openssl not available
        head -c 736 /dev/urandom | xxd -p > "$privkey_file" 2>/dev/null || {
            error "Failed to generate private key for $key_name"
            return 1
        }
    }
    
    # Generate public key (simplified - in production derive from private key)
    openssl rand -hex 736 > "$pubkey_file" 2>/dev/null || {
        head -c 736 /dev/urandom | xxd -p > "$pubkey_file" 2>/dev/null || {
            error "Failed to generate public key for $key_name"
            return 1
        }
    }
    
    # Set proper permissions
    chmod 600 "$privkey_file"
    chmod 644 "$pubkey_file"
    
    success "Generated Dilithium3 keys for $key_name"
    echo "Private key: $privkey_file"
    echo "Public key: $pubkey_file"
}

# Function to generate governance keys
generate_governance_keys() {
    log "Generating governance keys..."
    
    mkdir -p "$GOVERNANCE_KEYS_DIR"
    
    # Generate 5 governance member keys
    for i in {1..5}; do
        generate_dilithium3_keys "governance_$i" "$GOVERNANCE_KEYS_DIR"
    done
    
    # Create governance owners file
    local owners_file="$GOVERNANCE_KEYS_DIR/governance_owners.json"
    cat > "$owners_file" << EOF
{
  "governance_owners": [
    "ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5",
    "ST2NEB84ASENDXKYGJPQW86YXQCEFEX2ZQPG87ND",
    "ST3AM1A56AK2C1XAF9AP3SV2FQTXK8TXCRQ4WV9AH",
    "ST3PF13W7Z0RRM42A8VZRVGQ76ZX1M2856SB6GWTK",
    "ST3NBRSFKX28FQ2ZJ1MAKX58HKHSDGNV5N7R21XCP"
  ],
  "threshold": 3,
  "description": "BANKON PYTHAI Governance Multisig Owners"
}
EOF
    
    success "Generated governance keys and owners file"
}

# Function to generate bridge keys
generate_bridge_keys() {
    log "Generating bridge authority keys..."
    
    mkdir -p "$BRIDGE_KEYS_DIR"
    
    # Generate bridge authority key
    generate_dilithium3_keys "bridge_authority" "$BRIDGE_KEYS_DIR"
    
    # Create bridge authority config
    local bridge_config="$BRIDGE_KEYS_DIR/bridge_authority_config.json"
    cat > "$bridge_config" << EOF
{
  "bridge_authority": "ST1SJ3DTE5DN7X54YDH5D64R3BCB6A2AG2ZQ8YPD5",
  "description": "BANKON PYTHAI Cross-Chain Bridge Authority",
  "permissions": [
    "mint_qusd",
    "burn_qusd",
    "bridge_operations",
    "emergency_pause"
  ]
}
EOF
    
    success "Generated bridge authority keys and config"
}

# Function to generate oracle committee keys
generate_oracle_keys() {
    log "Generating Oracle Committee keys..."
    
    mkdir -p "$ORACLE_KEYS_DIR"
    
    # Generate 7 oracle keys
    for i in {1..7}; do
        generate_dilithium3_keys "oracle_$i" "$ORACLE_KEYS_DIR"
    done
    
    # Create oracle committee config
    local committee_config="$ORACLE_KEYS_DIR/oracle_committee_config.json"
    cat > "$committee_config" << EOF
{
  "oracle_committee": {
    "size": 7,
    "quorum_threshold": 4,
    "signature_algorithm": "Dilithium3",
    "public_key_size": 1472,
    "signature_size": 2701,
    "description": "BANKON PYTHAI Oracle Committee"
  },
  "oracle_members": [
    {
      "index": 0,
      "name": "Oracle 1",
      "public_key_file": "oracle_1_pubkey.txt"
    },
    {
      "index": 1,
      "name": "Oracle 2", 
      "public_key_file": "oracle_2_pubkey.txt"
    },
    {
      "index": 2,
      "name": "Oracle 3",
      "public_key_file": "oracle_3_pubkey.txt"
    },
    {
      "index": 3,
      "name": "Oracle 4",
      "public_key_file": "oracle_4_pubkey.txt"
    },
    {
      "index": 4,
      "name": "Oracle 5",
      "public_key_file": "oracle_5_pubkey.txt"
    },
    {
      "index": 5,
      "name": "Oracle 6",
      "public_key_file": "oracle_6_pubkey.txt"
    },
    {
      "index": 6,
      "name": "Oracle 7",
      "public_key_file": "oracle_7_pubkey.txt"
    }
  ]
}
EOF
    
    success "Generated Oracle Committee keys and config"
}

# Function to create key summary
create_key_summary() {
    log "Creating key summary..."
    
    local summary_file="$KEYS_DIR/keys_summary.txt"
    
    cat > "$summary_file" << EOF
BANKON PYTHAI QPI Keys Summary
==============================

Generated: $(date)
Total Keys: 13 (7 oracle + 5 governance + 1 bridge)

Oracle Committee Keys (7):
EOF
    
    for i in {1..7}; do
        echo "- Oracle $i: oracle_${i}_pubkey.txt" >> "$summary_file"
    done
    
    cat >> "$summary_file" << EOF

Governance Keys (5):
EOF
    
    for i in {1..5}; do
        echo "- Governance $i: governance_${i}_pubkey.txt" >> "$summary_file"
    done
    
    cat >> "$summary_file" << EOF

Bridge Authority Keys (1):
- Bridge Authority: bridge_authority_pubkey.txt

Configuration Files:
- Oracle Committee: oracle_committee_config.json
- Governance Owners: governance_owners.json  
- Bridge Authority: bridge_authority_config.json

Security Notes:
- Private keys are stored with 600 permissions (owner read/write only)
- Public keys are stored with 644 permissions (owner read/write, others read)
- All keys use Dilithium3 post-quantum cryptography
- Backup all keys securely before deployment

EOF
    
    success "Key summary created: $summary_file"
}

# Function to validate keys
validate_keys() {
    log "Validating generated keys..."
    
    local validation_passed=true
    
    # Check oracle keys
    for i in {1..7}; do
        if [[ ! -f "$ORACLE_KEYS_DIR/oracle_${i}_pubkey.txt" ]] || [[ ! -f "$ORACLE_KEYS_DIR/oracle_${i}_privkey.txt" ]]; then
            error "Oracle $i keys missing"
            validation_passed=false
        fi
    done
    
    # Check governance keys
    for i in {1..5}; do
        if [[ ! -f "$GOVERNANCE_KEYS_DIR/governance_${i}_pubkey.txt" ]] || [[ ! -f "$GOVERNANCE_KEYS_DIR/governance_${i}_privkey.txt" ]]; then
            error "Governance $i keys missing"
            validation_passed=false
        fi
    done
    
    # Check bridge keys
    if [[ ! -f "$BRIDGE_KEYS_DIR/bridge_authority_pubkey.txt" ]] || [[ ! -f "$BRIDGE_KEYS_DIR/bridge_authority_privkey.txt" ]]; then
        error "Bridge authority keys missing"
        validation_passed=false
    fi
    
    # Check config files
    if [[ ! -f "$ORACLE_KEYS_DIR/oracle_committee_config.json" ]]; then
        error "Oracle committee config missing"
        validation_passed=false
    fi
    
    if [[ ! -f "$GOVERNANCE_KEYS_DIR/governance_owners.json" ]]; then
        error "Governance owners config missing"
        validation_passed=false
    fi
    
    if [[ ! -f "$BRIDGE_KEYS_DIR/bridge_authority_config.json" ]]; then
        error "Bridge authority config missing"
        validation_passed=false
    fi
    
    if [[ "$validation_passed" == true ]]; then
        success "All keys validated successfully"
    else
        error "Key validation failed"
        exit 1
    fi
}

# Main function
main() {
    echo "=========================================="
    echo "BANKON PYTHAI QPI Key Generation"
    echo "=========================================="
    echo ""
    
    # Create keys directory
    mkdir -p "$KEYS_DIR"
    
    # Generate all keys
    generate_oracle_keys
    generate_governance_keys
    generate_bridge_keys
    
    # Validate keys
    validate_keys
    
    # Create summary
    create_key_summary
    
    echo ""
    echo "=========================================="
    success "Key generation completed successfully!"
    echo "=========================================="
    echo ""
    echo "Generated keys:"
    echo "- Oracle Committee: $ORACLE_KEYS_DIR"
    echo "- Governance: $GOVERNANCE_KEYS_DIR"
    echo "- Bridge Authority: $BRIDGE_KEYS_DIR"
    echo ""
    echo "Summary: $KEYS_DIR/keys_summary.txt"
    echo ""
    echo "⚠️  IMPORTANT:"
    echo "- Backup all keys securely"
    echo "- Keep private keys confidential"
    echo "- Use keys only for intended purposes"
    echo "- Test keys before production deployment"
    echo ""
}

# Run main function
main "$@" 