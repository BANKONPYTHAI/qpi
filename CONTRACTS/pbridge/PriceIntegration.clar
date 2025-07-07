;; Pyth Integration Supplemental Contract for qOracle
;; Provides functions to interact with Pyth oracle on Stacks blockchain
;; Enables zero-cost price retrieval from Pyth price feeds

(define-trait pyth-storage-trait
  (
    (read (buff 32) (response (tuple (price int) (conf uint) (expo int) (ema-price int) (ema-conf uint) (publish-time uint) (prev-publish-time uint)) uint))
    (read-price-with-staleness-check (buff 32) (response (tuple (price int) (conf uint) (expo int) (ema-price int) (ema-conf uint) (publish-time uint) (prev-publish-time uint)) uint))
  )
)

(define-constant pyth-oracle-contract 'SP3R4F6C1J3JQWWCVZ3S7FRRYPMYG6ZW6RZK31FXY.pyth-oracle-v3)
(define-constant pyth-storage-contract 'SP3R4F6C1J3JQWWCVZ3S7FRRYPMYG6ZW6RZK31FXY.pyth-storage-v3)

;; BTC-USD price feed ID on mainnet (example)
(define-constant btc-price-feed-id 0xe62df6c8b4a85fe1a67db44dc12de5db330f7ac66b72dc658afedf0f4a415b43)

(define-public (get-btc-price)
  (contract-call? pyth-oracle-contract read-price-feed btc-price-feed-id pyth-storage-contract)
)

;; Function to submit verified VAA price update to Pyth oracle contract
(define-public (submit-price-update (vaa (buff 8192)) (execution-plan (tuple (pyth-storage-contract principal) (pyth-decoder-contract principal) (wormhole-core-contract principal))))
  (contract-call? pyth-oracle-contract verify-and-update-price-feeds vaa execution-plan)
)
