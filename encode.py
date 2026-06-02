import sqlite3

def xor_encode(text):
    key = 'BibliotecaSecreta2025'
    res = bytearray()
    for i in range(len(text)):
        res.append(ord(text[i]) ^ ord(key[i % len(key)]))
    return res.hex()

conn = sqlite3.connect('biblioteca.db')
cursor = conn.cursor()
cursor.execute('SELECT id, parola FROM Utilizatori')
rows = cursor.fetchall()
for r in rows:
    user_id = r[0]
    parola = r[1]
    
    try:
        bytes.fromhex(parola)
        if len(parola) % 2 == 0 and len(parola) > 0:
            print(f'Skipping {user_id}, already encoded')
            continue
    except:
        pass
    
    encoded = xor_encode(parola)
    print(f'Encoding {user_id} {parola} -> {encoded}')
    cursor.execute('UPDATE Utilizatori SET parola = ? WHERE id = ?', (encoded, user_id))

conn.commit()
conn.close()
