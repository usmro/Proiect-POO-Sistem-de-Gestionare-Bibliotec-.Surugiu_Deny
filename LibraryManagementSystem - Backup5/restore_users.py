import sqlite3

def restore_users():
    conn = sqlite3.connect('biblioteca.db')
    cursor = conn.cursor()
    
    with open('db_utilizatori.txt', 'r') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            parts = line.split('|')
            if len(parts) < 10:
                continue
                
            tip = parts[0]
            uid = parts[1]
            parola = parts[2]
            nume = parts[3]
            prenume = parts[4]
            cnp = parts[5]
            email = parts[6]
            telefon = parts[7]
            adresa = parts[8]
            # parts[9] is 'activ' flag usually '1'
            
            if tip == "CITITOR":
                tip_ab = parts[10]
                data_in = parts[11]
                data_ex = parts[12]
                limita = int(parts[13])
                curente = int(parts[14])
                pen = float(parts[15])
                
                cursor.execute('''
                    INSERT OR REPLACE INTO Utilizatori 
                    (id, tip, parola, nume, prenume, cnp, email, telefon, adresa, tip_abonament, data_inregistrare, data_expirare, limita_imprumuturi, imprumuturi_curente, penalizari, is_deleted)
                    VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
                ''', (uid, tip, parola, nume, prenume, cnp, email, telefon, adresa, tip_ab, data_in, data_ex, limita, curente, pen, 0))
            else:
                dep = parts[10]
                salariu = float(parts[11])
                data_ang = parts[12]
                orar = parts[13] if len(parts) > 13 else ""
                
                cursor.execute('''
                    INSERT OR REPLACE INTO Utilizatori 
                    (id, tip, parola, nume, prenume, cnp, email, telefon, adresa, departament, salariu, data_angajare, orar, is_deleted)
                    VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
                ''', (uid, tip, parola, nume, prenume, cnp, email, telefon, adresa, dep, salariu, data_ang, orar, 0))
                
    conn.commit()
    conn.close()
    print("Users restored successfully.")

if __name__ == '__main__':
    restore_users()
