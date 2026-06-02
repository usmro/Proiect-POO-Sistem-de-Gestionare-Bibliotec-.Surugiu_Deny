import re

with open("Biblioteca.cpp", "r") as f:
    text = f.read()

# Fix constructor
old_ctor = """Biblioteca::Biblioteca(const std::string& fisier_db) : fisier_db(fisier_db), buget(0.0), offset_timp(0) {
    if(sqlite3_open(fisier_db.c_str(), &db)) {
        std::cerr << "Eroare la deschiderea bazei de date SQL: " << sqlite3_errmsg(db) << "\\n";
    }
    incarcaDateDinSQL();
    incarcaBuget();
    incarcaTimp();
}"""

new_ctor = """Biblioteca::Biblioteca(const std::string& fisier_db,
               const std::string& fisier_carti,
               const std::string& fisier_imprumuturi,
               const std::string& fisier_utilizatori,
               const std::string& fisier_carti_sterse,
               const std::string& fisier_utilizatori_stersi,
               const std::string& fisier_returnari,
               const std::string& fisier_plati,
               const std::string& fisier_defecte,
               const std::string& fisier_buget,
               const std::string& fisier_timp,
               const std::string& fisier_rezervari,
               const std::string& fisier_istoric,
               const std::string& fisier_recenzii,
               const std::string& fisier_sali,
               const std::string& fisier_notificari,
               const std::string& fisier_log,
               const std::string& fisier_rafturi) : fisier_db(fisier_db), fisier_carti(fisier_carti), fisier_imprumuturi(fisier_imprumuturi),
      fisier_utilizatori(fisier_utilizatori), fisier_carti_sterse(fisier_carti_sterse), fisier_utilizatori_stersi(fisier_utilizatori_stersi), fisier_returnari(fisier_returnari),
      fisier_plati(fisier_plati), fisier_defecte(fisier_defecte), fisier_buget(fisier_buget),
      fisier_timp(fisier_timp), fisier_rafturi(fisier_rafturi), fisier_rezervari(fisier_rezervari),
      fisier_istoric(fisier_istoric), fisier_recenzii(fisier_recenzii),
      fisier_sali(fisier_sali), fisier_notificari(fisier_notificari),
      fisier_log(fisier_log), buget(0.0), offset_timp(0) {
      
    if(sqlite3_open(fisier_db.c_str(), &db)) {
        std::cerr << "Eroare la deschiderea bazei de date SQL: " << sqlite3_errmsg(db) << "\\n";
    }
    incarcaDate(); // Incarca ce a ramas in .txt (rafturi, etc)
    incarcaDateDinSQL(); // Suprascrie/incarca tabelele mari din SQLite
    incarcaBuget();
    incarcaTimp();
}"""

text = text.replace(old_ctor, new_ctor)

# We need to implement incarcaDateDinSQL
# Let's insert it before incarcaDate()

incarca_sql = """void Biblioteca::incarcaDateDinSQL() {
    sqlite3_stmt* stmt;
    // Carti
    if(sqlite3_prepare_v2(db, "SELECT * FROM Carti;", -1, &stmt, nullptr) == SQLITE_OK) {
        while(sqlite3_step(stmt) == SQLITE_ROW) {
            std::string isbn = (const char*)sqlite3_column_text(stmt, 0);
            std::string titlu = (const char*)sqlite3_column_text(stmt, 1);
            std::string autori_str = (const char*)sqlite3_column_text(stmt, 2);
            std::string editura = (const char*)sqlite3_column_text(stmt, 3);
            double pret = sqlite3_column_double(stmt, 4);
            std::string cat = (const char*)sqlite3_column_text(stmt, 5);
            int an = sqlite3_column_int(stmt, 6);
            int pag = sqlite3_column_int(stmt, 7);
            int stoc = sqlite3_column_int(stmt, 8);
            int stoc_def = sqlite3_column_int(stmt, 9);
            std::string data_adg = (const char*)sqlite3_column_text(stmt, 10);
            int contor = sqlite3_column_int(stmt, 11);
            int tip = sqlite3_column_int(stmt, 12);
            int defecta = sqlite3_column_int(stmt, 13);
            std::string link = sqlite3_column_text(stmt, 14) ? (const char*)sqlite3_column_text(stmt, 14) : "";
            int dim = sqlite3_column_int(stmt, 15);
            int is_deleted = sqlite3_column_int(stmt, 16);
            
            std::shared_ptr<Carte> c;
            auto autori = Carte::stringToAutori(autori_str);
            if(tip == 0) {
                Locatie loc; // empty
                c = std::make_shared<CarteFizica>(isbn, titlu, autori, editura, pret, cat, an, pag, data_adg, contor, stoc, loc, "Standard", defecta == 1);
            } else {
                c = std::make_shared<CarteDigitala>(isbn, titlu, autori, editura, pret, cat, an, pag, data_adg, contor, "PDF", dim, link);
            }
            if(is_deleted) carti_sterse.push_back(c);
            else carti.push_back(c);
        }
        sqlite3_finalize(stmt);
    }
    
    // Utilizatori
    if(sqlite3_prepare_v2(db, "SELECT * FROM Utilizatori;", -1, &stmt, nullptr) == SQLITE_OK) {
        while(sqlite3_step(stmt) == SQLITE_ROW) {
            std::string id = (const char*)sqlite3_column_text(stmt, 0);
            std::string tip = (const char*)sqlite3_column_text(stmt, 1);
            std::string parola = (const char*)sqlite3_column_text(stmt, 2);
            std::string nume = (const char*)sqlite3_column_text(stmt, 3);
            std::string prenume = (const char*)sqlite3_column_text(stmt, 4);
            std::string cnp = (const char*)sqlite3_column_text(stmt, 5);
            std::string email = (const char*)sqlite3_column_text(stmt, 6);
            std::string telefon = (const char*)sqlite3_column_text(stmt, 7);
            std::string adresa = (const char*)sqlite3_column_text(stmt, 8);
            
            int is_deleted = sqlite3_column_int(stmt, 19);
            std::shared_ptr<Utilizator> u;
            
            if(tip == "CITITOR") {
                std::string tip_abonament = sqlite3_column_text(stmt, 13) ? (const char*)sqlite3_column_text(stmt, 13) : "Standard";
                std::string data_inregistrare = sqlite3_column_text(stmt, 14) ? (const char*)sqlite3_column_text(stmt, 14) : "";
                std::string data_expirare = sqlite3_column_text(stmt, 15) ? (const char*)sqlite3_column_text(stmt, 15) : "";
                int limita = sqlite3_column_int(stmt, 16);
                int curent = sqlite3_column_int(stmt, 17);
                double pen = sqlite3_column_double(stmt, 18);
                u = std::make_shared<Cititor>(id, parola, nume, prenume, cnp, email, telefon, adresa, true, tip_abonament, data_inregistrare, data_expirare, limita, curent, pen);
            } else {
                std::string dep = sqlite3_column_text(stmt, 9) ? (const char*)sqlite3_column_text(stmt, 9) : "";
                double sal = sqlite3_column_double(stmt, 10);
                std::string data_ang = sqlite3_column_text(stmt, 11) ? (const char*)sqlite3_column_text(stmt, 11) : "";
                std::string orar = sqlite3_column_text(stmt, 12) ? (const char*)sqlite3_column_text(stmt, 12) : "";
                
                if(tip == "DIRECTOR") u = std::make_shared<Director>(id, parola, nume, prenume, cnp, email, telefon, adresa, true, dep, sal, data_ang, orar, 0);
                else if(tip == "BIBLIOTECAR") u = std::make_shared<Bibliotecar>(id, parola, nume, prenume, cnp, email, telefon, adresa, true, dep, sal, data_ang, orar, 0);
                else u = std::make_shared<Ingrijitor>(id, parola, nume, prenume, cnp, email, telefon, adresa, true, dep, sal, data_ang, orar, "");
            }
            if(is_deleted) utilizatori_stersi.push_back(u);
            else utilizatori.push_back(u);
        }
        sqlite3_finalize(stmt);
    }
    
    // Imprumuturi
    if(sqlite3_prepare_v2(db, "SELECT * FROM Imprumuturi;", -1, &stmt, nullptr) == SQLITE_OK) {
        while(sqlite3_step(stmt) == SQLITE_ROW) {
            std::string isbn = (const char*)sqlite3_column_text(stmt, 1);
            std::string id_cititor = (const char*)sqlite3_column_text(stmt, 2);
            std::string nume_carte = (const char*)sqlite3_column_text(stmt, 3);
            std::string data_imp = (const char*)sqlite3_column_text(stmt, 4);
            std::string termen = (const char*)sqlite3_column_text(stmt, 5);
            std::string obs = sqlite3_column_text(stmt, 6) ? (const char*)sqlite3_column_text(stmt, 6) : "";
            
            imprumuturi.push_back(Imprumut(isbn, id_cititor, nume_carte, data_imp, termen, obs));
        }
        sqlite3_finalize(stmt);
    }
}

"""

text = text.replace("void Biblioteca::incarcaDate() {", incarca_sql + "void Biblioteca::incarcaDate() {")

# Remove Carti, Utilizatori, Imprumuturi from incarcaDate
text = re.sub(r'std::ifstream fin_carti\(fisier_carti\);.*?fin_carti\.close\(\);\n    }', '', text, flags=re.DOTALL)
text = re.sub(r'std::ifstream fin_carti_sterse\(fisier_carti_sterse\);.*?fin_carti_sterse\.close\(\);\n    }', '', text, flags=re.DOTALL)
text = re.sub(r'std::ifstream fin_util\(fisier_utilizatori\);.*?fin_util\.close\(\);\n    }', '', text, flags=re.DOTALL)
text = re.sub(r'std::ifstream fin_util_stersi\(fisier_utilizatori_stersi\);.*?fin_util_stersi\.close\(\);\n    }', '', text, flags=re.DOTALL)
text = re.sub(r'std::ifstream fin_imp\(fisier_imprumuturi\);.*?fin_imp\.close\(\);\n    }', '', text, flags=re.DOTALL)

# In salveazaDate, we want to clear the SQL tables and insert EVERYTHING
salveaza_sql = """void Biblioteca::salveazaDate() const {
    char* errMsg = nullptr;
    sqlite3_exec(db, "BEGIN TRANSACTION; DELETE FROM Carti; DELETE FROM Utilizatori; DELETE FROM Imprumuturi;", nullptr, nullptr, &errMsg);
    
    // Insert Carti
    for(const auto& c : carti) {
        std::string tip = std::to_string(c->getTipFormat());
        std::string stare = c->getStareDefecta() ? "1" : "0";
        std::string link = "";
        int dim = 0;
        if(c->getTipFormat() == 1) {
            auto cd = std::dynamic_pointer_cast<CarteDigitala>(c);
            if(cd) { link = cd->getLinkAcces(); dim = (int)cd->getDimensiuneMB(); }
        }
        std::string q = "INSERT INTO Carti (isbn, titlu, autori, editura, pret_intrare, categorie, an_aparitie, nr_pagini, stoc_disponibil, stoc_defect, data_adaugarii, contor_imprumuturi, tip_format, stare_defecta, link_acces, dimensiune_kb, is_deleted) VALUES ('" +
            c->getIsbn() + "', '" + c->getTitlu() + "', 'Autori', '" + c->getEditura() + "', " + std::to_string(c->getPretIntrare()) + ", '" +
            c->getCategorie() + "', " + std::to_string(c->getAnAparitie()) + ", " + std::to_string(c->getNrPagini()) + ", " +
            std::to_string(c->getStocDisponibil()) + ", 0, '" + c->getDataAdaugarii() + "', " + std::to_string(c->getContorImprumuturi()) + ", " + tip + ", " + stare + ", '" + link + "', " + std::to_string(dim) + ", 0);";
        sqlite3_exec(db, q.c_str(), nullptr, nullptr, nullptr);
    }
    
    // (Carti Sterse)
    for(const auto& c : carti_sterse) {
        std::string tip = std::to_string(c->getTipFormat());
        std::string stare = c->getStareDefecta() ? "1" : "0";
        std::string q = "INSERT INTO Carti (isbn, titlu, autori, editura, pret_intrare, categorie, an_aparitie, nr_pagini, stoc_disponibil, stoc_defect, data_adaugarii, contor_imprumuturi, tip_format, stare_defecta, link_acces, dimensiune_kb, is_deleted) VALUES ('" +
            c->getIsbn() + "', '" + c->getTitlu() + "', 'Autori', '" + c->getEditura() + "', " + std::to_string(c->getPretIntrare()) + ", '" +
            c->getCategorie() + "', " + std::to_string(c->getAnAparitie()) + ", " + std::to_string(c->getNrPagini()) + ", " +
            std::to_string(c->getStocDisponibil()) + ", 0, '" + c->getDataAdaugarii() + "', " + std::to_string(c->getContorImprumuturi()) + ", " + tip + ", " + stare + ", '', 0, 1);";
        sqlite3_exec(db, q.c_str(), nullptr, nullptr, nullptr);
    }
    
    // Imprumuturi
    for(const auto& imp : imprumuturi) {
        std::string q = "INSERT INTO Imprumuturi (isbn, id_cititor, nume_carte, data_imprumut, termen_limita, observatii) VALUES ('" +
            imp.getIdCarte() + "', '" + imp.getIdCititor() + "', 'nume_cititor', '" + imp.getDataImprumut() + "', '" + imp.getTermenLimita() + "', '" + imp.getObservatii() + "');";
        sqlite3_exec(db, q.c_str(), nullptr, nullptr, nullptr);
    }
    
    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);
"""

text = text.replace("void Biblioteca::salveazaDate() const {", salveaza_sql)

# Remove the txt savings for carti, utilizatori, imprumuturi inside salveazaDate
# I will use a simple regex for each block.
text = re.sub(r'std::ofstream fout_carti\(fisier_carti\);.*?fout_carti\.close\(\);\n    }', '', text, flags=re.DOTALL)
text = re.sub(r'std::ofstream fout_carti_sterse\(fisier_carti_sterse\);.*?fout_carti_sterse\.close\(\);\n    }', '', text, flags=re.DOTALL)
text = re.sub(r'std::ofstream fout_util\(fisier_utilizatori\);.*?fout_util\.close\(\);\n    }', '', text, flags=re.DOTALL)
text = re.sub(r'std::ofstream fout_util_stersi\(fisier_utilizatori_stersi\);.*?fout_util_stersi\.close\(\);\n    }', '', text, flags=re.DOTALL)
text = re.sub(r'std::ofstream fout_imp\(fisier_imprumuturi\);.*?fout_imp\.close\(\);\n    }', '', text, flags=re.DOTALL)

with open("Biblioteca.cpp", "w") as f:
    f.write(text)
