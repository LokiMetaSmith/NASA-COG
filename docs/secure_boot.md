Secure Boot [^1] and TrustZone [^2] are a series of techniques to ensure the integrity of the firmware and to prevent unauthorized modification without detection. Modern microprocessors use cryptographic techniques to achieve this security. 

By establishing a unique cryptographic idenity that is protected from being copied, modified, or compromised, devices can securely sign messages with a private key. The recipient of these messages can then verify that the underlying firmware is secure, ensuring that only trusted software can be executed during the boot process.

For example, on the ESP32-S3, Secure Boot is implemented by ensuring that the firmware is cryptographically signed, and this signature is verified during the boot process . Similarly, ARM's TrustZone architecture uses Secure Boot to create a chain of trust, starting from a root of trust that validates each subsequent stage of the boot process . This approach helps maintain the integrity and security of the system from the moment it powers on.


[^1]: https://docs.espressif.com/projects/esp-idf/en/v5.2.2/esp32s3/security/security.html

[^2]: https://developer.arm.com/documentation/PRD29-GENC-009492/c/TrustZone-Software-Architecture/Booting-a-secure-system/Secure-boot

