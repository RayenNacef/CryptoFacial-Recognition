import socket
from Crypto.Cipher import AES
import os

# Configuration
ESP_IP = "192.168.1.15"  
ESP_PORT = 12346  
key = b"1234567890123456"  # 16-byte key for AES
iv = os.urandom(16)  # Initialization vector for AES-CBC

def encrypt_message(message, key, iv):
    cipher = AES.new(key, AES.MODE_CBC, iv)
    # Pad message to be a multiple of 16 bytes
    padding = 16 - len(message) % 16
    padded_message = message + bytes([padding]) * padding
    encrypted_message = cipher.encrypt(padded_message)
    return iv + encrypted_message  # Send IV along with the encrypted data

print(f"Connecting to {ESP_IP}:{ESP_PORT}")

# Create socket connection
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((ESP_IP, ESP_PORT))

try:
    message = b"CRISTIANO RONALDO,12:55:37"  # plaintext message
    encrypted_data = encrypt_message(message, key, iv)
    sock.sendall(encrypted_data)
    print("Encrypted data sent.")
finally:
    sock.close()

# Print the encrypted data for reference
print("Encrypted Data: ", encrypted_data.hex())
