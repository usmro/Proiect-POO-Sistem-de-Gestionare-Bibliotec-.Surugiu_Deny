import sqlite3

conn = sqlite3.connect('biblioteca.db')
cursor = conn.cursor()
cursor.execute("UPDATE Utilizatori SET parola = '737b7a68' WHERE parola = '1234'")
conn.commit()
conn.close()
