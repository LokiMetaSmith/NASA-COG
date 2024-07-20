

## Secure Billing Using Secure Boot and TrustZone

### Overview

Secure Boot is a series of techniques designed to ensure the integrity of firmware and prevent unauthorized modifications without detection. Leveraging cryptographic methods, Secure Boot creates a unique cryptographic identity for the device, protecting it from being copied, modified, or compromised. This ensures that only trusted software is executed during the boot process.

Espressif's Secure Boot[^1]  and ARM's TrustZone technology[^2] provides a hardware-enforced security boundary between the secure world (trusted) and the non-secure world (untrusted) within a single processor. This separation ensures that sensitive operations can be carried out in a secure environment, even if the non-secure world is compromised.

[^1]: https://docs.espressif.com/projects/esp-idf/en/v5.2.2/esp32s3/security/security.html

[^2]: https://developer.arm.com/documentation/PRD29-GENC-009492/c/TrustZone-Software-Architecture/Booting-a-secure-system/Secure-boot


### Secure Billing System Design

**1. Logging Coulombs:**

- The firmware integrates (sums) the amperage over time to calculate the total number of coulombs used, ensuring accurate billing.
- The log of total coulombs is stored in non-volatile memory, such as EEPROM or an external flash memory connected via SPI or I2C. This ensures that the data is retained even if the device is powered off or disconnected from the internet.

**2. Secure Storage:**

- The logs are encrypted using a secure encryption algorithm before being stored, preventing unauthorized access or tampering.
- Encryption keys are securely stored within the device’s secure hardware, such as the ESP32’s Secure Boot or ARM’s TrustZone capabilities.

**3. Offline Operation:**

- The device can operate offline and continue logging the total coulombs. When the device reconnects to the internet, it transmits the encrypted logs to the billing system.
- Each log entry includes a timestamp and a cryptographic signature to ensure authenticity and integrity.

**4. External Server Monthly Billing:**

* An external server reads the encrypted logs transmitted by the device, when the device is online.
* The cryptographic signature ensures that the logs cannot be altered without detection.
* The server decrypts the logs and verifies the cryptographic signatures to ensure the data's integrity.
* Based on the verified data, the server generates the monthly bill. The bill includes the total coulombs logged during the period.
- At the end of each billing period, an external server  generates a bill of usage for a customer.
- This bill includes the total coulombs logged during the period and is transmitted to the billing system.


**5. Verification and Integrity:**

- To prevent cheating by either party, the system must be designed to allow for independent verification of the oxygen production.
- A pressure transducer and other sensors validate the oxygen produced, cross-referencing with the logged coulombs, providing an additional layer of integrity.

### Implementation Example

**ESP32-S3:**

- The ESP32-S3’s Secure Boot feature ensures the integrity of the firmware.
- Encrypted logs are stored in external flash memory via SPI or I2C, utilizing the ESP32’s built-in cryptographic capabilities for secure storage and encryption.

**ARM TrustZone:**

- ARM’s TrustZone creates a secure environment for storing encryption keys and performing cryptographic operations.
- Logs are securely stored in non-volatile memory, with TrustZone ensuring that only trusted software can access and modify these logs.

### Diagram

```plaintext
                          +-----------------------+
                          |                       |
                          |     Secure Boot       |
                          |   (ESP32 / TrustZone) |
                          |                       |
                          +-----------------------+
                                    |
                                    |
                                    V
                       +---------------------------+
                       |                           |
                       |  Firmware Integrates      |
                       |  Amperage Over Time       |
                       |  (Calculates Coulombs)    |
                       |                           |
                       +---------------------------+
                                    |
                                    |
                                    V
                       +---------------------------+
                       |                           |
                       |    Store Encrypted Logs   |
                       |  (EEPROM / External Flash)|
                       |                           |
                       +---------------------------+
                                    |
                                    |
                                    V
                       +---------------------------+
                       |                           |
                       |    Generate Monthly Bill  |
                       |  (Signed with Secret Key) |
                       |                           |
                       +---------------------------+
                                    |
                                    |
                                    V
                       +---------------------------+
                       |                           |
                       |    Transmit Bill and Logs |
                       |    When Online            |
                       |                           |
                       +---------------------------+
```

### Conclusion

This secure billing system ensures that the reported usage is accurate and tamper-proof, providing a reliable basis for billing in a commercial setting. By leveraging Secure Boot and TrustZone technologies, the system can operate independently of an internet connection while maintaining the integrity and authenticity of the usage data.
