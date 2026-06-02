import re

with open("Biblioteca.h", "r") as f:
    text = f.read()

text = text.replace("sqlite3* db;\n    std::string fisier_db;", """sqlite3* db;
    std::string fisier_db;
    
    std::string fisier_carti;
    std::string fisier_carti_sterse;
    std::string fisier_imprumuturi;
    std::string fisier_utilizatori;
    std::string fisier_utilizatori_stersi;
    std::string fisier_returnari;
    std::string fisier_plati;
    std::string fisier_defecte;
    std::string fisier_buget;
    std::string fisier_timp;
    std::string fisier_rezervari;
    std::string fisier_istoric;
    std::string fisier_recenzii;
    std::string fisier_sali;
    std::string fisier_notificari;
    std::string fisier_log;
    std::string fisier_rafturi;
""")

text = text.replace('Biblioteca(const std::string& fisier_db = "biblioteca.db");', """Biblioteca(const std::string& fisier_db = "biblioteca.db",
               const std::string& fisier_carti = "db_carti.txt",
               const std::string& fisier_imprumuturi = "db_imprumuturi.txt",
               const std::string& fisier_utilizatori = "db_utilizatori.txt",
               const std::string& fisier_carti_sterse = "db_carti_sterse.txt",
               const std::string& fisier_utilizatori_stersi = "db_utilizatori_stersi.txt",
               const std::string& fisier_returnari = "db_returnari.txt",
               const std::string& fisier_plati = "db_plati.txt",
               const std::string& fisier_defecte = "db_defecte.txt",
               const std::string& fisier_buget = "db_buget.txt",
               const std::string& fisier_timp = "db_timp.txt",
               const std::string& fisier_rezervari = "db_rezervari.txt",
               const std::string& fisier_istoric = "db_istoric.txt",
               const std::string& fisier_recenzii = "db_recenzii.txt",
               const std::string& fisier_sali = "db_sali.txt",
               const std::string& fisier_notificari = "db_notificari.txt",
               const std::string& fisier_log = "db_log.txt",
               const std::string& fisier_rafturi = "db_rafturi.txt");""")

with open("Biblioteca.h", "w") as f:
    f.write(text)
