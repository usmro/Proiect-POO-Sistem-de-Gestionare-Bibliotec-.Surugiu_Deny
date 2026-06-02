import sqlite3
import random
from datetime import datetime, timedelta

def main():
    conn = sqlite3.connect('biblioteca.db')
    c = conn.cursor()
    
    # 3 Cititori distincti
    cititori = [
        ("CIT001", "CITITOR", "1234", "Popescu", "Andrei", "1900101010101", "andrei.popescu@email.com", "0740111222", "Str. Florilor 1", "Standard", 3, 0, 0.0),
        ("CIT002", "CITITOR", "1234", "Ionescu", "Elena", "2910202020202", "elena.ionescu@email.com", "0740222333", "Aleea Rozelor 5", "Premium", 5, 0, 0.0),
        ("CIT003", "CITITOR", "1234", "Gheorghe", "Marian", "1850303030303", "marian.gheorghe@email.com", "0740333444", "Bd. Unirii 10", "Standard", 3, 0, 0.0)
    ]
    
    today = datetime.now().strftime("%d/%m/%Y")
    exp = (datetime.now() + timedelta(days=365)).strftime("%d/%m/%Y")
    
    for cit in cititori:
        id, tip, parola, nume, prenume, cnp, email, telefon, adresa, tip_abonament, max_carti, impr, pen = cit
        
        # Stergem daca exista deja sa evitam Unique Constraint
        c.execute("DELETE FROM Utilizatori WHERE id = ?", (id,))
        
        c.execute("""
            INSERT INTO Utilizatori (
                id, tip, parola, nume, prenume, cnp, email, telefon, adresa, 
                tip_abonament, data_inregistrare, data_expirare, limita_imprumuturi, 
                imprumuturi_curente, penalizari, is_deleted
            ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        """, (id, tip, parola, nume, prenume, cnp, email, telefon, adresa, tip_abonament, today, exp, max_carti, impr, pen, 0))
        
    conn.commit()
    conn.close()
    print("Cititori adaugati cu succes in SQLite!")

if __name__ == '__main__':
    main()
