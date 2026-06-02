import sqlite3

def xor_encode(text):
    key = 'BibliotecaSecreta2025'
    res = bytearray()
    for i in range(len(text)):
        res.append(ord(text[i]) ^ ord(key[i % len(key)]))
    return res.hex()

conn = sqlite3.connect('biblioteca.db')
cursor = conn.cursor()

admin_hash = xor_encode('admin')
pwd_1234_hash = xor_encode('1234')

print(f'admin hash: {admin_hash}')
print(f'1234 hash: {pwd_1234_hash}')

# Fix all that got incorrect hashes
# We know CIT001, CIT003, CIT002 were 1234.
# We manually set them to 737b7a68 which was wrong.
cursor.execute("UPDATE Utilizatori SET parola = ? WHERE parola = '737b7a68'", (pwd_1234_hash,))

# Actually, let's just make sure admin is correct too
# If admin was encoded correctly earlier (230d0f0507), let's check
cursor.execute("UPDATE Utilizatori SET parola = ? WHERE parola = '230d0f0507'", (admin_hash,))

conn.commit()
conn.close()
