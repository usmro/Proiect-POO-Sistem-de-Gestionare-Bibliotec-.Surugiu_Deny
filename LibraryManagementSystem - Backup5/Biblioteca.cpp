#include "Biblioteca.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <cstdio>

#include <sqlite3.h>

Biblioteca::Biblioteca(const std::string& fisier_db,
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
        std::cerr << "Eroare la deschiderea bazei de date SQL: " << sqlite3_errmsg(db) << "\n";
    }
    incarcaDate(); // Incarca ce a ramas in .txt (rafturi, etc)
    incarcaDateDinSQL(); // Suprascrie/incarca tabelele mari din SQLite
    incarcaBuget();
    incarcaTimp();
}

Biblioteca::~Biblioteca() {
    salveazaDate();
    salveazaBuget();
    salveazaTimp();
    if(db) sqlite3_close(db);
}

std::vector<std::string> Biblioteca::splitLinie(const std::string& linie, char delim) const {
    std::vector<std::string> result;
    std::stringstream ss(linie);
    std::string token;
    while (std::getline(ss, token, delim)) {
        result.push_back(token);
    }
    return result;
}

std::shared_ptr<Carte> Biblioteca::parseazaLinieCarte(const std::string& linie) const {
    auto campuri = splitLinie(linie, '|');
    if (campuri.empty()) return nullptr;

    std::string tip = campuri[0];

    try {
        if (tip == "0" && campuri.size() >= 17) {
            // 0|ISBN|Titlu|Autori|Pret|Categorie|An|Pagini|DataAdaugarii|Contor|Stoc|Cladire|Camera|Culoar|Raft|Coperta|StareDefecta|[Editura]
            auto autori = Carte::stringToAutori(campuri[3]);
            double pret = std::stod(campuri[4]);
            int an = std::stoi(campuri[6]);
            int pag = std::stoi(campuri[7]);
            int contor = std::stoi(campuri[9]);
            int stoc = std::stoi(campuri[10]);
            Locatie loc;
            loc.cladire = campuri[11];
            loc.camera = campuri[12];
            loc.culoar = campuri[13];
            loc.raft = campuri[14];
            bool defecta = (campuri[16] == "1");
            std::string editura = "Necunoscută";
            if (campuri.size() >= 18) editura = campuri[17];

            return std::make_shared<CarteFizica>(
                campuri[1], campuri[2], autori, editura, pret, campuri[5], an, pag,
                campuri[8], contor, stoc, loc, campuri[15], defecta
            );
        }
        else if (tip == "1" && campuri.size() >= 13) {
            // 1|ISBN|Titlu|Autori|Pret|Categorie|An|Pagini|DataAdaugarii|Contor|Format|Dimensiune|Link|[Editura]
            auto autori = Carte::stringToAutori(campuri[3]);
            double pret = std::stod(campuri[4]);
            int an = std::stoi(campuri[6]);
            int pag = std::stoi(campuri[7]);
            int contor = std::stoi(campuri[9]);
            double dim = std::stod(campuri[11]);
            std::string editura = "Necunoscută";
            if (campuri.size() >= 14) editura = campuri[13];

            return std::make_shared<CarteDigitala>(
                campuri[1], campuri[2], autori, editura, pret, campuri[5], an, pag,
                campuri[8], contor, campuri[10], dim, campuri[12]
            );
        }
    } catch (...) {
        return nullptr;
    }
    return nullptr;
}


std::shared_ptr<Utilizator> Biblioteca::parseazaLinieUtilizator(const std::string& linie) const {
    auto c = splitLinie(linie, '|');
    if (c.empty()) return nullptr;

    std::string tip = c[0];

    // Format comun: TIP|id|parola|nume|prenume|cnp|email|telefon|adresa|activ|...specific...
    if (c.size() < 10) return nullptr;

    bool activ = (c[9] == "1");

    if (tip == "DIRECTOR" && c.size() >= 15) {
        auto u = std::make_shared<Director>(
            c[1], c[2], c[3], c[4], c[5], c[6], c[7], c[8], activ,
            c[10], std::stod(c[11]), c[12], c[13], std::stoi(c[14])
        );
        u->setParolaRaw(c[2]); // parola din fisier e deja stocata (posibil encoded)
        return u;
    }
    else if (tip == "BIBLIOTECAR" && c.size() >= 15) {
        auto u = std::make_shared<Bibliotecar>(
            c[1], c[2], c[3], c[4], c[5], c[6], c[7], c[8], activ,
            c[10], std::stod(c[11]), c[12], c[13], std::stoi(c[14])
        );
        u->setParolaRaw(c[2]);
        return u;
    }
    else if (tip == "INGRIJITOR" && c.size() >= 15) {
        auto u = std::make_shared<Ingrijitor>(
            c[1], c[2], c[3], c[4], c[5], c[6], c[7], c[8], activ,
            c[10], std::stod(c[11]), c[12], c[13], c[14]
        );
        u->setParolaRaw(c[2]);
        return u;
    }
    else if (tip == "CITITOR" && c.size() >= 16) {
        auto u = std::make_shared<Cititor>(
            c[1], c[2], c[3], c[4], c[5], c[6], c[7], c[8], activ,
            c[10], c[11], c[12], std::stoi(c[13]), std::stoi(c[14]), std::stod(c[15])
        );
        u->setParolaRaw(c[2]);
        return u;
    }

    return nullptr;
}

// ═══════════════════════════════════════════════
//  ÎNCĂRCARE / SALVARE DATE
// ═══════════════════════════════════════════════

void Biblioteca::incarcaDateDinSQL() {
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
            int durata_min_val = sqlite3_column_int(stmt, 17);
            std::string narator_val = sqlite3_column_text(stmt, 18) ? (const char*)sqlite3_column_text(stmt, 18) : "";
            std::string cladire = sqlite3_column_text(stmt, 19) ? (const char*)sqlite3_column_text(stmt, 19) : "";
            std::string camera = sqlite3_column_text(stmt, 20) ? (const char*)sqlite3_column_text(stmt, 20) : "";
            std::string culoar = sqlite3_column_text(stmt, 21) ? (const char*)sqlite3_column_text(stmt, 21) : "";
            std::string raft_val = sqlite3_column_text(stmt, 22) ? (const char*)sqlite3_column_text(stmt, 22) : "";
            
            std::shared_ptr<Carte> c;
            auto autori = Carte::stringToAutori(autori_str);
            if(tip == 0) {
                Locatie loc = {cladire, camera, culoar, raft_val};
                c = std::make_shared<CarteFizica>(isbn, titlu, autori, editura, pret, cat, an, pag, data_adg, contor, stoc, loc, "Standard", defecta == 1);
            } else if(tip == 2) {
                c = std::make_shared<CarteAudio>(isbn, titlu, autori, editura, pret, cat, an, pag, data_adg, contor, durata_min_val, narator_val, link);
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

void Biblioteca::incarcaDate() {
    std::ifstream fin_rafturi(fisier_rafturi);
    if(fin_rafturi.is_open()) {
        std::string linie;
        while(std::getline(fin_rafturi, linie)) {
            if(linie.empty()) continue;
            auto tokens = splitLinie(linie, '|');
            if(tokens.size() >= 6) {
                Raft r = { tokens[0], tokens[1], tokens[2], tokens[3], tokens[4], std::stoi(tokens[5]) };
                rafturi.push_back(r);
            }
        }
        fin_rafturi.close();
    }
    
    

    

    

    

    // Incarca returnari in asteptare
    

    std::ifstream fin_ret(fisier_returnari);
    if (fin_ret.is_open()) {
        std::string linie;
        while (std::getline(fin_ret, linie)) {
            if (linie.empty()) continue;
            auto campuri = splitLinie(linie, '|');
            if (campuri.size() >= 5) {
                ReturnareInAsteptare r;
                r.isbn           = campuri[0];
                r.id_cititor     = campuri[1];
                r.nume_cititor   = campuri[2];
                r.titlu_carte    = campuri[3];
                r.data_returnare = campuri[4];
                if (campuri.size() >= 6) r.amenda_intarziere = std::stod(campuri[5]);
                else r.amenda_intarziere = 0;
                returnari_in_asteptare.push_back(r);
            }
        }
        fin_ret.close();
    }

    std::ifstream fin_plati(fisier_plati);
    if (fin_plati.is_open()) {
        std::string linie;
        while (std::getline(fin_plati, linie)) {
            if (linie.empty()) continue;
            auto c = splitLinie(linie, '|');
            if (c.size() >= 4) {
                PlataInAsteptare p;
                p.id_cititor = c[0];
                p.nume_cititor = c[1];
                p.suma = std::stod(c[2]);
                p.data_solicitare = c[3];
                plati_in_asteptare.push_back(p);
            }
        }
        fin_plati.close();
    }

    std::ifstream fin_defecte(fisier_defecte);
    if (fin_defecte.is_open()) {
        std::string linie;
        while (std::getline(fin_defecte, linie)) {
            if (linie.empty()) continue;
            auto c = splitLinie(linie, '|');
            if (c.size() >= 2) {
                stoc_defect[c[0]] = std::stoi(c[1]);
            }
        }
        fin_defecte.close();
    }

    // Incarcă rezervări
    std::ifstream fin_rez(fisier_rezervari);
    if (fin_rez.is_open()) {
        std::string linie;
        while (std::getline(fin_rez, linie)) {
            if (linie.empty()) continue;
            auto c = splitLinie(linie, '|');
            if (c.size() >= 6) {
                Rezervare r;
                r.isbn = c[0]; r.id_cititor = c[1]; r.nume_cititor = c[2];
                r.data_rezervare = c[3]; r.data_expirare = c[4];
                r.alocata = (c[5] == "1");
                rezervari.push_back(r);
            }
        }
        fin_rez.close();
    }

    // Incarcă istoric lectură
    std::ifstream fin_ist(fisier_istoric);
    if (fin_ist.is_open()) {
        std::string linie;
        while (std::getline(fin_ist, linie)) {
            if (linie.empty()) continue;
            auto c = splitLinie(linie, '|');
            if (c.size() >= 5) {
                InregistrareIstoric i;
                i.isbn = c[0]; i.titlu_carte = c[1]; i.id_cititor = c[2];
                i.data_imprumut = c[3]; i.data_returnare = c[4];
                istoric_lectura.push_back(i);
            }
        }
        fin_ist.close();
    }

    // Incarcă recenzii
    std::ifstream fin_rec(fisier_recenzii);
    if (fin_rec.is_open()) {
        std::string linie;
        while (std::getline(fin_rec, linie)) {
            if (linie.empty()) continue;
            auto c = splitLinie(linie, '|');
            if (c.size() >= 6) {
                Recenzie rec;
                rec.isbn = c[0]; rec.id_cititor = c[1]; rec.nume_cititor = c[2];
                rec.nota = std::stoi(c[3]); rec.text = c[4]; rec.data = c[5];
                recenzii.push_back(rec);
            }
        }
        fin_rec.close();
    }

    // Incarcă rezervări săli
    std::ifstream fin_sali(fisier_sali);
    if (fin_sali.is_open()) {
        std::string linie;
        while (std::getline(fin_sali, linie)) {
            if (linie.empty()) continue;
            auto c = splitLinie(linie, '|');
            if (c.size() >= 5) {
                RezervareStudiu rs;
                rs.id_cititor = c[0]; rs.nume_cititor = c[1]; rs.sala = c[2];
                rs.data = c[3]; rs.interval_orar = c[4];
                rezervari_sali.push_back(rs);
            }
        }
        fin_sali.close();
    }

    // Incarcă notificări
    std::ifstream fin_not(fisier_notificari);
    if (fin_not.is_open()) {
        std::string linie;
        while (std::getline(fin_not, linie)) {
            if (linie.empty()) continue;
            auto c = splitLinie(linie, '|');
            if (c.size() >= 4) {
                Notificare n;
                n.id_destinatar = c[0]; n.mesaj = c[1]; n.data = c[2];
                n.citita = (c[3] == "1");
                notificari.push_back(n);
            }
        }
        fin_not.close();
    }
    
    if(rafturi.empty()) {
        for(const auto& c : carti) {
            if(c->getTipFormat() == 0) {
                auto cf = std::dynamic_pointer_cast<CarteFizica>(c);
                if(cf) {
                    Locatie loc = cf->getLocatieRaft();
                    if(!existaRaft(loc)) {
                        std::string id = loc.cladire + "-" + loc.camera + "-" + loc.culoar + "-" + loc.raft;
                        rafturi.push_back({id, loc.cladire, loc.camera, loc.culoar, loc.raft, 50});
                    }
                }
            }
        }
    }
    
    // Asigura-te ca in cazul in care a trecut timpul real de la ultima rulare, totul se actualizeaza
    genereazaNotificariExpirari();
    verificaRezervariExpirate();
}

void Biblioteca::salveazaDate() const {
    char* errMsg = nullptr;
    sqlite3_exec(db, "BEGIN TRANSACTION; DELETE FROM Carti; DELETE FROM Utilizatori; DELETE FROM Imprumuturi;", nullptr, nullptr, &errMsg);
    
    // Insert Carti
    auto insertCarteSQL = [&](const std::shared_ptr<Carte>& c, int deleted) {
        std::string tip = std::to_string(c->getTipFormat());
        std::string stare = c->getStareDefecta() ? "1" : "0";
        std::string link = "";
        int dim = 0;
        int durata = 0;
        std::string narator_str = "";
        std::string cladire = "", camera = "", culoar = "", raft = "";
        if(c->getTipFormat() == 0) {
            auto cf = std::dynamic_pointer_cast<CarteFizica>(c);
            if(cf) {
                Locatie loc = cf->getLocatieRaft();
                cladire = loc.cladire;
                camera = loc.camera;
                culoar = loc.culoar;
                raft = loc.raft;
            }
        } else if(c->getTipFormat() == 1) {
            auto cd = std::dynamic_pointer_cast<CarteDigitala>(c);
            if(cd) { link = cd->getLinkAcces(); dim = (int)cd->getDimensiuneMB(); }
        } else if(c->getTipFormat() == 2) {
            auto ca = std::dynamic_pointer_cast<CarteAudio>(c);
            if(ca) { link = ca->getLinkAcces(); durata = ca->getDurataMinute(); narator_str = ca->getNarator(); }
        }
        std::string autori_str = c->autoriToString();
        // Escape single quotes
        auto esc = [](const std::string& s) {
            std::string r;
            for(char ch : s) { if(ch == '\'') r += "''"; else r += ch; }
            return r;
        };
        std::string q = "INSERT INTO Carti (isbn, titlu, autori, editura, pret_intrare, categorie, an_aparitie, nr_pagini, stoc_disponibil, stoc_defect, data_adaugarii, contor_imprumuturi, tip_format, stare_defecta, link_acces, dimensiune_kb, is_deleted, durata_minute, narator, cladire, camera, culoar, raft) VALUES ('" +
            esc(c->getIsbn()) + "', '" + esc(c->getTitlu()) + "', '" + esc(autori_str) + "', '" + esc(c->getEditura()) + "', " + std::to_string(c->getPretIntrare()) + ", '" +
            esc(c->getCategorie()) + "', " + std::to_string(c->getAnAparitie()) + ", " + std::to_string(c->getNrPagini()) + ", " +
            std::to_string(c->getStocDisponibil()) + ", 0, '" + esc(c->getDataAdaugarii()) + "', " + std::to_string(c->getContorImprumuturi()) + ", " + tip + ", " + stare + ", '" + esc(link) + "', " + std::to_string(dim) + ", " + std::to_string(deleted) + ", " + std::to_string(durata) + ", '" + esc(narator_str) + "', '" + esc(cladire) + "', '" + esc(camera) + "', '" + esc(culoar) + "', '" + esc(raft) + "');";
        int rc = sqlite3_exec(db, q.c_str(), nullptr, nullptr, nullptr);
        if(rc != SQLITE_OK) {
            std::cerr << "SQL Insert Carte error: " << sqlite3_errmsg(db) << "\nQuery: " << q << std::endl;
        }
    };
    
    for(const auto& c : carti) insertCarteSQL(c, 0);
    for(const auto& c : carti_sterse) insertCarteSQL(c, 1);
    
    // Imprumuturi
    for(const auto& imp : imprumuturi) {
        auto esc = [](const std::string& s) {
            std::string r;
            for(char ch : s) { if(ch == '\'') r += "''"; else r += ch; }
            return r;
        };
        std::string q = "INSERT INTO Imprumuturi (isbn, id_cititor, nume_carte, data_imprumut, termen_limita, observatii) VALUES ('" +
            esc(imp.getIdCarte()) + "', '" + esc(imp.getIdCititor()) + "', '" + esc(imp.getNumeCititor()) + "', '" + esc(imp.getDataImprumut()) + "', '" + esc(imp.getTermenLimita()) + "', '" + esc(imp.getObservatii()) + "');";
        sqlite3_exec(db, q.c_str(), nullptr, nullptr, nullptr);
    }
    
    // Utilizatori
    auto insertUtilizator = [&](const std::shared_ptr<Utilizator>& u, int is_deleted) {
        auto esc = [](const std::string& s) {
            std::string r;
            for(char ch : s) { if(ch == '\'') r += "''"; else r += ch; }
            return r;
        };
        std::string q = "INSERT INTO Utilizatori (id, tip, parola, nume, prenume, cnp, email, telefon, adresa, departament, salariu, data_angajare, orar, tip_abonament, data_inregistrare, data_expirare, limita_imprumuturi, imprumuturi_curente, penalizari, is_deleted) VALUES ('" +
            esc(u->getId()) + "', '" + esc(u->getTip()) + "', '" + esc(u->getParola()) + "', '" + esc(u->getNume()) + "', '" + esc(u->getPrenume()) + "', '" + esc(u->getCnp()) + "', '" + esc(u->getEmail()) + "', '" + esc(u->getTelefon()) + "', '" + esc(u->getAdresa()) + "', ";
            
        std::string line = u->formatFisier();
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> parts;
        while(std::getline(ss, token, '|')) parts.push_back(token);
        
        if(u->getTip() == "CITITOR") {
            q += "NULL, NULL, NULL, NULL, '" + esc(parts[10]) + "', '" + esc(parts[11]) + "', '" + esc(parts[12]) + "', " + parts[13] + ", " + parts[14] + ", " + parts[15] + ", " + std::to_string(is_deleted) + ");";
        } else {
            q += "'" + esc(parts[10]) + "', " + parts[11] + ", '" + esc(parts[12]) + "', '" + esc(parts.size() > 13 ? parts[13] : "") + "', NULL, NULL, NULL, NULL, NULL, NULL, " + std::to_string(is_deleted) + ");";
        }
        sqlite3_exec(db, q.c_str(), nullptr, nullptr, nullptr);
    };
    for(const auto& u : utilizatori) insertUtilizator(u, 0);
    for(const auto& u : utilizatori_stersi) insertUtilizator(u, 1);
    
    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);

    std::ofstream fout_carti(fisier_carti, std::ios::trunc);
    if (fout_carti.is_open()) {
        for (const auto& carte : carti) {
            fout_carti << carte->formatFisier() << "\n";
        }
        fout_carti.close();
    }

    std::ofstream fout_util(fisier_utilizatori, std::ios::trunc);
    if (fout_util.is_open()) {
        for (const auto& util : utilizatori) {
            fout_util << util->formatFisier() << "\n";
        }
        fout_util.close();
    }

    std::ofstream fout_imp(fisier_imprumuturi, std::ios::trunc);
    if (fout_imp.is_open()) {
        for (const auto& imp : imprumuturi) {
            fout_imp << imp.formatFisier() << "\n";
        }
        fout_imp.close();
    }

    // Salveaza returnari in asteptare
    std::ofstream fout_ret(fisier_returnari, std::ios::trunc);
    if (fout_ret.is_open()) {
        for (const auto& r : returnari_in_asteptare) {
            fout_ret << r.isbn << "|" << r.id_cititor << "|"
                     << r.nume_cititor << "|" << r.titlu_carte << "|"
                     << r.data_returnare << "|" << r.amenda_intarziere << "\n";
        }
        fout_ret.close();
    }

    // Salveaza plati in asteptare
    std::ofstream fout_plati(fisier_plati, std::ios::trunc);
    if (fout_plati.is_open()) {
        for (const auto& p : plati_in_asteptare) {
            fout_plati << p.id_cititor << "|" << p.nume_cititor << "|"
                       << p.suma << "|" << p.data_solicitare << "\n";
        }
    }

    // Salveaza stoc defect
    std::ofstream fout_defecte(fisier_defecte, std::ios::trunc);
    if (fout_defecte.is_open()) {
        for (const auto& pair : stoc_defect) {
            if (pair.second > 0) {
                fout_defecte << pair.first << "|" << pair.second << "\n";
            }
        }
        fout_defecte.close();
    }

    // Salveaza rezervări
    std::ofstream fout_rez(fisier_rezervari, std::ios::trunc);
    if (fout_rez.is_open()) {
        for (const auto& r : rezervari) {
            fout_rez << r.isbn << "|" << r.id_cititor << "|" << r.nume_cititor << "|"
                     << r.data_rezervare << "|" << r.data_expirare << "|"
                     << (r.alocata ? "1" : "0") << "\n";
        }
        fout_rez.close();
    }

    // Salveaza istoric
    std::ofstream fout_ist(fisier_istoric, std::ios::trunc);
    if (fout_ist.is_open()) {
        for (const auto& i : istoric_lectura) {
            fout_ist << i.isbn << "|" << i.titlu_carte << "|" << i.id_cititor << "|"
                     << i.data_imprumut << "|" << i.data_returnare << "\n";
        }
        fout_ist.close();
    }

    // Salveaza recenzii
    std::ofstream fout_rec(fisier_recenzii, std::ios::trunc);
    if (fout_rec.is_open()) {
        for (const auto& r : recenzii) {
            fout_rec << r.isbn << "|" << r.id_cititor << "|" << r.nume_cititor << "|"
                     << r.nota << "|" << r.text << "|" << r.data << "\n";
        }
        fout_rec.close();
    }

    // Salveaza rezervări săli
    std::ofstream fout_sali(fisier_sali, std::ios::trunc);
    if (fout_sali.is_open()) {
        for (const auto& rs : rezervari_sali) {
            fout_sali << rs.id_cititor << "|" << rs.nume_cititor << "|" << rs.sala << "|"
                      << rs.data << "|" << rs.interval_orar << "\n";
        }
        fout_sali.close();
    }

    // Salveaza notificări
    std::ofstream fout_not(fisier_notificari, std::ios::trunc);
    if (fout_not.is_open()) {
        for (const auto& n : notificari) {
            fout_not << n.id_destinatar << "|" << n.mesaj << "|" << n.data << "|"
                     << (n.citita ? "1" : "0") << "\n";
        }
        fout_not.close();
        fout_not.close();
    }

    // Salveaza rafturi
    std::ofstream fout_rafturi(fisier_rafturi, std::ios::trunc);
    if (fout_rafturi.is_open()) {
        for (const auto& r : rafturi) {
            fout_rafturi << r.formatFisier() << "\n";
        }
        fout_rafturi.close();
    }
}

// ═══════════════════════════════════════════════
//  AUTENTIFICARE
// ═══════════════════════════════════════════════

std::shared_ptr<Utilizator> Biblioteca::autentificare(const std::string& id, const std::string& parola) const {
    for (const auto& u : utilizatori) {
        if (u->getId() == id && u->verificaParola(parola) && u->getActiv()) {
            return u;
        }
    }
    return nullptr;
}


// ═══════════════════════════════════════════════
//  CĂRȚI
// ═══════════════════════════════════════════════

bool Biblioteca::adaugaCarteFizica(const std::string& isbn, const std::string& titlu, const std::vector<std::string>& autori,
                                   const std::string& editura, double pret, const std::string& categorie, int an_aparitie, int nr_pagini,
                                   const std::string& data_adaugarii, int contor_imprumuturi,
                                   int stoc_disponibil, const Locatie& loc, const std::string& coperta, bool stare_defecta) {
    if(!existaRaft(loc)) return false; // Nu putem adauga pe un raft inexistent
    int stoc_curent_raft = obtineNumarCartiFiziceRaft(loc);
    if (stoc_curent_raft + stoc_disponibil > capacitateRaft(loc)) return false;

    auto nouaCarte = std::make_shared<CarteFizica>(isbn, titlu, autori, editura, pret, categorie, an_aparitie, nr_pagini,
                                                   data_adaugarii, contor_imprumuturi, stoc_disponibil, loc, coperta, stare_defecta);
    carti.push_back(nouaCarte);
    return true;
}

void Biblioteca::adaugaCarteDigitala(const std::string& isbn, const std::string& titlu, const std::vector<std::string>& autori,
                                     const std::string& editura, double pret, const std::string& categorie, int an_aparitie, int nr_pagini,
                                     const std::string& data_adaugarii, int contor_imprumuturi,
                                     const std::string& format, double dimensiune, const std::string& link) {
    auto nouaCarte = std::make_shared<CarteDigitala>(isbn, titlu, autori, editura, pret, categorie, an_aparitie, nr_pagini,
                                                     data_adaugarii, contor_imprumuturi, format, dimensiune, link);
    carti.push_back(nouaCarte);
}

void Biblioteca::adaugaCarteAudio(const std::string& isbn, const std::string& titlu, const std::vector<std::string>& autori,
                                  const std::string& editura, double pret, const std::string& categorie, int an_aparitie, int nr_pagini,
                                  const std::string& data_adaugarii, int contor_imprumuturi,
                                  int durata_minute, const std::string& narator, const std::string& link) {
    auto nouaCarte = std::make_shared<CarteAudio>(isbn, titlu, autori, editura, pret, categorie, an_aparitie, nr_pagini,
                                                   data_adaugarii, contor_imprumuturi, durata_minute, narator, link);
    carti.push_back(nouaCarte);
}

bool Biblioteca::stergeCarte(const std::string& isbn, const std::string& id_actor) {
    auto it = std::find_if(carti.begin(), carti.end(),
        [&isbn](const std::shared_ptr<Carte>& c) { return c->getIsbn() == isbn; });

    if (it != carti.end()) {
        carti_sterse.push_back(*it); // Move to deleted
        carti.erase(it);
        
        auto it_imp = std::remove_if(imprumuturi.begin(), imprumuturi.end(),
            [&isbn](const Imprumut& i) { return i.getIdCarte() == isbn; });
        imprumuturi.erase(it_imp, imprumuturi.end());

        if(!id_actor.empty()) scrieLaLog(id_actor, "STERGERE CARTE: " + isbn);
        return true;
    }
    return false;
}
bool Biblioteca::restaureazaCarte(const std::string& isbn, const std::string& id_actor) {
    auto it = std::find_if(carti_sterse.begin(), carti_sterse.end(),
        [&isbn](const std::shared_ptr<Carte>& c) { return c->getIsbn() == isbn; });

    if (it != carti_sterse.end()) {
        carti.push_back(*it);
        carti_sterse.erase(it);
        if(!id_actor.empty()) scrieLaLog(id_actor, "RESTAURARE CARTE: " + isbn);
        return true;
    }
    return false;
}

bool Biblioteca::restaureazaUtilizator(const std::string& id, const std::string& id_actor) {
    auto it = std::find_if(utilizatori_stersi.begin(), utilizatori_stersi.end(),
        [&id](const std::shared_ptr<Utilizator>& u) { return u->getId() == id; });

    if (it != utilizatori_stersi.end()) {
        utilizatori.push_back(*it);
        utilizatori_stersi.erase(it);
        if(!id_actor.empty()) scrieLaLog(id_actor, "RESTAURARE UTILIZATOR: " + id);
        return true;
    }
    return false;
}

bool Biblioteca::modificaCarte(const std::string& isbn, const std::string& noul_titlu, const std::vector<std::string>& noii_autori, const std::string& noua_editura, double noul_pret, const std::string& noua_categorie, int noul_an, int noile_pagini, const std::string& id_actor) {
    auto carte = gasesteCarte(isbn);
    if (!carte) return false;
    
    if (!noul_titlu.empty()) carte->setTitlu(noul_titlu);
    if (!noii_autori.empty()) carte->setAutori(noii_autori);
    if (!noua_editura.empty()) carte->setEditura(noua_editura);
    if (noul_pret > 0) carte->setPretIntrare(noul_pret);
    if (!noua_categorie.empty()) carte->setCategorie(noua_categorie);
    if (noul_an > 0) carte->setAnAparitie(noul_an);
    if (noile_pagini > 0) carte->setNrPagini(noile_pagini);
    
    if (!id_actor.empty()) scrieLaLog(id_actor, "a modificat cartea ISBN: " + isbn);
    return true;
}

bool Biblioteca::modificaUtilizator(const std::string& id, const std::string& noua_parola, const std::string& noul_nume, const std::string& noul_prenume, const std::string& noul_email, const std::string& noul_telefon, const std::string& noua_adresa, const std::string& id_actor) {
    auto u = gasesteUtilizator(id);
    if (!u) return false;
    
    if (!noua_parola.empty()) u->setParola(noua_parola);
    if (!noul_nume.empty()) u->setNume(noul_nume);
    if (!noul_prenume.empty()) u->setPrenume(noul_prenume);
    if (!noul_email.empty()) u->setEmail(noul_email);
    if (!noul_telefon.empty()) u->setTelefon(noul_telefon);
    if (!noua_adresa.empty()) u->setAdresa(noua_adresa);
    
    if (!id_actor.empty()) scrieLaLog(id_actor, "a modificat utilizatorul ID: " + id);
    return true;
}

// ═══════════════════════════════════════════════
//  FINANCIAR & SIMULARE
// ═══════════════════════════════════════════════

void Biblioteca::incarcaBuget() {
    std::ifstream f(fisier_buget);
    if (f.is_open()) {
        f >> buget;
        f.close();
    } else {
        buget = 50000.0;
        salveazaBuget();
    }
}

void Biblioteca::salveazaBuget() const {
    std::ofstream f(fisier_buget);
    if (f.is_open()) {
        f << buget;
        f.close();
    }
}

double Biblioteca::getBuget() const {
    return buget;
}

double Biblioteca::calculeazaSalariiTotale() const {
    double total = 0;
    std::ifstream f(fisier_utilizatori);
    std::string linie;
    while (std::getline(f, linie)) {
        auto campuri = splitLinie(linie, '|');
        if (campuri.empty()) continue;
        std::string tip = campuri[0];
        if ((tip == "DIRECTOR" || tip == "BIBLIOTECAR" || tip == "INGRIJITOR") && campuri.size() >= 12) {
            total += std::stod(campuri[11]);
        }
    }
    return total;
}

void Biblioteca::platesteSalarii() {
    time_t acum = getVirtualTime();
    double diferenta_secunde = difftime(acum, ultima_plata_salarii);
    int zile_trecute = diferenta_secunde / (24 * 3600);
    int cicluri = zile_trecute / 14; // Un ciclu = 2 saptamani
    
    if (cicluri == 0) {
        std::cout << "\n  \033[33m[!] Nu au trecut 14 zile de la ultima plată. Au trecut doar " << zile_trecute << " zile.\033[0m\n";
        return;
    }
    
    double salarii = calculeazaSalariiTotale();
    double cheltuieli_fixe = 5000.0;
    
    double venituri_stat = 25000.0;
    double venituri_abonamente = 5000.0;
    double venituri_donatii = 2000.0;
    double venituri_totale = venituri_stat + venituri_abonamente + venituri_donatii;
    
    double total_per_ciclu = salarii + cheltuieli_fixe;
    
    for (int i = 0; i < cicluri; i++) {
        buget += venituri_totale;
        buget -= total_per_ciclu;
    }
    
    ultima_plata_salarii += cicluri * 14 * 24 * 3600; // Avansam pointerul de plata
    
    std::cout << "\n  [Finanțe] S-au procesat " << cicluri * 2 << " săptămâni (" << cicluri << " cicluri de plată acumulate).\n";
    std::cout << "  \033[32m[+] Venituri încasate: " << venituri_totale * cicluri << " RON\033[0m (Stat: 15k, Abonamente: 2k, Donații: 1k per ciclu)\n";
    std::cout << "  \033[31m[-] Salarii plătite: " << salarii * cicluri << " RON\033[0m\n";
    std::cout << "  \033[31m[-] Cheltuieli administrative: " << cheltuieli_fixe * cicluri << " RON\033[0m\n";
    std::cout << "  Buget nou: \033[36m" << buget << " RON\033[0m\n";
    
    salveazaBuget();
    salveazaTimp();
}

void Biblioteca::simuleazaTrecereTimp(int zile) {
    offset_timp += zile * 24 * 3600;
    
    std::cout << "\n  [Simulare] Timpul a fost avansat cu " << zile << " zile.\n";
    
    time_t virtual_time = getVirtualTime();
    struct tm* ti = localtime(&virtual_time);
    char buf[80];
    strftime(buf, sizeof(buf), "%d/%m/%Y", ti);
    std::cout << "  Data curentă în aplicație: \033[33m" << buf << "\033[0m\n";
    
    // Generează notificări de expirări
    genereazaNotificariExpirari();
    
    // Verifică rezervări expirate
    verificaRezervariExpirate();
    
    salveazaTimp();
}

void Biblioteca::incarcaTimp() {
    std::ifstream f(fisier_timp);
    if (f.is_open()) {
        f >> offset_timp;
        if (!(f >> ultima_plata_salarii)) {
            ultima_plata_salarii = time(nullptr) + offset_timp; // fallback pentru fisiere vechi
        }
        f.close();
    } else {
        offset_timp = 0;
        ultima_plata_salarii = time(nullptr);
        salveazaTimp();
    }
}

void Biblioteca::salveazaTimp() const {
    std::ofstream f(fisier_timp);
    if (f.is_open()) {
        f << offset_timp << " " << ultima_plata_salarii;
        f.close();
    }
}

time_t Biblioteca::getVirtualTime() const {
    return time(nullptr) + offset_timp;
}

double Biblioteca::calculeazaPenalizariTotale(const std::string& id_cititor) const {
    double total = 0;
    auto u = gasesteCititor(id_cititor);
    if (u) total += u->getPenalizari();
    
    for (const auto& imp : imprumuturi) {
        if (imp.getIdCititor() == id_cititor) {
            int zile = calculeazaZileIntarziere(imp.getTermenLimita());
            if (zile > 0) {
                total += zile * 1.0; // 1 RON pe zi
            }
        }
    }
    return total;
}

time_t Biblioteca::stringToTime(const std::string& data_str) {
    struct tm tm_val = {0};
    int d, m, y;
    if (sscanf(data_str.c_str(), "%d/%d/%d", &d, &m, &y) == 3) {
        tm_val.tm_mday = d;
        tm_val.tm_mon = m - 1;
        tm_val.tm_year = y - 1900;
        return mktime(&tm_val);
    }
    return 0;
}

int Biblioteca::calculeazaZileIntarziere(const std::string& data_limita_str) const {
    time_t limita = stringToTime(data_limita_str);
    time_t acum = getVirtualTime();
    if (acum <= limita) return 0;
    return (int)difftime(acum, limita) / (60 * 60 * 24);
}

std::string Biblioteca::getDataCurentaStr() const {
    time_t rawtime = getVirtualTime();
    struct tm * timeinfo = localtime(&rawtime);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y", timeinfo);
    return std::string(buffer);
}

std::string Biblioteca::getDataCurentaPlusStr(int zile) const {
    time_t rawtime = getVirtualTime();
    rawtime += zile * 24 * 60 * 60;
    struct tm * timeinfo = localtime(&rawtime);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y", timeinfo);
    return std::string(buffer);
}

std::vector<std::shared_ptr<Carte>> Biblioteca::cautaDupaAutor(const std::string& autor) const {
    std::vector<std::shared_ptr<Carte>> rez;
    for (const auto& c : carti) {
        if (c->autoriToString().find(autor) != std::string::npos) rez.push_back(c);
    }
    return rez;
}

std::vector<std::shared_ptr<Carte>> Biblioteca::cautaDupaNume(const std::string& nume) const {
    std::vector<std::shared_ptr<Carte>> rez;
    for (const auto& c : carti) {
        if (c->getTitlu().find(nume) != std::string::npos) rez.push_back(c);
    }
    return rez;
}

std::vector<std::shared_ptr<Carte>> Biblioteca::cautaDupaCategorie(const std::string& categorie) const {
    std::vector<std::shared_ptr<Carte>> rez;
    for (const auto& c : carti) {
        if (c->getCategorie().find(categorie) != std::string::npos) rez.push_back(c);
    }
    return rez;
}

std::vector<std::shared_ptr<Carte>> Biblioteca::cautaDupaEditura(const std::string& editura) const {
    std::vector<std::shared_ptr<Carte>> rez;
    for (const auto& c : carti) {
        if (c->getEditura().find(editura) != std::string::npos) rez.push_back(c);
    }
    return rez;
}

bool Biblioteca::reparaCarte(const std::string& isbn) {
    auto c = gasesteCarte(isbn);
    if (!c) return false;
    if (c->getStareDefecta()) {
        c->setStareDefecta(false);
        c->setStocDisponibil(c->getStocDisponibil() + 1);
        if (stoc_defect.find(isbn) != stoc_defect.end() && stoc_defect[isbn] > 0) {
            stoc_defect[isbn]--;
        }
        return true;
    }
    return false;
}

bool Biblioteca::adaugaImprumut(const std::string& isbn, const std::string& id_cititor,
                    const std::string& data_imp, const std::string& termen,
                    const std::string& obs) {
    auto c = gasesteCarte(isbn);
    if (!c || c->getStocDisponibil() <= 0) return false;
    c->setStocDisponibil(c->getStocDisponibil() - 1);
    c->setContorImprumuturi(c->getContorImprumuturi() + 1);
    Imprumut i(isbn, id_cititor, "", data_imp, termen, obs);
    imprumuturi.push_back(i);
    return true;
}

bool Biblioteca::returneazaCarte(const std::string& isbn, const std::string& id_cititor, const std::string& id_actor) {
    auto it = std::find_if(imprumuturi.begin(), imprumuturi.end(), [&](const Imprumut& i) {
        return i.getIdCarte() == isbn && i.getIdCititor() == id_cititor;
    });
    if (it != imprumuturi.end()) {
        auto c = gasesteCarte(isbn);
        if (c) c->setStocDisponibil(c->getStocDisponibil() + 1);
        
        auto u = gasesteUtilizator(id_cititor);
        if(u && u->getRol() == "Cititor") {
            auto cit = std::dynamic_pointer_cast<Cititor>(u);
            if(cit) cit->decrementeazaImprumuturi();
        }
        
        std::string titlu = c ? c->getTitlu() : isbn;
        adaugaInIstoric(isbn, titlu, id_cititor, it->getDataImprumut(), getDataCurentaStr());
        
        imprumuturi.erase(it);
        alocaRezervare(isbn);
        return true;
    }
    return false;
}

bool Biblioteca::solicitaReturnare(const std::string& isbn, const std::string& id_cititor) {
    for(const auto& r : returnari_in_asteptare) {
        if(r.isbn == isbn && r.id_cititor == id_cititor) return false;
    }
    
    auto it = std::find_if(imprumuturi.begin(), imprumuturi.end(), [&](const Imprumut& i) {
        return i.getIdCarte() == isbn && i.getIdCititor() == id_cititor;
    });
    if (it != imprumuturi.end()) {
        ReturnareInAsteptare r = { isbn, id_cititor, "", "", getDataCurentaStr(), 0.0 };
        returnari_in_asteptare.push_back(r);
        return true;
    }
    return false;
}

bool Biblioteca::confirmaReturnare(size_t index, const std::string& id_actor) {
    if (index >= returnari_in_asteptare.size()) return false;
    auto r = returnari_in_asteptare[index];
    if (returneazaCarte(r.isbn, r.id_cititor, id_actor)) {
        returnari_in_asteptare.erase(returnari_in_asteptare.begin() + index);
        return true;
    }
    return false;
}

bool Biblioteca::refuzaReturnareDefecta(size_t index, const std::string& id_actor) {
    if (index >= returnari_in_asteptare.size()) return false;
    auto r = returnari_in_asteptare[index];
    auto c = gasesteCarte(r.isbn);
    if(c) {
        c->setStareDefecta(true);
        stoc_defect[c->getIsbn()]++;
    }
    
    auto it = std::find_if(imprumuturi.begin(), imprumuturi.end(), [&](const Imprumut& i) {
        return i.getIdCarte() == r.isbn && i.getIdCititor() == r.id_cititor;
    });
    if (it != imprumuturi.end()) {
        imprumuturi.erase(it);
        auto u = gasesteUtilizator(r.id_cititor);
        if(u && u->getRol() == "Cititor") {
            auto cit = std::dynamic_pointer_cast<Cititor>(u);
            if(cit) {
                cit->decrementeazaImprumuturi();
                cit->setPenalizari(cit->getPenalizari() + 50.0);
            }
        }
    }
    
    returnari_in_asteptare.erase(returnari_in_asteptare.begin() + index);
    return true;
}

bool Biblioteca::rezervaSala(const std::string& id_cititor, const std::string& nume_cititor,
                 const std::string& sala, const std::string& data,
                 const std::string& interval) {
    if(!esteSalaDisponibila(sala, data, interval)) return false;
    RezervareStudiu rs = { id_cititor, nume_cititor, sala, data, interval };
    rezervari_sali.push_back(rs);
    return true;
}

bool Biblioteca::esteSalaDisponibila(const std::string& sala, const std::string& data,
                         const std::string& interval) const {
    for(const auto& r : rezervari_sali) {
        if(r.sala == sala && r.data == data && r.interval_orar == interval) return false;
    }
    return true;
}

bool Biblioteca::adaugaRezervare(const std::string& isbn, const std::string& id_cititor) {
    Rezervare r = { isbn, id_cititor, "", getDataCurentaStr(), "", false };
    rezervari.push_back(r);
    
    alocaRezervare(isbn);
    
    return true;
}

bool Biblioteca::anuleazaRezervare(const std::string& isbn, const std::string& id_cititor) {
    auto it = std::remove_if(rezervari.begin(), rezervari.end(), [&](const Rezervare& r){
        return r.isbn == isbn && r.id_cititor == id_cititor;
    });
    if(it != rezervari.end()) {
        rezervari.erase(it, rezervari.end());
        return true;
    }
    return false;
}

void Biblioteca::alocaRezervare(const std::string& isbn) {
    for(auto& r : rezervari) {
        if(r.isbn == isbn && r.alocata == false) {
            auto c = gasesteCarte(isbn);
            if(c && c->getStocDisponibil() > 0) {
                c->setStocDisponibil(c->getStocDisponibil() - 1);
                r.alocata = true;
                r.data_expirare = getDataCurentaPlusStr(3);
                adaugaNotificare(r.id_cititor, "Cartea ta rezervata este disponibila pentru ridicare!");
            }
            break;
        }
    }
}

bool Biblioteca::confirmaRidicare(size_t index, const std::string& id_actor) {
    if(index >= rezervari.size()) return false;
    if(rezervari[index].alocata == true) {
        Imprumut imp(rezervari[index].isbn, rezervari[index].id_cititor, rezervari[index].nume_cititor, getDataCurentaStr(), getDataCurentaPlusStr(14), "Din rezervare");
        imprumuturi.push_back(imp);
        
        auto c = gasesteCarte(rezervari[index].isbn);
        if(c) c->setContorImprumuturi(c->getContorImprumuturi() + 1);
        
        rezervari.erase(rezervari.begin() + index);
        return true;
    }
    return false;
}

bool Biblioteca::adaugaRecenzie(const std::string& isbn, const std::string& id_cititor,
                    const std::string& nume_cititor, int nota, const std::string& text) {
    if(nota < 1 || nota > 5) return false;
    Recenzie r = { isbn, id_cititor, nume_cititor, nota, text, getDataCurentaStr() };
    recenzii.push_back(r);
    return true;
}

double Biblioteca::calculeazaRatingMediu(const std::string& isbn) const {
    double suma = 0;
    int count = 0;
    for(const auto& r : recenzii) {
        if(r.isbn == isbn) { suma += r.nota; count++; }
    }
    return count == 0 ? 0 : suma / count;
}

void Biblioteca::afiseazaRecenziiCarte(std::ostream& os, const std::string& isbn) const {
    for(const auto& r : recenzii) {
        if(r.isbn == isbn) {
            os << "Nota: " << r.nota << "/5 | De: " << r.nume_cititor << " | Data: " << r.data << "\n";
            os << r.text << "\n---\n";
        }
    }
}

void Biblioteca::afiseazaRecenziileCititorului(std::ostream& os, const std::string& id_cititor) const {
    bool gasit = false;
    os << "\n";
    for(const auto& r : recenzii) {
        if(r.id_cititor == id_cititor) {
            auto c = gasesteCarte(r.isbn);
            std::string titlu = c ? c->getTitlu() : r.isbn;
            os << "  ⭐ Nota: " << r.nota << "/5 | Carte: " << titlu << "\n";
            os << "     Data: " << r.data << "\n";
            os << "     Recenzie: " << r.text << "\n";
            os << "  ----------------------------------------\n";
            gasit = true;
        }
    }
    if(!gasit) os << "  📭 Nu ai scris nicio recenzie.\n\n";
}

bool Biblioteca::achizitioneazaExemplare(const std::string& isbn, int cantitate, const std::string& id_actor) {
    auto c = gasesteCarte(isbn);
    if(c) {
        c->setStocDisponibil(c->getStocDisponibil() + cantitate);
        for(int i = 0; i < cantitate; ++i) {
            alocaRezervare(isbn);
        }
        return true;
    }
    return false;
}

bool Biblioteca::caseazaCarte(const std::string& isbn, const std::string& motiv, const std::string& id_actor) {
    auto c = gasesteCarte(isbn);
    if(c && c->getStocDisponibil() > 0) {
        c->setStocDisponibil(c->getStocDisponibil() - 1);
        
        if(!motiv.empty()) {
            auto u = gasesteUtilizator(motiv);
            if(u && u->getRol() == "Cititor") {
                auto cit = std::dynamic_pointer_cast<Cititor>(u);
                if(cit) cit->setPenalizari(cit->getPenalizari() + 50.0);
            }
        }
        return true;
    }
    return false;
}

bool Biblioteca::confirmaPlata(size_t index, const std::string& id_actor) {
    if(index >= plati_in_asteptare.size()) return false;
    
    // Gaseste cititorul si sterge amenzile
    auto u = gasesteUtilizator(plati_in_asteptare[index].id_cititor);
    if(u && u->getRol() == "Cititor") {
        auto cit = std::dynamic_pointer_cast<Cititor>(u);
        if(cit) cit->setPenalizari(0.0);
    }
    
    plati_in_asteptare.erase(plati_in_asteptare.begin() + index);
    return true;
}

bool Biblioteca::refuzaPlata(size_t index) {
    if(index >= plati_in_asteptare.size()) return false;
    plati_in_asteptare.erase(plati_in_asteptare.begin() + index);
    return true;
}

void Biblioteca::verificaRezervariExpirate() {
    auto it = rezervari.begin();
    while (it != rezervari.end()) {
        if (it->alocata) {
            int zile_trecute = calculeazaZileIntarziere(it->data_expirare);
            if (zile_trecute > 0) {
                auto c = gasesteCarte(it->isbn);
                if (c) c->setStocDisponibil(c->getStocDisponibil() + 1);
                adaugaNotificare(it->id_cititor, "Rezervarea pentru carte a expirat (neridicata la timp).");
                it = rezervari.erase(it);
                continue;
            }
        }
        ++it;
    }
}
void Biblioteca::afiseazaRezervariCititor(std::ostream& os, const std::string& id_cititor) const {
    bool gasit = false;
    os << "\n";
    for(const auto& r : rezervari) {
        if(r.id_cititor == id_cititor) {
            auto c = gasesteCarte(r.isbn);
            std::string titlu = c ? c->getTitlu() : r.isbn;
            os << "  📌 Titlu: " << titlu << "\n";
            os << "     Data rezervare: " << r.data_rezervare << "\n";
            if(r.alocata) {
                os << "     Stare: " << "\033[32m[GATA DE RIDICAT]\033[0m" << " (Expira la: " << r.data_expirare << ")\n";
            } else {
                os << "     Stare: " << "\033[33m[In asteptare stoc]\033[0m" << "\n";
            }
            os << "  ----------------------------------------\n";
            gasit = true;
        }
    }
    if(!gasit) os << "  📭 Nu ai nicio rezervare de carte activa.\n\n";
}

void Biblioteca::afiseazaRezervariAlocate(std::ostream& os) const {
    for(const auto& r : rezervari) {
        if(r.alocata) {
            auto c = gasesteCarte(r.isbn);
            std::string titlu = c ? c->getTitlu() : r.isbn;
            os << "Cititor: " << r.id_cititor << " | Carte: " << titlu << " | Expira: " << r.data_expirare << "\n";
        }
    }
}

size_t Biblioteca::getNumarRezervariAlocate() const {
    size_t count = 0;
    for(const auto& r : rezervari) {
        if(r.alocata) count++;
    }
    return count;
}

void Biblioteca::adaugaInIstoric(const std::string& isbn, const std::string& titlu, const std::string& id_cititor, const std::string& data_imp, const std::string& data_ret) {
    InregistrareIstoric i = { isbn, titlu, id_cititor, data_imp, data_ret };
    istoric_lectura.push_back(i);
}

void Biblioteca::afiseazaIstoricCititor(std::ostream& os, const std::string& id_cititor) const {
    bool gasit = false;
    os << "\n";
    for(const auto& i : istoric_lectura) {
        if(i.id_cititor == id_cititor) {
            os << "  📖 Titlu: " << i.titlu_carte << "\n";
            os << "     Imprumutat la: " << i.data_imprumut << " | Returnat la: " << i.data_returnare << "\n";
            os << "  ----------------------------------------\n";
            gasit = true;
        }
    }
    if(!gasit) os << "  📭 Nu ai istoric de lectura.\n\n";
}

bool Biblioteca::aCititCartea(const std::string& isbn, const std::string& id_cititor) const {
    for(const auto& i : istoric_lectura) {
        if(i.isbn == isbn && i.id_cititor == id_cititor) return true;
    }
    return false;
}

bool Biblioteca::areRecenzie(const std::string& isbn, const std::string& id_cititor) const {
    for(const auto& r : recenzii) {
        if(r.isbn == isbn && r.id_cititor == id_cititor) return true;
    }
    return false;
}

int Biblioteca::getNumarRecenzii(const std::string& isbn) const {
    int count = 0;
    for(const auto& r : recenzii) {
        if(r.isbn == isbn) count++;
    }
    return count;
}

void Biblioteca::scrieLaLog(const std::string& actor, const std::string& actiune) {
    std::ofstream fout(fisier_log, std::ios::app);
    if(fout.is_open()) {
        fout << "[" << getDataCurentaStr() << "] Actor: " << actor << " | Actiune: " << actiune << "\n";
        fout.close();
    }
}

int Biblioteca::obtineNumarCartiFiziceRaft(const Locatie& loc) const {
    int contor = 0;
    for(const auto& c : carti) {
        if(c->getTipFormat() == 0) {
            auto cf = std::dynamic_pointer_cast<CarteFizica>(c);
            if(cf && cf->getLocatieRaft().cladire == loc.cladire && cf->getLocatieRaft().camera == loc.camera && 
               cf->getLocatieRaft().culoar == loc.culoar && cf->getLocatieRaft().raft == loc.raft) {
                contor++;
            }
        }
    }
    return contor;
}

std::shared_ptr<Carte> Biblioteca::gasesteCarte(const std::string& isbn) const {
    auto it = std::find_if(carti.begin(), carti.end(), [&](const std::shared_ptr<Carte>& c) {
        return c->getIsbn() == isbn;
    });
    if(it != carti.end()) return *it;
    return nullptr;
}

std::shared_ptr<Utilizator> Biblioteca::gasesteUtilizator(const std::string& id) const {
    auto it = std::find_if(utilizatori.begin(), utilizatori.end(), [&](const std::shared_ptr<Utilizator>& u) {
        return u->getId() == id;
    });
    if(it != utilizatori.end()) return *it;
    return nullptr;
}

bool Biblioteca::solicitaPlata(const std::string& id_cititor) {
    for(const auto& p : plati_in_asteptare) {
        if(p.id_cititor == id_cititor) return false;
    }
    
    auto u = gasesteUtilizator(id_cititor);
    if(u && u->getRol() == "Cititor") {
        auto cit = std::dynamic_pointer_cast<Cititor>(u);
        if(cit && cit->getPenalizari() > 0) {
            PlataInAsteptare p = { id_cititor, u->getNumeComplet(), cit->getPenalizari(), getDataCurentaStr() };
            plati_in_asteptare.push_back(p);
            return true;
        }
    }
    return false;
}

void Biblioteca::adaugaDirector(const std::string& id, const std::string& parola, const std::string& nume, const std::string& prenume,
                    const std::string& cnp, const std::string& email, const std::string& telefon,
                    const std::string& adresa,
                    const std::string& departament, double salariu,
                    const std::string& data_angajare, const std::string& birou, int nivel_acces) {
    utilizatori.push_back(std::make_shared<Director>(id, parola, nume, prenume, cnp, email, telefon, adresa, true, departament, salariu, data_angajare, birou, nivel_acces));
}

void Biblioteca::adaugaBibliotecar(const std::string& id, const std::string& parola, const std::string& nume, const std::string& prenume,
                       const std::string& cnp, const std::string& email, const std::string& telefon,
                       const std::string& adresa,
                       const std::string& sectie, double salariu,
                       const std::string& data_angajare, const std::string& program_lucru,
                       int nr_carti_gestionate) {
    utilizatori.push_back(std::make_shared<Bibliotecar>(id, parola, nume, prenume, cnp, email, telefon, adresa, true, sectie, salariu, data_angajare, program_lucru, nr_carti_gestionate));
}

void Biblioteca::adaugaIngrijitor(const std::string& id, const std::string& parola, const std::string& nume, const std::string& prenume,
                      const std::string& cnp, const std::string& email, const std::string& telefon,
                      const std::string& adresa,
                      const std::string& zona, double salariu,
                      const std::string& data_angajare, const std::string& program_lucru,
                      const std::string& echipament) {
    utilizatori.push_back(std::make_shared<Ingrijitor>(id, parola, nume, prenume, cnp, email, telefon, adresa, true, zona, salariu, data_angajare, program_lucru, echipament));
}

void Biblioteca::adaugaCititor(const std::string& id, const std::string& parola, const std::string& nume, const std::string& prenume,
                   const std::string& cnp, const std::string& email, const std::string& telefon,
                   const std::string& adresa,
                   const std::string& tip_abonament, const std::string& data_inregistrare,
                   const std::string& data_expirare, int max_carti) {
    auto u = std::make_shared<Cititor>(id, parola, nume, prenume, cnp, email, telefon, adresa, true, tip_abonament, data_inregistrare, data_expirare, max_carti, 0, 0.0);
    u->setParola(parola); // Forteaza encodarea corecta in momentul crearii
    utilizatori.push_back(u);
}

bool Biblioteca::stergeUtilizator(const std::string& id, const std::string& id_actor) {
    auto it = std::find_if(utilizatori.begin(), utilizatori.end(),
        [&id](const std::shared_ptr<Utilizator>& u) { return u->getId() == id; });

    if (it != utilizatori.end()) {
        utilizatori_stersi.push_back(*it);
        utilizatori.erase(it);
        
        auto it_imp = std::remove_if(imprumuturi.begin(), imprumuturi.end(),
            [&id](const Imprumut& i) { return i.getIdCititor() == id; });
        imprumuturi.erase(it_imp, imprumuturi.end());

        if(!id_actor.empty()) scrieLaLog(id_actor, "STERGERE UTILIZATOR: " + id);
        return true;
    }
    return false;
}

std::shared_ptr<Cititor> Biblioteca::gasesteCititor(const std::string& id) const {
    auto u = gasesteUtilizator(id);
    if(u && u->getTip() == "CITITOR") return std::dynamic_pointer_cast<Cititor>(u);
    return nullptr;
}

void Biblioteca::afiseazaJurnalAudit(std::ostream& os, int linii) const {
    os << "--- Jurnal Audit ---\n";
    std::ifstream fin(fisier_log);
    if(fin.is_open()) {
        std::string linie;
        while(std::getline(fin, linie)) {
            os << linie << "\n";
        }
        fin.close();
    }
}

void Biblioteca::afiseazaUtilizatoriScurt(std::ostream& os) const {
    os << "\n  " << Color::Cyan << Color::Bold 
       << std::left << std::setw(15) << "ROL" 
       << std::setw(10) << "ID" 
       << std::setw(30) << "NUME COMPLET" 
       << std::setw(35) << "EMAIL" << Color::Reset << "\n";
    os << "  " << std::string(85, '-') << "\n";
    
    for(const auto& u : utilizatori) {
        std::string nume = u->getNumeComplet();
        if(nume.length() > 28) nume = nume.substr(0, 25) + "...";
        std::string email = u->getEmail();
        if(email.length() > 33) email = email.substr(0, 30) + "...";
        
        std::string rol_color = Color::Reset;
        if(u->getTip() == "DIRECTOR") rol_color = Color::Magenta;
        else if(u->getTip() == "BIBLIOTECAR") rol_color = Color::Yellow;
        else if(u->getTip() == "CITITOR") rol_color = Color::Green;
        else rol_color = Color::Cyan;

        os << "  " << rol_color << std::left << std::setw(15) << u->getTip() << Color::Reset
           << std::left << std::setw(10) << u->getId() 
           << std::left << std::setw(30) << nume 
           << std::left << std::setw(35) << email << "\n";
    }
    os << "  " << std::string(85, '-') << "\n\n";
}

void Biblioteca::afiseazaToateImprumuturile(std::ostream& os) const {
    for(const auto& i : imprumuturi) {
        os << "Carte: " << i.getIdCarte() << " | Cititor: " << i.getIdCititor() << " | Data: " << i.getDataImprumut() << "\n";
    }
}

void Biblioteca::afiseazaImprumuturiCititor(std::ostream& os, const std::string& id_cititor) const {
    for(const auto& i : imprumuturi) {
        if(i.getIdCititor() == id_cititor) {
            os << "Carte: " << i.getIdCarte() << " | Data: " << i.getDataImprumut() << " | Termen: " << i.getTermenLimita() << "\n";
        }
    }
}

void Biblioteca::afiseazaRezervariSaliCititor(std::ostream& os, const std::string& id_cititor) const {
    for(const auto& r : rezervari_sali) {
        if(r.id_cititor == id_cititor) {
            os << "Sala: " << r.sala << " | Data: " << r.data << " | Interval: " << r.interval_orar << "\n";
        }
    }
}

void Biblioteca::adaugaNotificare(const std::string& id_destinatar, const std::string& mesaj) {
    Notificare n = { id_destinatar, mesaj, getDataCurentaStr(), false };
    notificari.push_back(n);
}

void Biblioteca::genereazaNotificariExpirari() {
    for (auto& imp : imprumuturi) {
        int zile = calculeazaZileIntarziere(imp.getTermenLimita());
        if (zile > 0) {
            double penalizare = zile * 1.0; // 1 RON / zi întârziere
            auto cit = gasesteCititor(imp.getIdCititor());
            if (cit) {
                cit->setPenalizari(cit->getPenalizari() + penalizare);
                adaugaNotificare(cit->getId(), "Avertisment: Intarziere retur! Penalizare: " + std::to_string((int)penalizare) + " RON pentru " + std::to_string(zile) + " zile.");
            }
            imp.setTermenLimita(getDataCurentaStr());
        }
    }
}

bool Biblioteca::existaRaft(const Locatie& loc) const {
    for(const auto& r : rafturi) {
        if(r.cladire == loc.cladire && r.camera == loc.camera && 
           r.culoar == loc.culoar && r.nume_raft == loc.raft) return true;
    }
    return false;
}

int Biblioteca::capacitateRaft(const Locatie& loc) const {
    for(const auto& r : rafturi) {
        if(r.cladire == loc.cladire && r.camera == loc.camera && 
           r.culoar == loc.culoar && r.nume_raft == loc.raft) return r.capacitate_maxima;
    }
    return 0;
}

bool Biblioteca::adaugaRaft(const std::string& cladire, const std::string& camera, const std::string& culoar, const std::string& nume_raft, int capacitate) {
    Locatie loc = {cladire, camera, culoar, nume_raft};
    if(existaRaft(loc)) return false;
    std::string id = cladire + "-" + camera + "-" + culoar + "-" + nume_raft;
    rafturi.push_back({id, cladire, camera, culoar, nume_raft, capacitate});
    return true;
}

bool Biblioteca::stergeRaft(const std::string& id_raft) {
    auto it = std::find_if(rafturi.begin(), rafturi.end(), [&](const Raft& r){ return r.id == id_raft; });
    if(it != rafturi.end()) {
        Locatie loc = {it->cladire, it->camera, it->culoar, it->nume_raft};
        if(obtineNumarCartiFiziceRaft(loc) > 0) return false; // Nu stergem rafturi cu carti
        rafturi.erase(it);
        return true;
    }
    return false;
}

bool Biblioteca::mutaCarte(const std::string& isbn, const Locatie& loc_noua) {
    if(!existaRaft(loc_noua)) return false;
    auto c = gasesteCarte(isbn);
    if(!c || c->getTipFormat() != 0) return false;
    auto cf = std::dynamic_pointer_cast<CarteFizica>(c);
    
    int stoc_curent = obtineNumarCartiFiziceRaft(loc_noua);
    if(stoc_curent + cf->getStocDisponibil() > capacitateRaft(loc_noua)) return false; // Not enough space
    
    cf->setLocatieRaft(loc_noua);
    return true;
}

std::vector<std::string> Biblioteca::getCategoriiDistincte() const {
    std::vector<std::string> cats;
    for (const auto& c : carti) {
        if (std::find(cats.begin(), cats.end(), c->getCategorie()) == cats.end()) {
            cats.push_back(c->getCategorie());
        }
    }
    // Add "Altele" if not present
    if (std::find(cats.begin(), cats.end(), "Altele") == cats.end()) {
        cats.push_back("Altele");
    }
    return cats;
}
