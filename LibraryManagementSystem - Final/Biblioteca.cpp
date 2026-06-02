#include "Biblioteca.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <cstdio>

#include <sqlite3.h>

Biblioteca::Biblioteca(const std::string& fisier_db) : fisier_db(fisier_db), buget(0.0), offset_timp(0) {
      
    if(sqlite3_open(fisier_db.c_str(), &db)) {
        std::cerr << "Eroare la deschiderea bazei de date SQL: " << sqlite3_errmsg(db) << "\n";
    }
    incarcaDateDinSQL(); // Incarca mai intai din baza de date
    incarcaDate(); // Apoi auto-genereaza rafturi daca lipsesc
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
            std::string nume_cititor = (const char*)sqlite3_column_text(stmt, 3);
            std::string data_imp = (const char*)sqlite3_column_text(stmt, 4);
            std::string termen = (const char*)sqlite3_column_text(stmt, 5);
            std::string obs = sqlite3_column_text(stmt, 6) ? (const char*)sqlite3_column_text(stmt, 6) : "";
            
            imprumuturi.push_back(Imprumut(isbn, id_cititor, nume_cititor, data_imp, termen, obs));
        }
        sqlite3_finalize(stmt);
    }

    // Rafturi
    if(sqlite3_prepare_v2(db, "SELECT * FROM Rafturi;", -1, &stmt, nullptr) == SQLITE_OK) {
        while(sqlite3_step(stmt) == SQLITE_ROW) {
            std::string c_cladire = (const char*)sqlite3_column_text(stmt, 0);
            std::string c_camera = (const char*)sqlite3_column_text(stmt, 1);
            std::string c_culoar = (const char*)sqlite3_column_text(stmt, 2);
            std::string c_nume = (const char*)sqlite3_column_text(stmt, 3);
            std::string c_id = c_cladire + "-" + c_camera + "-" + c_culoar + "-" + c_nume;
            rafturi.push_back({
                c_id, c_cladire, c_camera, c_culoar, c_nume, sqlite3_column_int(stmt, 5)
            });
        }
        sqlite3_finalize(stmt);
    }
    
    // ReturnariInAsteptare
    if(sqlite3_prepare_v2(db, "SELECT * FROM ReturnariInAsteptare;", -1, &stmt, nullptr) == SQLITE_OK) {
        while(sqlite3_step(stmt) == SQLITE_ROW) {
            ReturnareInAsteptare r;
            r.isbn = (const char*)sqlite3_column_text(stmt, 0);
            r.id_cititor = (const char*)sqlite3_column_text(stmt, 1);
            r.nume_cititor = (const char*)sqlite3_column_text(stmt, 2);
            r.titlu_carte = (const char*)sqlite3_column_text(stmt, 3);
            r.data_returnare = (const char*)sqlite3_column_text(stmt, 4);
            r.amenda_intarziere = sqlite3_column_double(stmt, 5);
            returnari_in_asteptare.push_back(r);
        }
        sqlite3_finalize(stmt);
    }
    
    // PlatiInAsteptare
    if(sqlite3_prepare_v2(db, "SELECT * FROM PlatiInAsteptare;", -1, &stmt, nullptr) == SQLITE_OK) {
        while(sqlite3_step(stmt) == SQLITE_ROW) {
            PlataInAsteptare p;
            p.id_cititor = (const char*)sqlite3_column_text(stmt, 0);
            p.nume_cititor = (const char*)sqlite3_column_text(stmt, 1);
            p.suma = sqlite3_column_double(stmt, 2);
            p.data_solicitare = (const char*)sqlite3_column_text(stmt, 3);
            plati_in_asteptare.push_back(p);
        }
        sqlite3_finalize(stmt);
    }
    
    // StocDefect
    if(sqlite3_prepare_v2(db, "SELECT * FROM StocDefect;", -1, &stmt, nullptr) == SQLITE_OK) {
        while(sqlite3_step(stmt) == SQLITE_ROW) {
            stoc_defect[(const char*)sqlite3_column_text(stmt, 0)] = sqlite3_column_int(stmt, 1);
        }
        sqlite3_finalize(stmt);
    }
    
    // Rezervari
    if(sqlite3_prepare_v2(db, "SELECT * FROM Rezervari;", -1, &stmt, nullptr) == SQLITE_OK) {
        while(sqlite3_step(stmt) == SQLITE_ROW) {
            Rezervare r;
            r.isbn = (const char*)sqlite3_column_text(stmt, 0);
            r.id_cititor = (const char*)sqlite3_column_text(stmt, 1);
            r.nume_cititor = (const char*)sqlite3_column_text(stmt, 2);
            r.data_rezervare = (const char*)sqlite3_column_text(stmt, 3);
            r.data_expirare = (const char*)sqlite3_column_text(stmt, 4);
            r.alocata = sqlite3_column_int(stmt, 5) == 1;
            rezervari.push_back(r);
        }
        sqlite3_finalize(stmt);
    }
    
    // IstoricImprumuturi
    if(sqlite3_prepare_v2(db, "SELECT * FROM IstoricImprumuturi;", -1, &stmt, nullptr) == SQLITE_OK) {
        while(sqlite3_step(stmt) == SQLITE_ROW) {
            InregistrareIstoric i;
            i.isbn = (const char*)sqlite3_column_text(stmt, 0);
            i.titlu_carte = (const char*)sqlite3_column_text(stmt, 1);
            i.id_cititor = (const char*)sqlite3_column_text(stmt, 2);
            i.data_imprumut = (const char*)sqlite3_column_text(stmt, 3);
            i.data_returnare = (const char*)sqlite3_column_text(stmt, 4);
            istoric_lectura.push_back(i);
        }
        sqlite3_finalize(stmt);
    }
    
    // Recenzii
    if(sqlite3_prepare_v2(db, "SELECT * FROM Recenzii;", -1, &stmt, nullptr) == SQLITE_OK) {
        while(sqlite3_step(stmt) == SQLITE_ROW) {
            Recenzie r;
            r.isbn = (const char*)sqlite3_column_text(stmt, 0);
            r.id_cititor = (const char*)sqlite3_column_text(stmt, 1);
            r.nume_cititor = (const char*)sqlite3_column_text(stmt, 2);
            r.nota = sqlite3_column_int(stmt, 3);
            r.text = (const char*)sqlite3_column_text(stmt, 4);
            r.data = (const char*)sqlite3_column_text(stmt, 5);
            recenzii.push_back(r);
        }
        sqlite3_finalize(stmt);
    }
    
    // RezervariSali
    if(sqlite3_prepare_v2(db, "SELECT * FROM RezervariSali;", -1, &stmt, nullptr) == SQLITE_OK) {
        while(sqlite3_step(stmt) == SQLITE_ROW) {
            RezervareStudiu r;
            r.id_cititor = (const char*)sqlite3_column_text(stmt, 0);
            r.nume_cititor = (const char*)sqlite3_column_text(stmt, 1);
            r.sala = (const char*)sqlite3_column_text(stmt, 2);
            r.data = (const char*)sqlite3_column_text(stmt, 3);
            r.interval_orar = (const char*)sqlite3_column_text(stmt, 4);
            rezervari_sali.push_back(r);
        }
        sqlite3_finalize(stmt);
    }
    
    // Notificari
    if(sqlite3_prepare_v2(db, "SELECT * FROM Notificari;", -1, &stmt, nullptr) == SQLITE_OK) {
        while(sqlite3_step(stmt) == SQLITE_ROW) {
            Notificare n;
            n.id_destinatar = (const char*)sqlite3_column_text(stmt, 0);
            n.mesaj = (const char*)sqlite3_column_text(stmt, 1);
            n.data = (const char*)sqlite3_column_text(stmt, 2);
            n.citita = sqlite3_column_int(stmt, 3) == 1;
            notificari.push_back(n);
        }
        sqlite3_finalize(stmt);
    }
}

void Biblioteca::incarcaDate() {
    if(rafturi.empty()) {
        std::map<std::string, Locatie> catToLoc;
        int raftCounter = 1;
        
        for(const auto& c : carti) {
            if(c->getTipFormat() == 0) {
                auto cf = std::dynamic_pointer_cast<CarteFizica>(c);
                if(cf) {
                    Locatie loc = cf->getLocatieRaft();
                    if(!loc.cladire.empty()) {
                        if(!existaRaft(loc)) {
                            std::string id = loc.cladire + "-" + loc.camera + "-" + loc.culoar + "-" + loc.raft;
                            rafturi.push_back({id, loc.cladire, loc.camera, loc.culoar, loc.raft, 50});
                        }
                    } else {
                        // Create a shelf for this category if it doesn't exist
                        std::string cat = c->getCategorie();
                        if(cat.empty()) cat = "Diverse";
                        
                        if(catToLoc.find(cat) == catToLoc.end()) {
                            Locatie newLoc = {"Centrala", "Sala Lectura", "Culoar A", "Raft " + std::to_string(raftCounter++)};
                            catToLoc[cat] = newLoc;
                            std::string id = newLoc.cladire + "-" + newLoc.camera + "-" + newLoc.culoar + "-" + newLoc.raft;
                            rafturi.push_back({id, newLoc.cladire, newLoc.camera, newLoc.culoar, newLoc.raft, 100});
                        }
                        
                        cf->setLocatieRaft(catToLoc[cat]);
                    }
                }
            }
        }
        
        if(!rafturi.empty()) {
            salveazaDate(); // Salveaza modificarile de locatie in DB
        }
    }
    genereazaNotificariExpirari();
    verificaRezervariExpirate();
}

void Biblioteca::salveazaDate() const {
    char* errMsg = nullptr;
    sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, &errMsg);
    
    sqlite3_exec(db, "DELETE FROM Carti; DELETE FROM Utilizatori; DELETE FROM Imprumuturi; DELETE FROM Rafturi; DELETE FROM ReturnariInAsteptare; DELETE FROM PlatiInAsteptare; DELETE FROM StocDefect; DELETE FROM Rezervari; DELETE FROM IstoricImprumuturi; DELETE FROM Recenzii; DELETE FROM RezervariSali; DELETE FROM Notificari;", nullptr, nullptr, &errMsg);
    
    auto esc = [](const std::string& s) {
        std::string r;
        for(char ch : s) { if(ch == '\'') r += "''"; else r += ch; }
        return r;
    };
    
    // Insert Carti
    auto insertCarteSQL = [&](const std::shared_ptr<Carte>& c, int deleted) {
        std::string tip = std::to_string(c->getTipFormat());
        std::string stare = c->getStareDefecta() ? "1" : "0";
        std::string link = "", narator_str = "", cladire = "", camera = "", culoar = "", raft = "";
        int dim = 0, durata = 0;
        if(c->getTipFormat() == 0) {
            auto cf = std::dynamic_pointer_cast<CarteFizica>(c);
            if(cf) {
                Locatie loc = cf->getLocatieRaft();
                cladire = loc.cladire; camera = loc.camera; culoar = loc.culoar; raft = loc.raft;
            }
        } else if(c->getTipFormat() == 1) {
            auto cd = std::dynamic_pointer_cast<CarteDigitala>(c);
            if(cd) { link = cd->getLinkAcces(); dim = (int)cd->getDimensiuneMB(); }
        } else if(c->getTipFormat() == 2) {
            auto ca = std::dynamic_pointer_cast<CarteAudio>(c);
            if(ca) { link = ca->getLinkAcces(); durata = ca->getDurataMinute(); narator_str = ca->getNarator(); }
        }
        std::string autori_str = c->autoriToString();
        std::string q = "INSERT INTO Carti (isbn, titlu, autori, editura, pret_intrare, categorie, an_aparitie, nr_pagini, stoc_disponibil, stoc_defect, data_adaugarii, contor_imprumuturi, tip_format, stare_defecta, link_acces, dimensiune_kb, is_deleted, durata_minute, narator, cladire, camera, culoar, raft) VALUES ('" +
            esc(c->getIsbn()) + "', '" + esc(c->getTitlu()) + "', '" + esc(autori_str) + "', '" + esc(c->getEditura()) + "', " + std::to_string(c->getPretIntrare()) + ", '" +
            esc(c->getCategorie()) + "', " + std::to_string(c->getAnAparitie()) + ", " + std::to_string(c->getNrPagini()) + ", " +
            std::to_string(c->getStocDisponibil()) + ", 0, '" + esc(c->getDataAdaugarii()) + "', " + std::to_string(c->getContorImprumuturi()) + ", " + tip + ", " + stare + ", '" + esc(link) + "', " + std::to_string(dim) + ", " + std::to_string(deleted) + ", " + std::to_string(durata) + ", '" + esc(narator_str) + "', '" + esc(cladire) + "', '" + esc(camera) + "', '" + esc(culoar) + "', '" + esc(raft) + "');";
        sqlite3_exec(db, q.c_str(), nullptr, nullptr, nullptr);
    };
    for(const auto& c : carti) insertCarteSQL(c, 0);
    for(const auto& c : carti_sterse) insertCarteSQL(c, 1);
    
    // Imprumuturi
    for(const auto& imp : imprumuturi) {
        std::string q = "INSERT INTO Imprumuturi (isbn, id_cititor, nume_cititor, data_imprumut, termen_limita, observatii) VALUES ('" +
            esc(imp.getIdCarte()) + "', '" + esc(imp.getIdCititor()) + "', '" + esc(imp.getNumeCititor()) + "', '" + 
            esc(imp.getDataImprumut()) + "', '" + esc(imp.getTermenLimita()) + "', '" + esc(imp.getObservatii()) + "');";
        sqlite3_exec(db, q.c_str(), nullptr, nullptr, nullptr);
    }
    
    // Utilizatori
    auto insertUtilSQL = [&](const std::shared_ptr<Utilizator>& u, int deleted) {
        std::string tip = u->getTip();
        std::string dep = "", angajare = "", orar = "", abonament = "";
        double sal = 0.0, pen = 0.0;
        int limita = 0, curent = 0;
        
        if(tip == "DIRECTOR") {
            auto d = std::dynamic_pointer_cast<Director>(u);
            if(d) { dep = d->getDepartament(); sal = d->getSalariu(); angajare = d->getDataAngajare(); orar = d->getBirou(); }
        } else if(tip == "BIBLIOTECAR") {
            auto b = std::dynamic_pointer_cast<Bibliotecar>(u);
            if(b) { dep = b->getSectie(); sal = b->getSalariu(); angajare = b->getDataAngajare(); orar = b->getProgramLucru(); }
        } else if(tip == "INGRIJITOR") {
            auto i = std::dynamic_pointer_cast<Ingrijitor>(u);
            if(i) { dep = i->getZonaResponsabilitate(); sal = i->getSalariu(); angajare = i->getDataAngajare(); orar = i->getProgramLucru(); }
        } else if(tip == "CITITOR") {
            auto c = std::dynamic_pointer_cast<Cititor>(u);
            if(c) { abonament = c->getTipAbonament(); angajare = c->getDataInregistrare(); orar = c->getDataExpirare(); limita = c->getMaxCartiPermise(); curent = c->getNrCartiImprumutate(); pen = c->getPenalizari(); }
        }
        
        std::string q = "INSERT INTO Utilizatori (id, tip, parola, nume, prenume, cnp, email, telefon, adresa, departament, salariu, data_angajare, orar, tip_abonament, data_inregistrare, data_expirare, limita_imprumuturi, imprumuturi_curente, penalizari, is_deleted) VALUES ('" +
            esc(u->getId()) + "', '" + esc(tip) + "', '" + esc(u->getParola()) + "', '" + esc(u->getNume()) + "', '" + esc(u->getPrenume()) + "', '" + esc(u->getCnp()) + "', '" +
            esc(u->getEmail()) + "', '" + esc(u->getTelefon()) + "', '" + esc(u->getAdresa()) + "', '" + esc(dep) + "', " + std::to_string(sal) + ", '" + esc(angajare) + "', '" + esc(orar) + "', '" +
            esc(abonament) + "', '" + esc(angajare) + "', '" + esc(orar) + "', " + std::to_string(limita) + ", " + std::to_string(curent) + ", " + std::to_string(pen) + ", " + std::to_string(deleted) + ");";
        sqlite3_exec(db, q.c_str(), nullptr, nullptr, nullptr);
    };
    for(const auto& u : utilizatori) insertUtilSQL(u, 0);
    for(const auto& u : utilizatori_stersi) insertUtilSQL(u, 1);
    
    // Rafturi
    for(const auto& r : rafturi) {
        std::string q = "INSERT INTO Rafturi (cladire, camera, culoar, nume_raft, categorie_alocata, capacitate) VALUES ('" +
            esc(r.cladire) + "', '" + esc(r.camera) + "', '" + esc(r.culoar) + "', '" + esc(r.nume_raft) + "', '', " + std::to_string(r.capacitate_maxima) + ");";
        sqlite3_exec(db, q.c_str(), nullptr, nullptr, nullptr);
    }
    
    // ReturnariInAsteptare
    for(const auto& r : returnari_in_asteptare) {
        std::string q = "INSERT INTO ReturnariInAsteptare (isbn, id_cititor, nume_cititor, titlu_carte, data_returnare, amenda_intarziere) VALUES ('" +
            esc(r.isbn) + "', '" + esc(r.id_cititor) + "', '" + esc(r.nume_cititor) + "', '" + esc(r.titlu_carte) + "', '" + esc(r.data_returnare) + "', " + std::to_string(r.amenda_intarziere) + ");";
        sqlite3_exec(db, q.c_str(), nullptr, nullptr, nullptr);
    }
    
    // PlatiInAsteptare
    for(const auto& p : plati_in_asteptare) {
        std::string q = "INSERT INTO PlatiInAsteptare (id_cititor, nume_cititor, suma, data_solicitare) VALUES ('" +
            esc(p.id_cititor) + "', '" + esc(p.nume_cititor) + "', " + std::to_string(p.suma) + ", '" + esc(p.data_solicitare) + "');";
        sqlite3_exec(db, q.c_str(), nullptr, nullptr, nullptr);
    }
    
    // StocDefect
    for(const auto& pair : stoc_defect) {
        if(pair.second > 0) {
            std::string q = "INSERT INTO StocDefect (isbn, cantitate) VALUES ('" + esc(pair.first) + "', " + std::to_string(pair.second) + ");";
            sqlite3_exec(db, q.c_str(), nullptr, nullptr, nullptr);
        }
    }
    
    // Rezervari
    for(const auto& r : rezervari) {
        std::string q = "INSERT INTO Rezervari (isbn, id_cititor, nume_cititor, data_rezervare, data_expirare, alocata) VALUES ('" +
            esc(r.isbn) + "', '" + esc(r.id_cititor) + "', '" + esc(r.nume_cititor) + "', '" + esc(r.data_rezervare) + "', '" + esc(r.data_expirare) + "', " + (r.alocata ? "1" : "0") + ");";
        sqlite3_exec(db, q.c_str(), nullptr, nullptr, nullptr);
    }
    
    // IstoricImprumuturi
    for(const auto& i : istoric_lectura) {
        std::string q = "INSERT INTO IstoricImprumuturi (isbn, titlu, id_cititor, data_imprumut, data_retur) VALUES ('" +
            esc(i.isbn) + "', '" + esc(i.titlu_carte) + "', '" + esc(i.id_cititor) + "', '" + esc(i.data_imprumut) + "', '" + esc(i.data_returnare) + "');";
        sqlite3_exec(db, q.c_str(), nullptr, nullptr, nullptr);
    }
    
    // Recenzii
    for(const auto& r : recenzii) {
        std::string q = "INSERT INTO Recenzii (isbn, id_cititor, nume_cititor, rating, comentariu, data) VALUES ('" +
            esc(r.isbn) + "', '" + esc(r.id_cititor) + "', '" + esc(r.nume_cititor) + "', " + std::to_string(r.nota) + ", '" + esc(r.text) + "', '" + esc(r.data) + "');";
        sqlite3_exec(db, q.c_str(), nullptr, nullptr, nullptr);
    }
    
    // RezervariSali
    for(const auto& rs : rezervari_sali) {
        std::string q = "INSERT INTO RezervariSali (id_cititor, nume_cititor, id_sala, data, interval_orar) VALUES ('" +
            esc(rs.id_cititor) + "', '" + esc(rs.nume_cititor) + "', '" + esc(rs.sala) + "', '" + esc(rs.data) + "', '" + esc(rs.interval_orar) + "');";
        sqlite3_exec(db, q.c_str(), nullptr, nullptr, nullptr);
    }
    
    // Notificari
    for(const auto& n : notificari) {
        std::string q = "INSERT INTO Notificari (id_cititor, mesaj, data_ora, citita) VALUES ('" +
            esc(n.id_destinatar) + "', '" + esc(n.mesaj) + "', '" + esc(n.data) + "', " + (n.citita ? "1" : "0") + ");";
        sqlite3_exec(db, q.c_str(), nullptr, nullptr, nullptr);
    }
    
    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, &errMsg);
}

double Biblioteca::getBuget() const {
    return buget;
}

double Biblioteca::calculeazaSalariiTotale() const {
    double total = 0;
    for (const auto& u : utilizatori) {
        if(u->getTip() == "DIRECTOR") {
            auto d = std::dynamic_pointer_cast<Director>(u);
            if(d) total += d->getSalariu();
        } else if(u->getTip() == "BIBLIOTECAR") {
            auto b = std::dynamic_pointer_cast<Bibliotecar>(u);
            if(b) total += b->getSalariu();
        } else if(u->getTip() == "INGRIJITOR") {
            auto i = std::dynamic_pointer_cast<Ingrijitor>(u);
            if(i) total += i->getSalariu();
        }
    }
    return total;
}

std::string Biblioteca::platesteSalarii() {
    time_t acum = getVirtualTime();
    double diferenta_secunde = difftime(acum, ultima_plata_salarii);
    int zile_trecute = diferenta_secunde / (24 * 3600);
    int cicluri = zile_trecute / 14; // Un ciclu = 2 saptamani
    
    if (cicluri == 0) {
        return "Nu au trecut 14 zile de la ultima plată. Au trecut doar " + std::to_string(zile_trecute) + " zile.";
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
    
    return "S-au plătit salariile pentru " + std::to_string(cicluri) + " cicluri (14 zile/ciclu). Bugetul a fost actualizat.";
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
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db, "SELECT zi_curenta FROM Sistem WHERE id = 1;", -1, &stmt, nullptr) == SQLITE_OK) {
        if(sqlite3_step(stmt) == SQLITE_ROW) {
            int zi = sqlite3_column_int(stmt, 0);
            offset_timp = zi * 86400;
        } else {
            offset_timp = 0;
        }
        sqlite3_finalize(stmt);
    } else {
        offset_timp = 0;
    }
    ultima_plata_salarii = time(nullptr) + offset_timp;
}

void Biblioteca::salveazaTimp() const {
    int zi = offset_timp / 86400;
    std::string q = "UPDATE Sistem SET zi_curenta = " + std::to_string(zi) + " WHERE id = 1;";
    sqlite3_exec(db, q.c_str(), nullptr, nullptr, nullptr);
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
        std::string titlu = "", nume = "";
        auto c = gasesteCarte(isbn);
        if(c) titlu = c->getTitlu();
        auto u = gasesteUtilizator(id_cititor);
        if(u) nume = u->getNumeComplet();
        
        double amenda = 0.0;
        int zile = calculeazaZileIntarziere(it->getTermenLimita());
        if (zile > 0) amenda = zile * 1.0;
        
        ReturnareInAsteptare r = { isbn, id_cititor, nume, titlu, getDataCurentaStr(), amenda };
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

void Biblioteca::scrieLaLog(const std::string& actor, const std::string& actiune) const {
    auto esc = [](const std::string& s) {
        std::string r;
        for(char ch : s) { if(ch == '\'') r += "''"; else r += ch; }
        return r;
    };
    std::string data = getDataCurentaStr();
    std::string q = "INSERT INTO JurnalAudit (data_ora, id_utilizator, actiune) VALUES ('" + esc(data) + "', '" + esc(actor) + "', '" + esc(actiune) + "');";
    sqlite3_exec(db, q.c_str(), nullptr, nullptr, nullptr);
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
    utilizatori.push_back(std::make_shared<Director>(id, Utilizator::xorEncode(parola), nume, prenume, cnp, email, telefon, adresa, true, departament, salariu, data_angajare, birou, nivel_acces));
}

void Biblioteca::adaugaBibliotecar(const std::string& id, const std::string& parola, const std::string& nume, const std::string& prenume,
                       const std::string& cnp, const std::string& email, const std::string& telefon,
                       const std::string& adresa,
                       const std::string& sectie, double salariu,
                       const std::string& data_angajare, const std::string& program_lucru,
                       int nr_carti_gestionate) {
    utilizatori.push_back(std::make_shared<Bibliotecar>(id, Utilizator::xorEncode(parola), nume, prenume, cnp, email, telefon, adresa, true, sectie, salariu, data_angajare, program_lucru, nr_carti_gestionate));
}

void Biblioteca::adaugaIngrijitor(const std::string& id, const std::string& parola, const std::string& nume, const std::string& prenume,
                      const std::string& cnp, const std::string& email, const std::string& telefon,
                      const std::string& adresa,
                      const std::string& zona, double salariu,
                      const std::string& data_angajare, const std::string& program_lucru,
                      const std::string& echipament) {
    utilizatori.push_back(std::make_shared<Ingrijitor>(id, Utilizator::xorEncode(parola), nume, prenume, cnp, email, telefon, adresa, true, zona, salariu, data_angajare, program_lucru, echipament));
}

void Biblioteca::adaugaCititor(const std::string& id, const std::string& parola, const std::string& nume, const std::string& prenume,
                   const std::string& cnp, const std::string& email, const std::string& telefon,
                   const std::string& adresa,
                   const std::string& tip_abonament, const std::string& data_inregistrare,
                   const std::string& data_expirare, int max_carti) {
    auto u = std::make_shared<Cititor>(id, Utilizator::xorEncode(parola), nume, prenume, cnp, email, telefon, adresa, true, tip_abonament, data_inregistrare, data_expirare, max_carti, 0, 0.0);
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
    os << "┌─────────────────────┬──────────────────────┬────────────────────────────────────────────────────────────┐\n";
    os << "│ DATA SI ORA         │ UTILIZATOR           │ ACTIUNE                                                    │\n";
    os << "├─────────────────────┼──────────────────────┼────────────────────────────────────────────────────────────┤\n";

    sqlite3_stmt* stmt;
    std::string query = "SELECT data_ora, id_utilizator, actiune FROM JurnalAudit ORDER BY id DESC LIMIT " + std::to_string(linii) + ";";
    if(sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while(sqlite3_step(stmt) == SQLITE_ROW) {
            std::string data = (const char*)sqlite3_column_text(stmt, 0);
            std::string usr = (const char*)sqlite3_column_text(stmt, 1);
            std::string act = (const char*)sqlite3_column_text(stmt, 2);
            os << "│ " << std::setw(19) << data 
               << " │ " << std::setw(20) << usr 
               << " │ " << std::setw(58) << act.substr(0, 58) << " │\n";
        }
        sqlite3_finalize(stmt);
    }
    os << "└─────────────────────┴──────────────────────┴────────────────────────────────────────────────────────────┘\n";
}

std::vector<JurnalAuditRow> Biblioteca::getJurnalAudit(int linii) const {
    std::vector<JurnalAuditRow> rezultat;
    sqlite3_stmt* stmt;
    std::string query = "SELECT data_ora, id_utilizator, actiune FROM JurnalAudit ORDER BY id DESC LIMIT " + std::to_string(linii) + ";";
    if(sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while(sqlite3_step(stmt) == SQLITE_ROW) {
            std::string data = (const char*)sqlite3_column_text(stmt, 0);
            std::string usr = (const char*)sqlite3_column_text(stmt, 1);
            std::string act = (const char*)sqlite3_column_text(stmt, 2);
            rezultat.push_back({data, usr, act});
        }
        sqlite3_finalize(stmt);
    }
    return rezultat;
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


std::shared_ptr<Utilizator> Biblioteca::autentificare(const std::string& id_or_email, const std::string& parola) const {
    auto u = gasesteUtilizator(id_or_email);
    if (!u) {
        // Caută după email
        for (const auto& util : utilizatori) {
            if (util->getEmail() == id_or_email) {
                u = util;
                break;
            }
        }
    }

    if(u && u->verificaParola(parola) && u->getActiv()) {
        scrieLaLog(u->getId(), "AUTENTIFICARE: " + u->getRol());
        return u;
    }
    return nullptr;
}

void Biblioteca::salveazaBuget() const {
    std::string q = "UPDATE Sistem SET buget = " + std::to_string(buget) + " WHERE id = 1;";
    sqlite3_exec(db, q.c_str(), nullptr, nullptr, nullptr);
}

void Biblioteca::incarcaBuget() {
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(db, "SELECT buget FROM Sistem WHERE id = 1;", -1, &stmt, nullptr) == SQLITE_OK) {
        if(sqlite3_step(stmt) == SQLITE_ROW) {
            buget = sqlite3_column_double(stmt, 0);
        } else {
            buget = 0.0;
        }
        sqlite3_finalize(stmt);
    } else {
        buget = 0.0;
    }
}

bool Biblioteca::modificaUtilizator(const std::string& id, const std::string& noua_parola, const std::string& noul_nume, const std::string& noul_prenume, const std::string& noul_email, const std::string& noul_telefon, const std::string& noua_adresa, const std::string& id_actor) {
    auto u = gasesteUtilizator(id);
    if(u) {
        if(!noua_parola.empty()) u->setParola(noua_parola);
        if(!noul_nume.empty()) u->setNume(noul_nume);
        if(!noul_prenume.empty()) u->setPrenume(noul_prenume);
        if(!noul_email.empty()) u->setEmail(noul_email);
        if(!noul_telefon.empty()) u->setTelefon(noul_telefon);
        if(!noua_adresa.empty()) u->setAdresa(noua_adresa);
        if(!id_actor.empty()) scrieLaLog(id_actor, "MODIFICARE UTILIZATOR: " + id);
        return true;
    }
    return false;
}

bool Biblioteca::modificaCarte(const std::string& isbn, const std::string& noul_titlu, const std::vector<std::string>& noii_autori, const std::string& noua_editura, double noul_pret, const std::string& noua_categorie, int noul_an, int noile_pagini, const std::string& id_actor) {
    auto c = gasesteCarte(isbn);
    if(c) {
        if(!noul_titlu.empty()) c->setTitlu(noul_titlu);
        if(!noii_autori.empty()) c->setAutori(noii_autori);
        if(!noua_editura.empty()) c->setEditura(noua_editura);
        if(noul_pret > 0) c->setPretIntrare(noul_pret);
        if(!noua_categorie.empty()) c->setCategorie(noua_categorie);
        if(noul_an > 0) c->setAnAparitie(noul_an);
        if(noile_pagini > 0) c->setNrPagini(noile_pagini);
        if(!id_actor.empty()) scrieLaLog(id_actor, "MODIFICARE CARTE: " + isbn);
        return true;
    }
    return false;
}

bool Biblioteca::stergeCarte(const std::string& isbn, const std::string& id_actor) {
    auto it = std::find_if(carti.begin(), carti.end(), [&](const std::shared_ptr<Carte>& c) { return c->getIsbn() == isbn; });
    if(it != carti.end()) {
        carti_sterse.push_back(*it);
        carti.erase(it);
        if(!id_actor.empty()) scrieLaLog(id_actor, "STERGERE CARTE: " + isbn);
        return true;
    }
    return false;
}

bool Biblioteca::restaureazaCarte(const std::string& isbn, const std::string& id_actor) {
    auto it = std::find_if(carti_sterse.begin(), carti_sterse.end(), [&](const std::shared_ptr<Carte>& c) { return c->getIsbn() == isbn; });
    if(it != carti_sterse.end()) {
        carti.push_back(*it);
        carti_sterse.erase(it);
        if(!id_actor.empty()) scrieLaLog(id_actor, "RESTAURARE CARTE: " + isbn);
        return true;
    }
    return false;
}

bool Biblioteca::restaureazaUtilizator(const std::string& id, const std::string& id_actor) {
    auto it = std::find_if(utilizatori_stersi.begin(), utilizatori_stersi.end(), [&](const std::shared_ptr<Utilizator>& u) { return u->getId() == id; });
    if(it != utilizatori_stersi.end()) {
        utilizatori.push_back(*it);
        utilizatori_stersi.erase(it);
        if(!id_actor.empty()) scrieLaLog(id_actor, "RESTAURARE UTILIZATOR: " + id);
        return true;
    }
    return false;
}

void Biblioteca::adaugaCarteDigitala(const std::string& isbn, const std::string& titlu, const std::vector<std::string>& autori,
                         const std::string& editura, double pret, const std::string& categorie, int an_aparitie, int nr_pagini,
                         const std::string& data_adaugarii, int contor_imprumuturi,
                         const std::string& format, double dimensiune, const std::string& link) {
    carti.push_back(std::make_shared<CarteDigitala>(isbn, titlu, autori, editura, pret, categorie, an_aparitie, nr_pagini, data_adaugarii, contor_imprumuturi, format, dimensiune, link));
}

void Biblioteca::adaugaCarteAudio(const std::string& isbn, const std::string& titlu, const std::vector<std::string>& autori,
                      const std::string& editura, double pret, const std::string& categorie, int an_aparitie, int nr_pagini,
                      const std::string& data_adaugarii, int contor_imprumuturi,
                      int durata_minute, const std::string& narator, const std::string& link) {
    carti.push_back(std::make_shared<CarteAudio>(isbn, titlu, autori, editura, pret, categorie, an_aparitie, nr_pagini, data_adaugarii, contor_imprumuturi, durata_minute, narator, link));
}

bool Biblioteca::adaugaCarteFizica(const std::string& isbn, const std::string& titlu, const std::vector<std::string>& autori,
                       const std::string& editura, double pret, const std::string& categorie, int an_aparitie, int nr_pagini,
                       const std::string& data_adaugarii, int contor_imprumuturi,
                       int stoc_disponibil, const Locatie& loc, const std::string& coperta, bool stare_defecta) {
    carti.push_back(std::make_shared<CarteFizica>(isbn, titlu, autori, editura, pret, categorie, an_aparitie, nr_pagini, data_adaugarii, contor_imprumuturi, stoc_disponibil, loc, coperta, stare_defecta));
    return true;
}
