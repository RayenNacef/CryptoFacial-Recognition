#include <WiFi.h>
#include <WiFiClient.h>
#include <mbedtls/aes.h> // Include mbedtls AES library

const byte key[16] = { 
    0x31, 0x32, 0x33, 0x34, 
    0x35, 0x36, 0x37, 0x38, 
    0x39, 0x30, 0x31, 0x32, 
    0x33, 0x34, 0x35, 0x36 
}; // 16-byte key

void setup() {
    Serial.begin(115200); // Initialize Serial communication

    // Encrypted data (provided)
    byte encryptedData[] = {
        0x2d, 0x71, 0xd7, 0xb8, 0xf7, 0x85, 0xdc, 0x05, 
        0x64, 0x1d, 0xc8, 0xce, 0xc0, 0x57, 0x4d, 0x23, 
        0x65, 0xf1, 0xba, 0x5e, 0x63, 0x36, 0x12, 0xe2, 
        0xaf, 0x7c, 0x70, 0x92, 0x4a, 0xea, 0x54, 0x99, 
        0x30, 0x17, 0x19, 0x6f, 0x36, 0x37, 0xf8, 0x4c, 
        0xfd, 0x1c, 0xa0, 0x49, 0x98, 0x0a, 0x99, 0xc9
    };

    // Decrypted data buffer
    byte decryptedData[128];
    int decryptedLength = decryptAES(encryptedData, sizeof(encryptedData), decryptedData, key);

    // Output decrypted data to Serial
    if (decryptedLength > 0) {
        Serial.println("Decrypted Message:");
        for (int i = 0; i < decryptedLength; i++) {
            Serial.print((char)decryptedData[i]);
        }
        Serial.println();
    } else {
        Serial.println("Decryption failed or padding was invalid.");
    }
}

int decryptAES(byte *encryptedData, int dataLength, byte *decryptedData, const byte *key) {
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);

    // Extract IV from the encrypted data
    byte iv[16];
    memcpy(iv, encryptedData, 16); // First 16 bytes are the IV
    Serial.println("Extracted IV:");
    for (int i = 0; i < 16; i++) {
        Serial.printf("%02x ", iv[i]);
    }
    Serial.println();

    // Initialize AES decryption with the key
    mbedtls_aes_setkey_dec(&aes, key, 128); // 128-bit key
    int blockSize = 16;
    int decryptedLength = 0;

    // Decrypt blocks of data starting after the IV
    for (int i = 16; i < dataLength; i += blockSize) {
        Serial.printf("Decrypting block %d...\n", (i - 16) / blockSize + 1);
        mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, blockSize, iv, encryptedData + i, decryptedData + decryptedLength);
        decryptedLength += blockSize;
    }

    // Print raw decrypted data
    Serial.println("Decrypted Data (before unpadding):");
    for (int i = 0; i < decryptedLength; i++) {
        Serial.printf("%02x ", decryptedData[i]);
    }
    Serial.println();

    // Remove padding: last byte indicates padding length
    int padding = decryptedData[decryptedLength - 1];
    if (padding < 1 || padding > blockSize) {
        Serial.println("Invalid padding detected!");
        mbedtls_aes_free(&aes);
        return 0; // Return zero-length if padding is invalid
    }
    decryptedLength -= padding;

    mbedtls_aes_free(&aes);
    return decryptedLength;
}

void loop() {
    // Nothing to do here
}

/*#include <WiFi.h>

const char* ssid = "TOPNET_4948";
const char* password = "4hvfa5m5de";
WiFiServer server(12346);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();  // Start the server
}

void loop() {
  WiFiClient client = server.available();  // Listen for incoming clients
  if (client) {
    Serial.println("Client connected");

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);  // Print received data
      }
    }

    client.stop();  // Stop the client
    Serial.println("Client disconnected");
  }
}
*/

