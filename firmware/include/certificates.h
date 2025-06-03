#ifndef CERTIFICATES_H
#define CERTIFICATES_H

#include <pgmspace.h>

// IMPORTANT: Replace these placeholders with your actual PEM-encoded certificates and key.
// Ensure they are correctly formatted as C strings for PROGMEM.
// Example: Convert .pem to .h using: xxd -i my_cert.pem > my_cert.h
// Then copy the array content here.

// Placeholder for the CA certificate (to verify the server)
const char ca_cert_pem[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
PASTE_YOUR_CA_CERTIFICATE_PEM_HERE
-----END CERTIFICATE-----
)EOF";

// Placeholder for the Client certificate
const char client_cert_pem[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
PASTE_YOUR_CLIENT_CERTIFICATE_PEM_HERE
-----END CERTIFICATE-----
)EOF";

// Placeholder for the Client private key
// IMPORTANT: Protecting this key is crucial. Embedding in firmware has security implications.
const char client_key_pem[] PROGMEM = R"EOF(
-----BEGIN PRIVATE KEY-----
PASTE_YOUR_CLIENT_PRIVATE_KEY_PEM_HERE
-----END PRIVATE KEY-----
)EOF";

#endif // CERTIFICATES_H
