import sqlite3

def format_price(pret):
    # If the price is an integer (e.g. 32.0), format it as "32". Otherwise "32.5"
    if pret.is_integer():
        return str(int(pret))
    return str(pret)

def main():
    conn = sqlite3.connect('biblioteca.db')
    c = conn.cursor()
    
    c.execute("SELECT * FROM Carti")
    books = c.fetchall()
    
    columns = [desc[0] for desc in c.description]
    col_idx = {col: i for i, col in enumerate(columns)}
    
    with open('db_carti.txt', 'w', encoding='utf-8') as f:
        for row in books:
            tip = row[col_idx['tip_format']]
            isbn = row[col_idx['isbn']]
            titlu = row[col_idx['titlu']]
            autori = row[col_idx['autori']]
            pret = format_price(row[col_idx['pret_intrare']])
            categorie = row[col_idx['categorie']]
            an = row[col_idx['an_aparitie']]
            pagini = row[col_idx['nr_pagini']]
            data = row[col_idx['data_adaugarii']]
            contor = row[col_idx['contor_imprumuturi']]
            editura = row[col_idx['editura']]
            
            if tip == 0:
                # Fizica
                stoc = row[col_idx['stoc_disponibil']]
                cladire = row[col_idx['cladire']]
                camera = row[col_idx['camera']]
                culoar = row[col_idx['culoar']]
                raft = row[col_idx['raft']]
                stare = row[col_idx['stare_defecta']]
                # Coperta is hardcoded as "Hardcover" or "Standard" in previous examples, let's use "Hardcover"
                f.write(f"0|{isbn}|{titlu}|{autori}|{pret}|{categorie}|{an}|{pagini}|{data}|{contor}|{stoc}|{cladire}|{camera}|{culoar}|{raft}|Hardcover|{stare}|{editura}\n")
            
            elif tip == 1:
                # Digitala
                dim = row[col_idx['dimensiune_kb']]
                link = row[col_idx['link_acces']]
                # Format
                f.write(f"1|{isbn}|{titlu}|{autori}|{pret}|{categorie}|{an}|{pagini}|{data}|{contor}|PDF|{dim}|{link}|{editura}\n")
                
            elif tip == 2:
                # Audio
                durata = row[col_idx['durata_minute']]
                narator = row[col_idx['narator']]
                link = row[col_idx['link_acces']]
                f.write(f"2|{isbn}|{titlu}|{autori}|{pret}|{categorie}|{an}|{pagini}|{data}|{contor}|{durata}|{narator}|{link}|{editura}\n")

    conn.close()
    print("db_carti.txt synced.")

if __name__ == '__main__':
    main()
