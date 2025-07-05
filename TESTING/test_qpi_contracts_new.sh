#!/bin/bash

# QPI-Compliant Contracts Test Suite
# Tests all QPI-compliant contracts for BANKON PYTHAI ecosystem

# Remove 'set -e' to allow all tests to run

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test counter
TESTS_PASSED=0
TESTS_FAILED=0

# Array to store summary lines
SUMMARY=()

# Function to run a test
run_test() {
    local test_name="$1"
    local contract_file="$2"
    local result=""
    
    echo -e "${BLUE}Testing: ${test_name}${NC}"
    echo "Contract: $contract_file"
    
    if [ -f "$contract_file" ]; then
        echo -e "${GREEN}✓ Contract file exists${NC}"
        
        # Check file size
        local file_size=$(stat -c%s "$contract_file")
        echo "File size: ${file_size} bytes"
        
        # Remove comments and blank lines for compliance checks
        local code_only=$(grep -vE '^[[:space:]]*/\*|^[[:space:]]*\*|^[[:space:]]*\*/' "$contract_file" | sed 's|//.*||' | grep -vE '^[[:space:]]*$')
        
        # Check for QPI compliance indicators
        if echo "$code_only" | grep -q "REGISTER_USER_FUNCTIONS_AND_PROCEDURES"; then
            echo -e "${GREEN}✓ QPI registration found${NC}"
        else
            echo -e "${RED}✗ QPI registration missing${NC}"
            SUMMARY+=("$test_name: QPI registration missing ❌")
            ((TESTS_FAILED++))
            return
        fi
        
        # Check for prohibited C++ features (in filtered code only)
        if echo "$code_only" | grep -q "std::"; then
            echo -e "${RED}✗ STL containers detected${NC}"
            SUMMARY+=("$test_name: STL containers detected ❌")
            ((TESTS_FAILED++))
            return
        fi
        
        # Check for forbidden types (in filtered code only)
        if echo "$code_only" | grep -Pq '(^|[;\s])int[\s\*]'; then
            echo -e "${RED}✗ Non-QPI data type 'int' detected${NC}"
            SUMMARY+=("$test_name: Non-QPI data type 'int' detected ❌")
            ((TESTS_FAILED++))
            return
        fi
        if echo "$code_only" | grep -Pq '(^|[;\s])long[\s\*]'; then
            echo -e "${RED}✗ Non-QPI data type 'long' detected${NC}"
            SUMMARY+=("$test_name: Non-QPI data type 'long' detected ❌")
            ((TESTS_FAILED++))
            return
        fi
        if echo "$code_only" | grep -Pq '(^|[;\s])char[\s\*]'; then
            echo -e "${RED}✗ Non-QPI data type 'char' detected${NC}"
            SUMMARY+=("$test_name: Non-QPI data type 'char' detected ❌")
            ((TESTS_FAILED++))
            return
        fi
        if echo "$code_only" | grep -Pq '(^|[;\s])bool[\s\*]'; then
            echo -e "${RED}✗ Non-QPI data type 'bool' detected${NC}"
            SUMMARY+=("$test_name: Non-QPI data type 'bool' detected ❌")
            ((TESTS_FAILED++))
            return
        fi
        
        # Check for QPI-compliant features
        local features=""
        if echo "$code_only" | grep -q "collection<"; then
            echo -e "${GREEN}✓ QPI collection types used${NC}"
            features+="collection "
        fi
        if echo "$code_only" | grep -q "bit\|uint8\|uint64\|id"; then
            echo -e "${GREEN}✓ QPI data types used${NC}"
            features+="QPI-types "
        fi
        
        echo -e "${GREEN}✓ Test PASSED${NC}"
        SUMMARY+=("$test_name: PASSED ✅ $features")
        ((TESTS_PASSED++))
        return
    else
        echo -e "${RED}✗ Contract file not found${NC}"
        SUMMARY+=("$test_name: Contract file not found ❌")
        ((TESTS_FAILED++))
        return
    fi
}

# Function to test contract compilation (if qubic-cli available)
test_compilation() {
    local contract_file="$1"
    local test_name="$2"
    
    echo -e "${BLUE}Testing compilation: ${test_name}${NC}"
    
    if command -v ../../qubic-cli &> /dev/null; then
        echo "Attempting compilation with qubic-cli..."
        
        # Create a temporary test directory
        local temp_dir=$(mktemp -d)
        cp "$contract_file" "$temp_dir/"
        cd "$temp_dir"
        
        # Try to compile (this may fail but we're testing the process)
        if timeout 30s ../../qubic-cli --help &> /dev/null; then
            echo -e "${GREEN}✓ qubic-cli is available${NC}"
            echo -e "${YELLOW}Note: Full compilation test requires proper QPI environment${NC}"
        else
            echo -e "${YELLOW}⚠ qubic-cli not fully functional in test environment${NC}"
        fi
        
        cd - > /dev/null
        rm -rf "$temp_dir"
        
    else
        echo -e "${YELLOW}⚠ qubic-cli not available for compilation test${NC}"
    fi
}

# Main test execution
echo "=========================================="
echo "QPI-COMPLIANT CONTRACTS TEST SUITE"
echo "=========================================="
echo "Testing all QPI-compliant contracts..."
echo ""
echo "Starting QPI compliance tests..."
echo ""

# Test 1: Main Oracle System
run_test "Main Oracle System" "qOracle_QPI_Compliant.cpp"
test_compilation "qOracle_QPI_Compliant.cpp" "Main Oracle System"
echo ""

# Test 2: BANKON PYTHAI Token
run_test "BANKON PYTHAI Token" "BANKON_PYTHAI_QPI_Compliant.cpp"
test_compilation "BANKON_PYTHAI_QPI_Compliant.cpp" "BANKON PYTHAI Token"
echo ""

# Test 3: qBTC Synthetic Token
run_test "qBTC Synthetic Token" "qBTC_Synthetic_QPI_Compliant.cpp"
test_compilation "qBTC_Synthetic_QPI_Compliant.cpp" "qBTC Synthetic Token"
echo ""

# Test 4: qUSD Stablecoin
run_test "qUSD Stablecoin" "qUSD_Stablecoin_QPI_Compliant.cpp"
test_compilation "qUSD_Stablecoin_QPI_Compliant.cpp" "qUSD Stablecoin"
echo ""

# Test 5: BANKON SATPAY Token
run_test "BANKON SATPAY Token" "BANKON_SATPAY_QPI_Compliant.cpp"
test_compilation "BANKON_SATPAY_QPI_Compliant.cpp" "BANKON SATPAY Token"
echo ""

# Summary
echo "=========================================="
echo "TEST SUMMARY"
echo "=========================================="
echo -e "Tests Passed: ${GREEN}${TESTS_PASSED}${NC}"
echo -e "Tests Failed: ${RED}${TESTS_FAILED}${NC}"
echo "Total Tests: $((TESTS_PASSED + TESTS_FAILED))"
echo ""
echo "Contract Results:"
for line in "${SUMMARY[@]}"; do
    echo "- $line"
done

echo "" > QPI_COMPLIANCE_TEST_RESULTS.txt
echo "==========================================" >> QPI_COMPLIANCE_TEST_RESULTS.txt
echo "QPI-COMPLIANT CONTRACTS TEST SUITE" >> QPI_COMPLIANCE_TEST_RESULTS.txt
echo "==========================================" >> QPI_COMPLIANCE_TEST_RESULTS.txt
echo "" >> QPI_COMPLIANCE_TEST_RESULTS.txt
echo "Tests Passed: $TESTS_PASSED" >> QPI_COMPLIANCE_TEST_RESULTS.txt
echo "Tests Failed: $TESTS_FAILED" >> QPI_COMPLIANCE_TEST_RESULTS.txt
echo "Total Tests: $((TESTS_PASSED + TESTS_FAILED))" >> QPI_COMPLIANCE_TEST_RESULTS.txt
echo "" >> QPI_COMPLIANCE_TEST_RESULTS.txt
echo "Contract Results:" >> QPI_COMPLIANCE_TEST_RESULTS.txt
for line in "${SUMMARY[@]}"; do
    echo "- $line" >> QPI_COMPLIANCE_TEST_RESULTS.txt
done 