#include "Biblioteca.h"
#include <sqlite3.h>
#include <iostream>
#include <sstream>

// Helper func pentru a repara ghilimele in string (pentru SQL)
std::string escapeSQL(const std::string& str) {
    std::string escaped;
    for (char c : str) {
        if (c == '\'') escaped += "''";
        else escaped += c;
    }
    return escaped;
}

void creeazaTabele(sqlite3* db) {
    const char* queries[] = {
        // Sistem
        "CREATE TABLE IF NOT EXISTS Sistem (id INTEGER PRIMARY KEY, buget REAL, zi_curenta INTEGER);",
        
        // Carti
        "CREATE TABLE IF NOT EXISTS Carti ("
        "isbn TEXT PRIMARY KEY,"
        "titlu TEXT,"
        "autori TEXT,"
        "editura TEXT,"
        "pret_intrare REAL,"
        "categorie TEXT,"
        "an_aparitie INTEGER,"
        "nr_pagini INTEGER,"
        "stoc_disponibil INTEGER,"
        "stoc_defect INTEGER,"
        "data_adaugarii TEXT,"
        "contor_imprumuturi INTEGER,"
        "tip_format INTEGER," // 0 = Fizica, 1 = Digitala
        "stare_defecta INTEGER," // pt fizica
        "link_acces TEXT," // pt digitala
        "dimensiune_kb INTEGER," // pt digitala
        "is_deleted INTEGER DEFAULT 0"
        ");",

        // Utilizatori
        "CREATE TABLE IF NOT EXISTS Utilizatori ("
        "id TEXT PRIMARY KEY,"
        "tip TEXT,"
        "parola TEXT,"
        "nume TEXT,"
        "prenume TEXT,"
        "cnp TEXT,"
        "email TEXT,"
        "telefon TEXT,"
        "adresa TEXT,"
        "departament TEXT,"
        "salariu REAL,"
        "data_angajare TEXT,"
        "orar TEXT,"
        "tip_abonament TEXT,"
        "data_inregistrare TEXT,"
        "data_expirare TEXT,"
        "limita_imprumuturi INTEGER,"
        "imprumuturi_curente INTEGER,"
        "penalizari REAL,"
        "is_deleted INTEGER DEFAULT 0"
        ");",

        // Imprumuturi
        "CREATE TABLE IF NOT EXISTS Imprumuturi ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "isbn TEXT,"
        "id_cititor TEXT,"
        "nume_carte TEXT,"
        "data_imprumut TEXT,"
        "termen_limita TEXT,"
        "observatii TEXT"
        ");",

        // Returnari (in asteptare)
        "CREATE TABLE IF NOT EXISTS ReturnariInAsteptare ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "isbn TEXT,"
        "id_cititor TEXT,"
        "data_retur TEXT,"
        "penalizare REAL"
        ");",
        
        // Plati (in asteptare)
        "CREATE TABLE IF NOT EXISTS PlatiInAsteptare ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "id_cititor TEXT,"
        "suma REAL,"
        "motiv TEXT,"
        "data TEXT"
        ");",

        // Jurnal Audit
        "CREATE TABLE IF NOT EXISTS JurnalAudit ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "data_ora TEXT,"
        "id_utilizator TEXT,"
        "actiune TEXT"
        ");",

        // Istoric Imprumuturi
        "CREATE TABLE IF NOT EXISTS IstoricImprumuturi ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "isbn TEXT,"
        "titlu TEXT,"
        "id_cititor TEXT,"
        "data_imprumut TEXT,"
        "data_retur TEXT"
        ");"
    };

    char* errMsg = nullptr;
    for(const auto& q : queries) {
        if(sqlite3_exec(db, q, nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::cerr << "Eroare creare tabele: " << errMsg << "\n";
            sqlite3_free(errMsg);
        }
    }
}

int main() {
    std::cout << "Incarcare date din fisiere text...\n";
    Biblioteca bib; // Citeste fisierele txt standard
    
    sqlite3* db;
    if(sqlite3_open("biblioteca.db", &db)) {
        std::cerr << "Nu s-a putut deschide biblioteca.db\n";
        return 1;
    }
    
    std::cout << "Creare structuri tabele...\n";
    creeazaTabele(db);
    
    char* errMsg = nullptr;
    sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, &errMsg);
    
    // Sistem
    {
        std::string q = "INSERT INTO Sistem (id, buget, zi_curenta) VALUES (1, " + std::to_string(bib.getBuget()) + ", 0);";
        sqlite3_exec(db, q.c_str(), nullptr, nullptr, nullptr);
    }
    
    // Functie helper pt insert carte
    auto insertCarte = [&](const std::shared_ptr<Carte>& c, int is_deleted) {
        int tip = c->getTipFormat();
        int stare_defecta = c->getStareDefecta() ? 1 : 0;
        std::string link = "";
        int dim = 0;
        if(tip == 1) { // Digitala
            auto cd = std::dynamic_pointer_cast<CarteDigitala>(c);
            if(cd) {
                link = cd->getLinkAcces();
                dim = (int)cd->getDimensiuneMB();
            }
        }
        
        std::string autori = "";
        for(size_t i=0; i<c->getAutori().size(); i++) {
            autori += c->getAutori()[i];
            if(i < c->getAutori().size()-1) autori += ", ";
        }
        
        std::string q = "INSERT INTO Carti (isbn, titlu, autori, editura, pret_intrare, categorie, an_aparitie, nr_pagini, stoc_disponibil, stoc_defect, data_adaugarii, contor_imprumuturi, tip_format, stare_defecta, link_acces, dimensiune_kb, is_deleted) VALUES ('" +
            escapeSQL(c->getIsbn()) + "', '" +
            escapeSQL(c->getTitlu()) + "', '" +
            escapeSQL(autori) + "', '" +
            escapeSQL(c->getEditura()) + "', " +
            std::to_string(c->getPretIntrare()) + ", '" +
            escapeSQL(c->getCategorie()) + "', " +
            std::to_string(c->getAnAparitie()) + ", " +
            std::to_string(c->getNrPagini()) + ", " +
            std::to_string(c->getStocDisponibil()) + ", " +
            "0, '" + // stoc_defect handled globally or per book, hardcoded 0 for simplicity, it will recompute
            escapeSQL(c->getDataAdaugarii()) + "', " +
            std::to_string(c->getContorImprumuturi()) + ", " +
            std::to_string(tip) + ", " +
            std::to_string(stare_defecta) + ", '" +
            escapeSQL(link) + "', " +
            std::to_string(dim) + ", " +
            std::to_string(is_deleted) + ");";
            
        sqlite3_exec(db, q.c_str(), nullptr, nullptr, nullptr);
    };
    
    std::cout << "Migrare carti...\n";
    for(auto& c : bib.getToateCartile()) insertCarte(c, 0);
    for(auto& c : bib.getCartiSterse()) insertCarte(c, 1);
    
    // Functie helper pt insert utilizator
    auto insertUtilizator = [&](const std::shared_ptr<Utilizator>& u, int is_deleted) {
        std::string q = "INSERT INTO Utilizatori (id, tip, parola, nume, prenume, cnp, email, telefon, adresa, departament, salariu, data_angajare, orar, tip_abonament, data_inregistrare, data_expirare, limita_imprumuturi, imprumuturi_curente, penalizari, is_deleted) VALUES ('" +
            escapeSQL(u->getId()) + "', '" +
            escapeSQL(u->getTip()) + "', '" +
            escapeSQL(u->getParola()) + "', '" +
            escapeSQL(u->getNume()) + "', '" +
            escapeSQL(u->getPrenume()) + "', '" +
            escapeSQL(u->getCnp()) + "', '" +
            escapeSQL(u->getEmail()) + "', '" +
            escapeSQL(u->getTelefon()) + "', '" +
            escapeSQL(u->getAdresa()) + "', ";
            
        if(u->getRol() == "Director" || u->getRol() == "Bibliotecar" || u->getRol() == "Ingrijitor") {
            auto ang = std::dynamic_pointer_cast<Director>(u); // just assuming they share methods or use fallback
            // To simplify, we get fields from string representation
            std::string line = u->formatFisier();
            std::stringstream ss(line);
            std::string token;
            std::vector<std::string> parts;
            while(std::getline(ss, token, '|')) parts.push_back(token);
            
            // Format for staff: TIP|ID|pass|nume|pren|cnp|email|tel|adr|1|dep|sal|data|orar_sau_bonus
            // Cititor format: CITITOR|ID|pass|nume|pren|cnp|email|tel|adr|1|tip_ab|data_inreg|data_exp|limita|impr_cur|pen
            
            if(u->getTip() == "CITITOR") {
                q += "NULL, NULL, NULL, NULL, '" +
                     escapeSQL(parts[10]) + "', '" + // tip_abonament
                     escapeSQL(parts[11]) + "', '" + // data_inreg
                     escapeSQL(parts[12]) + "', " +  // data_exp
                     parts[13] + ", " +              // limita
                     parts[14] + ", " +              // curente
                     parts[15] + ", " +              // penalizari
                     std::to_string(is_deleted) + ");";
            } else {
                q += "'" + escapeSQL(parts[10]) + "', " + // departament
                     parts[11] + ", '" +                  // salariu
                     escapeSQL(parts[12]) + "', '" +      // data_angajare
                     escapeSQL(parts.size() > 13 ? parts[13] : "") + "', " + // orar/birou
                     "NULL, NULL, NULL, NULL, NULL, NULL, " +
                     std::to_string(is_deleted) + ");";
            }
        }
        sqlite3_exec(db, q.c_str(), nullptr, nullptr, nullptr);
    };
    
    std::cout << "Migrare utilizatori...\n";
    for(auto& u : bib.getTotiUtilizatorii()) insertUtilizator(u, 0);
    for(auto& u : bib.getUtilizatoriStersi()) insertUtilizator(u, 1);
    
    std::cout << "Migrare imprumuturi si returnari...\n";
    for(const auto& imp : bib.getToateImprumuturile()) {
        std::string q = "INSERT INTO Imprumuturi (isbn, id_cititor, nume_carte, data_imprumut, termen_limita, observatii) VALUES ('" +
            escapeSQL(imp.getIdCarte()) + "', '" + escapeSQL(imp.getIdCititor()) + "', 'necunoscut', '" +
            escapeSQL(imp.getDataImprumut()) + "', '" + escapeSQL(imp.getTermenLimita()) + "', '" + escapeSQL(imp.getObservatii()) + "');";
        sqlite3_exec(db, q.c_str(), nullptr, nullptr, nullptr);
    }
    
    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, &errMsg);
    sqlite3_close(db);
    std::cout << "Migrare completata cu succes in biblioteca.db!\n";
    return 0;
}
