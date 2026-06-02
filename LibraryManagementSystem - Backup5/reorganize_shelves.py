import sqlite3
import random

def get_location_for_category(category):
    # Determine Building and Room based on keywords or just map deterministically
    mapping = {
        "Clasici Românești": ("Centrala", "Sala Românească", "Culoar A"),
        "Clasici Internaționali": ("Centrala", "Sala Universală", "Culoar B"),
        "Science Fiction & Fantasy": ("Filiala N", "Sala SF", "Culoar A"),
        "Thriller, Mister & Horror": ("Filiala N", "Sala Mister", "Culoar B"),
        "Non-ficțiune, Filosofie & Psihologie": ("Centrala", "Sala Lectură", "Culoar C"),
        "Ficțiune Contemporană & Modernă": ("Filiala S", "Sala Modernă", "Culoar A"),
        "Aventură & Romane Istorice": ("Filiala S", "Sala Istorică", "Culoar B"),
        "Dramaturgie, Poezie & Epopei Antice": ("Centrala", "Sala Arte", "Culoar D"),
        "Bestselleruri & Literatură Contemporană": ("Filiala S", "Sala Bestseller", "Culoar C"),
        "Distopii & Universuri Fantastice (Partea a II-a)": ("Filiala N", "Sala SF", "Culoar C"),
        "Dezvoltare Personală, Business & Biografii": ("Centrala", "Sala Business", "Culoar E")
    }
    
    if category in mapping:
        return mapping[category]
    return ("Centrala", "Sala Diverse", "Culoar Z")

def main():
    conn = sqlite3.connect('biblioteca.db')
    c = conn.cursor()
    
    # We only care about physical books (tip_format = 0)
    c.execute("SELECT isbn, categorie FROM Carti WHERE tip_format = 0")
    books = c.fetchall()
    
    # To avoid putting 100 books on "Raft 1", we'll keep track of how many books per category
    cat_counts = {}
    
    for isbn, categorie in books:
        if categorie not in cat_counts:
            cat_counts[categorie] = 0
        
        cladire, camera, culoar = get_location_for_category(categorie)
        
        # Every 10 books go to a new shelf
        raft_num = (cat_counts[categorie] // 10) + 1
        raft = f"Raft {raft_num}"
        
        c.execute('''
            UPDATE Carti 
            SET cladire = ?, camera = ?, culoar = ?, raft = ?
            WHERE isbn = ?
        ''', (cladire, camera, culoar, raft, isbn))
        
        cat_counts[categorie] += 1
        
    conn.commit()
    print(f"Updated locations for {len(books)} physical books based on their categories.")
    conn.close()

if __name__ == '__main__':
    main()
