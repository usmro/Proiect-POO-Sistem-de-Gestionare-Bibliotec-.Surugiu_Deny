import sqlite3

def main():
    conn = sqlite3.connect('biblioteca.db')
    c = conn.cursor()
    
    # 1 Ingrijitor
    ingrijitor = ("ING001", "INGRIJITOR", "admin", "Stanescu", "Vasile", "1750505050505", "vasile.stanescu@biblio.ro", "0755111222", "Str. Curateniei 4", "Toate salile", 3500.0, "10/10/2020", "06:00-14:00")
    
    id, tip, parola, nume, prenume, cnp, email, telefon, adresa, departament, salariu, data_angajare, orar = ingrijitor
    
    c.execute("DELETE FROM Utilizatori WHERE id = ?", (id,))
    
    c.execute("""
        INSERT INTO Utilizatori (
            id, tip, parola, nume, prenume, cnp, email, telefon, adresa, 
            departament, salariu, data_angajare, orar, is_deleted
        ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    """, (id, tip, parola, nume, prenume, cnp, email, telefon, adresa, departament, salariu, data_angajare, orar, 0))
        
    conn.commit()
    conn.close()
    print("Ingrijitor adaugat cu succes in SQLite!")

if __name__ == '__main__':
    main()
