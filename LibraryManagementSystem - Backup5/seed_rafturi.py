import sqlite3

def main():
    conn = sqlite3.connect('biblioteca.db')
    c = conn.cursor()
    
    # Get all distinct locations from physical books
    c.execute('''
        SELECT DISTINCT cladire, camera, culoar, raft 
        FROM Carti 
        WHERE tip_format = 0 
          AND cladire != '' 
          AND camera != ''
    ''')
    
    locations = c.fetchall()
    conn.close()
    
    with open('db_rafturi.txt', 'w', encoding='utf-8') as f:
        for loc in locations:
            cladire, camera, culoar, raft = loc
            # id = cladire-camera-culoar-raft
            rid = f"{cladire}-{camera}-{culoar}-{raft}"
            capacitate = 100
            
            f.write(f"{rid}|{cladire}|{camera}|{culoar}|{raft}|{capacitate}\n")
            
    print(f"Am regenerat {len(locations)} rafturi pe baza locatiilor existente din carti.")

if __name__ == '__main__':
    main()
