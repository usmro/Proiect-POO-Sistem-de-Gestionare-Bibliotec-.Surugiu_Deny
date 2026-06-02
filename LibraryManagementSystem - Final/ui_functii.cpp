#include "ui_functii.h"
#include <iostream>
#include <random>
#include <ctime>

void pausare() {
    ConsoleUI::showToast("Apasati orice tasta pentru a continua...", ConsoleUI::Yellow);
    ConsoleUI::getKeyPress();
}

static std::string getDataCurenta(const Biblioteca& bib) {
    time_t rawtime = bib.getVirtualTime();
    struct tm * timeinfo = localtime(&rawtime);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y", timeinfo);
    return std::string(buffer);
}

static std::string getDataCurentaPlus(const Biblioteca& bib, int zile) {
    time_t rawtime = bib.getVirtualTime();
    rawtime += zile * 24 * 60 * 60;
    struct tm * timeinfo = localtime(&rawtime);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y", timeinfo);
    return std::string(buffer);
}

static std::string generateISBN() {
    static std::mt19937 gen(time(nullptr));
    std::uniform_int_distribution<> dis(1000000, 9999999);
    return "978-973-" + std::to_string(dis(gen));
}

static std::string generateUserId(const std::string& prefix) {
    static std::mt19937 gen(time(nullptr));
    std::uniform_int_distribution<> dis(100, 999);
    return prefix + std::to_string(dis(gen));
}

void adaugaCarteFizicaUI(Biblioteca& bib) {
    ConsoleUI::clearScreen();
    ConsoleUI::drawBox(2, 2, 85, 22, "Adaugă Carte Fizică (Rapid)");
    std::string titlu = ConsoleUI::citesteFormularLinie(4, 4, "Titlu: ", 40);
    if(titlu.empty()) return;
    std::string auto_s = ConsoleUI::citesteFormularLinie(4, 5, "Autori (,): ", 40);
    std::string editura = ConsoleUI::citesteFormularLinie(4, 6, "Editura: ", 40);
    
    time_t t = time(nullptr);
    tm* acum = localtime(&t);
    int an_curent = acum->tm_year + 1900;
    int an;
    while(true) {
        an = ConsoleUI::citesteFormularInt(4, 7, "An: ");
        if(an > 0 && an <= an_curent) break;
        ConsoleUI::showToast("❌ An invalid (viitor sau negativ)!", ConsoleUI::Red);
        ConsoleUI::clearArea(4 + 4, 7, 15, 1);
    }
    
    int pag;
    while(true) {
        pag = ConsoleUI::citesteFormularInt(4, 8, "Pagini: ");
        if(pag > 0) break;
        ConsoleUI::showToast("❌ Numar pagini invalid!", ConsoleUI::Red);
        ConsoleUI::clearArea(4 + 8, 8, 15, 1);
    }
    
    double pret;
    while(true) {
        pret = ConsoleUI::citesteFormularDouble(4, 9, "Preț (RON): ");
        if(pret > 0.0) break;
        ConsoleUI::showToast("❌ Pretul trebuie sa fie > 0!", ConsoleUI::Red);
        ConsoleUI::clearArea(4 + 12, 9, 15, 1);
    }
    
    int stoc;
    while(true) {
        stoc = ConsoleUI::citesteFormularInt(4, 10, "Stoc inițial: ");
        if(stoc >= 0) break;
        ConsoleUI::showToast("❌ Stocul trebuie sa fie >= 0!", ConsoleUI::Red);
        ConsoleUI::clearArea(4 + 14, 10, 15, 1);
    }

    int cat_opt = 0;
    std::vector<std::string> cats = bib.getCategoriiDistincte();
    cat_opt = ConsoleUI::showInteractiveMenu(45, 4, "Categorie", cats, cat_opt);
    if(cat_opt < 0) return;
    ConsoleUI::clearArea(45, 4, 38, cats.size() + 4);
    ConsoleUI::printAt(45, 4, "Categorie: " + cats[cat_opt], ConsoleUI::Cyan);
    
    int st_opt = 0;
    st_opt = ConsoleUI::showInteractiveMenu(45, 6, "Stare", {"BUNA", "DEFECTA"}, st_opt);
    if(st_opt < 0) return;
    ConsoleUI::clearArea(45, 6, 38, 6);
    ConsoleUI::printAt(45, 5, "Stare: " + std::string(st_opt == 0 ? "BUNA" : "DEFECTA"), ConsoleUI::Cyan);

    int cop_opt = 0;
    cop_opt = ConsoleUI::showInteractiveMenu(45, 7, "Copertă", {"Hardcover", "Softcover"}, cop_opt);
    if(cop_opt < 0) return;
    ConsoleUI::clearArea(45, 7, 38, 6);
    std::string coperta = (cop_opt == 0) ? "Hardcover" : "Softcover";
    ConsoleUI::printAt(45, 6, "Copertă: " + coperta, ConsoleUI::Cyan);

    int stoc_disponibil = stoc;
    const auto& rafturi = bib.getToateRafturile();
    if(rafturi.empty()) {
        ConsoleUI::showToast("❌ Eroare: Nu exista rafturi definite in sistem! Creati un raft intai.", ConsoleUI::Red);
        return;
    }
    
    std::vector<std::vector<std::string>> rafturi_str;
    for(const auto& r : rafturi) {
        Locatie temp_loc = {r.cladire, r.camera, r.culoar, r.nume_raft};
        int curent = bib.obtineNumarCartiFiziceRaft(temp_loc);
        std::string disp = (curent + stoc_disponibil <= r.capacitate_maxima) ? "[DISPONIBIL]" : "[FULL]";
        rafturi_str.push_back({r.id, std::to_string(curent) + "/" + std::to_string(r.capacitate_maxima), disp});
    }
    
    ConsoleUI::clearScreen();
    int sel_raft = ConsoleUI::showPaginatedTable(2, 2, "Selecteaza Raftul Destinatie ptr: " + titlu, {"ID Raft", "Ocupat/Capacitate", "Stare"}, rafturi_str);
    if(sel_raft < 0) return;
    
    const auto& sel_r = rafturi[sel_raft];
    Locatie loc = {sel_r.cladire, sel_r.camera, sel_r.culoar, sel_r.nume_raft};
    
    if(bib.obtineNumarCartiFiziceRaft(loc) + stoc_disponibil > sel_r.capacitate_maxima) {
        ConsoleUI::showToast("❌ Eroare: Raftul selectat nu are suficient spatiu!", ConsoleUI::Red);
        return;
    }

    std::string isbn = generateISBN();
    bool defecta = (st_opt == 1);
    bool ok = bib.adaugaCarteFizica(isbn, titlu, Carte::stringToAutori(auto_s), editura, pret, cats[cat_opt], an, pag, getDataCurenta(bib), 0, stoc, loc, coperta, defecta);
    if(ok) {
        bib.salveazaDate();
        ConsoleUI::showToast("✅ Cartea a fost adăugată! ISBN: " + isbn);
    } else {
        ConsoleUI::showToast("❌ Eroare: S-a depășit limita de 50 cărți pe raft!", ConsoleUI::Red);
    }
}

void adaugaCarteDigitalaUI(Biblioteca& bib) {
    ConsoleUI::clearScreen();
    ConsoleUI::drawBox(2, 2, 85, 20, "Adaugă Carte Digitală (Rapid)");
    std::string titlu = ConsoleUI::citesteFormularLinie(4, 4, "Titlu: ", 40);
    if(titlu.empty()) return;
    std::string auto_s = ConsoleUI::citesteFormularLinie(4, 5, "Autori (,): ", 40);
    std::string editura = ConsoleUI::citesteFormularLinie(4, 6, "Editura: ", 40);
    
    time_t t = time(nullptr);
    tm* acum = localtime(&t);
    int an_curent = acum->tm_year + 1900;
    int an;
    while(true) {
        an = ConsoleUI::citesteFormularInt(4, 7, "An: ");
        if(an > 0 && an <= an_curent) break;
        ConsoleUI::showToast("❌ An invalid (viitor sau negativ)!", ConsoleUI::Red);
        ConsoleUI::clearArea(4 + 4, 7, 15, 1);
    }
    
    int pag;
    while(true) {
        pag = ConsoleUI::citesteFormularInt(4, 8, "Pagini: ");
        if(pag > 0) break;
        ConsoleUI::showToast("❌ Numar pagini invalid!", ConsoleUI::Red);
        ConsoleUI::clearArea(4 + 8, 8, 15, 1);
    }
    
    double pret;
    while(true) {
        pret = ConsoleUI::citesteFormularDouble(4, 9, "Preț (RON): ");
        if(pret > 0.0) break;
        ConsoleUI::showToast("❌ Pretul trebuie sa fie > 0!", ConsoleUI::Red);
        ConsoleUI::clearArea(4 + 12, 9, 15, 1);
    }

    int cat_opt = 0;
    std::vector<std::string> cats = bib.getCategoriiDistincte();
    cat_opt = ConsoleUI::showInteractiveMenu(45, 4, "Categorie", cats, cat_opt);
    if(cat_opt < 0) return;
    ConsoleUI::clearArea(45, 4, 38, cats.size() + 4);
    ConsoleUI::printAt(45, 4, "Categorie: " + cats[cat_opt], ConsoleUI::Cyan);

    std::string isbn = generateISBN();
    std::string link = "http://biblio.ro/ebooks/" + isbn;
    bib.adaugaCarteDigitala(isbn, titlu, Carte::stringToAutori(auto_s), editura, pret, cats[cat_opt], an, pag, getDataCurenta(bib), 0, "PDF", 2.5, link);
    bib.salveazaDate();
    ConsoleUI::showToast("✅ E-Book adăugat! ISBN: " + isbn);
}

void adaugaCarteAudioUI(Biblioteca& bib) {
    ConsoleUI::clearScreen();
    ConsoleUI::drawBox(2, 2, 85, 22, "Adaugă Audiobook (Rapid)");
    std::string titlu = ConsoleUI::citesteFormularLinie(4, 4, "Titlu: ", 40);
    if(titlu.empty()) return;
    std::string auto_s = ConsoleUI::citesteFormularLinie(4, 5, "Autori (,): ", 40);
    std::string editura = ConsoleUI::citesteFormularLinie(4, 6, "Editura: ", 40);
    std::string narator = ConsoleUI::citesteFormularLinie(4, 7, "Narator: ", 40);
    if(narator.empty()) { ConsoleUI::showToast("❌ Naratorul este obligatoriu!", ConsoleUI::Red); return; }
    
    time_t t = time(nullptr);
    tm* acum = localtime(&t);
    int an_curent = acum->tm_year + 1900;
    int an;
    while(true) {
        an = ConsoleUI::citesteFormularInt(4, 8, "An: ");
        if(an > 0 && an <= an_curent) break;
        ConsoleUI::showToast("❌ An invalid (viitor sau negativ)!", ConsoleUI::Red);
        ConsoleUI::clearArea(4 + 4, 8, 15, 1);
    }
    
    int ore;
    while(true) {
        ore = ConsoleUI::citesteFormularInt(4, 9, "Durata ore: ");
        if(ore >= 0) break;
        ConsoleUI::showToast("❌ Ore invalide!", ConsoleUI::Red);
        ConsoleUI::clearArea(4 + 12, 9, 15, 1);
    }
    
    int minute;
    while(true) {
        minute = ConsoleUI::citesteFormularInt(4, 10, "Durata minute: ");
        if(minute >= 0 && minute < 60) break;
        ConsoleUI::showToast("❌ Minute invalide (0-59)!", ConsoleUI::Red);
        ConsoleUI::clearArea(4 + 15, 10, 15, 1);
    }
    
    int durata_totala = ore * 60 + minute;
    if(durata_totala <= 0) { ConsoleUI::showToast("❌ Durata trebuie să fie > 0!", ConsoleUI::Red); return; }
    
    double pret;
    while(true) {
        pret = ConsoleUI::citesteFormularDouble(4, 11, "Preț (RON): ");
        if(pret > 0.0) break;
        ConsoleUI::showToast("❌ Pretul trebuie sa fie > 0!", ConsoleUI::Red);
        ConsoleUI::clearArea(4 + 12, 11, 15, 1);
    }

    int cat_opt = 0;
    std::vector<std::string> cats = bib.getCategoriiDistincte();
    cat_opt = ConsoleUI::showInteractiveMenu(45, 4, "Categorie", cats, cat_opt);
    if(cat_opt < 0) return;
    ConsoleUI::clearArea(45, 4, 38, cats.size() + 4);
    ConsoleUI::printAt(45, 4, "Categorie: " + cats[cat_opt], ConsoleUI::Cyan);

    std::string isbn = generateISBN();
    std::string link = "http://biblio.ro/audiobooks/" + isbn;
    bib.adaugaCarteAudio(isbn, titlu, Carte::stringToAutori(auto_s), editura, pret, cats[cat_opt], an, 0, getDataCurenta(bib), 0, durata_totala, narator, link);
    bib.salveazaDate();
    ConsoleUI::showToast("✅ Audiobook adăugat! ISBN: " + isbn + " | Durată: " + std::to_string(ore) + "h " + std::to_string(minute) + "m");
}

void stergeCarteUI(Biblioteca& bib, const std::string& id_actor) {
    int tip_opt = 0;
    tip_opt = ConsoleUI::showInteractiveMenu(2, 2, "Alege Tip Carte", {"Toate", "Fizice", "Digitale", "Audiobook"}, tip_opt);
    if(tip_opt < 0) return;
    
    auto toate = bib.getToateCartile();
    std::vector<std::shared_ptr<Carte>> carti;
    for(const auto& c : toate) {
        if(tip_opt == 0) carti.push_back(c);
        else if(tip_opt == 1 && c->getTipFormat() == 0) carti.push_back(c);
        else if(tip_opt == 2 && c->getTipFormat() == 1) carti.push_back(c);
        else if(tip_opt == 3 && c->getTipFormat() == 2) carti.push_back(c);
    }
    
    if(carti.empty()) { ConsoleUI::showToast("❌ Nicio carte gasita!"); return; }

    std::vector<std::string> headers = {"Nr", "Titlu", "ISBN"};
    std::vector<std::vector<std::string>> rows;
    for(size_t i=0; i<carti.size(); ++i) rows.push_back({std::to_string(i+1), carti[i]->getTitlu(), carti[i]->getIsbn()});
    
    int sel = ConsoleUI::showPaginatedTable(2, 2, "Șterge Carte", headers, rows);
    if(sel >= 0 && sel < (int)carti.size()) {
        if(bib.stergeCarte(carti[sel]->getIsbn(), id_actor)) ConsoleUI::showToast("✅ Șters!");
        else ConsoleUI::showToast("❌ Eroare", ConsoleUI::Red);
    }
}

void restaureazaCarteUI(Biblioteca& bib, const std::string& id_actor) {
    auto carti = bib.getCartiSterse();
    if(carti.empty()) { ConsoleUI::showToast("❌ Nicio carte stearsa in stocare!"); return; }

    std::vector<std::string> headers = {"Nr", "Titlu", "ISBN"};
    std::vector<std::vector<std::string>> rows;
    for(size_t i=0; i<carti.size(); ++i) rows.push_back({std::to_string(i+1), carti[i]->getTitlu(), carti[i]->getIsbn()});
    
    int sel = ConsoleUI::showPaginatedTable(2, 2, "Restaurează Carte", headers, rows);
    if(sel >= 0 && sel < (int)carti.size()) {
        if(bib.restaureazaCarte(carti[sel]->getIsbn(), id_actor)) ConsoleUI::showToast("✅ Restaurată!");
        else ConsoleUI::showToast("❌ Eroare", ConsoleUI::Red);
    }
}

void modificaCarteUI(Biblioteca& bib, const std::string& id_actor) {
    int tip_opt = 0;
    tip_opt = ConsoleUI::showInteractiveMenu(2, 2, "Alege Tip Carte", {"Toate", "Fizice", "Digitale"}, tip_opt);
    if(tip_opt < 0) return;
    
    auto toate = bib.getToateCartile();
    std::vector<std::shared_ptr<Carte>> carti;
    for(const auto& c : toate) {
        if(tip_opt == 0) carti.push_back(c);
        else if(tip_opt == 1 && c->getTipFormat() == 0) carti.push_back(c);
        else if(tip_opt == 2 && c->getTipFormat() == 1) carti.push_back(c);
    }
    
    if(carti.empty()) { ConsoleUI::showToast("❌ Nicio carte gasita!"); return; }

    std::vector<std::string> headers = {"Nr", "Titlu", "ISBN"};
    std::vector<std::vector<std::string>> rows;
    for(size_t i=0; i<carti.size(); ++i) rows.push_back({std::to_string(i+1), carti[i]->getTitlu(), carti[i]->getIsbn()});
    
    int sel = ConsoleUI::showPaginatedTable(2, 2, "Modifică Carte", headers, rows);
    if(sel >= 0 && sel < (int)carti.size()) {
        ConsoleUI::clearScreen();
        ConsoleUI::drawBox(2, 2, 60, 10, "Modifică: " + carti[sel]->getTitlu());
        std::string titlu = ConsoleUI::citesteFormularLinie(4, 4, "Titlu nou: ", 40);
        if(titlu.empty()) titlu = carti[sel]->getTitlu();
        
        std::string editura = ConsoleUI::citesteFormularLinie(4, 5, "Editură nouă: ", 40);
        
        std::string pret_s = ConsoleUI::citesteFormularLinie(4, 6, "Preț nou: ", 10);
        double pret = pret_s.empty() ? carti[sel]->getPretIntrare() : std::stod(pret_s);
        
        if(bib.modificaCarte(carti[sel]->getIsbn(), titlu, {}, editura, pret, carti[sel]->getCategorie(), carti[sel]->getAnAparitie(), carti[sel]->getNrPagini(), id_actor))
            ConsoleUI::showToast("✅ Modificat!");
    }
}

void vizualizareCatalogUI(Biblioteca& bib) {
    int tip_opt = 0;
    tip_opt = ConsoleUI::showInteractiveMenu(2, 2, "Alege Tip Carte", {"Toate", "Fizice", "Digitale", "Audiobook"}, tip_opt);
    if(tip_opt < 0) return;
    
    auto toate = bib.getToateCartile();
    std::vector<std::shared_ptr<Carte>> carti;
    for(const auto& c : toate) {
        if(tip_opt == 0) carti.push_back(c);
        else if(tip_opt == 1 && c->getTipFormat() == 0) carti.push_back(c);
        else if(tip_opt == 2 && c->getTipFormat() == 1) carti.push_back(c);
        else if(tip_opt == 3 && c->getTipFormat() == 2) carti.push_back(c);
    }
    
    if(carti.empty()) { ConsoleUI::showToast("❌ Nicio carte gasita!"); return; }

    std::vector<std::string> headers = {"Nr", "Titlu", "Autor", "Stoc", "Locatie/Format"};
    std::vector<std::vector<std::string>> rows;
    for(size_t i=0; i<carti.size(); ++i) {
        std::string loc = "";
        if (carti[i]->getTipFormat() == 0) {
            auto cf = std::dynamic_pointer_cast<CarteFizica>(carti[i]);
            loc = cf->getLocatieRaft().camera + " (" + cf->getLocatieRaft().raft + ")";
        } else if (carti[i]->getTipFormat() == 2) {
            auto ca = std::dynamic_pointer_cast<CarteAudio>(carti[i]);
            loc = "Audio (" + ca->getDurataFormatata() + ")";
        } else {
            auto cd = std::dynamic_pointer_cast<CarteDigitala>(carti[i]);
            loc = "Digital (" + cd->getFormatFisier() + ")";
        }
        auto tr = [](std::string s, size_t m) { return s.length() > m ? s.substr(0, m-3) + "..." : s; };
        rows.push_back({std::to_string(i+1), tr(carti[i]->getTitlu(), 20), tr(carti[i]->autoriToString(), 15), std::to_string(carti[i]->getStocDisponibil()), tr(loc, 20)});
    }
    
    int sel = ConsoleUI::showPaginatedTable(2, 2, "Catalog Cărți", headers, rows);
    if(sel >= 0 && sel < (int)carti.size()) {
        ConsoleUI::clearScreen();
        ConsoleUI::drawBox(2, 2, 70, 15, "Detalii Carte");
        ConsoleUI::printAt(4, 4, "Titlu: " + carti[sel]->getTitlu(), ConsoleUI::Cyan, true);
        ConsoleUI::printAt(4, 5, "Autor: " + carti[sel]->autoriToString());
        ConsoleUI::printAt(4, 6, "Editură: " + carti[sel]->getEditura());
        ConsoleUI::printAt(4, 7, "ISBN: " + carti[sel]->getIsbn());
        ConsoleUI::printAt(4, 8, "Stoc disp: " + std::to_string(carti[sel]->getStocDisponibil()));
        
        int current_y = 9;
        if (carti[sel]->getTipFormat() == 0) {
            auto cf = std::dynamic_pointer_cast<CarteFizica>(carti[sel]);
            Locatie l = cf->getLocatieRaft();
            ConsoleUI::printAt(4, current_y++, "Locatie: " + l.cladire + ", " + l.camera + ", " + l.culoar + "-" + l.raft, ConsoleUI::Cyan);
        } else if (carti[sel]->getTipFormat() == 2) {
            auto ca = std::dynamic_pointer_cast<CarteAudio>(carti[sel]);
            ConsoleUI::printAt(4, current_y++, "Narator: " + ca->getNarator(), ConsoleUI::Cyan);
            ConsoleUI::printAt(4, current_y++, "Durata: " + ca->getDurataFormatata(), ConsoleUI::Cyan);
        } else {
            auto cd = std::dynamic_pointer_cast<CarteDigitala>(carti[sel]);
            ConsoleUI::printAt(4, current_y++, "Format: " + cd->getFormatFisier() + " | " + std::to_string(cd->getDimensiuneMB()) + "MB", ConsoleUI::Cyan);
        }
        
        ConsoleUI::printAt(4, current_y++, "Rating: " + std::to_string(bib.calculeazaRatingMediu(carti[sel]->getIsbn())));
        
        ConsoleUI::printAt(4, current_y++, "--- RECENZII ---", ConsoleUI::Yellow, true);
        ConsoleUI::moveTo(4, current_y++);
        bib.afiseazaRecenziiCarte(std::cout, carti[sel]->getIsbn());
        std::cout << std::endl;
        
        pausare();
    }
}

void cautaCarteUI(Biblioteca& bib) {
    int opt = 0;
    opt = ConsoleUI::showInteractiveMenu(2, 2, "Caută Carte", {"Dupa Autor", "Dupa Titlu", "Dupa Categorie", "Dupa Editura"}, opt);
    if(opt < 0) return;
    
    ConsoleUI::clearScreen();
    std::string query;
    if(opt == 2) {
        int cat_opt = 0;
        std::vector<std::string> cats = bib.getCategoriiDistincte();
        cat_opt = ConsoleUI::showInteractiveMenu(35, 11, "Alege Categoria", cats, cat_opt);
        if(cat_opt < 0) return;
        query = cats[cat_opt];
    } else {
        ConsoleUI::drawBox(2, 2, 50, 4, "Cautare");
        query = ConsoleUI::citesteFormularLinie(4, 4, "Termen: ", 30);
    }
    
    std::vector<std::shared_ptr<Carte>> rezultate;
    if(opt == 0) rezultate = bib.cautaDupaAutor(query);
    else if(opt == 1) rezultate = bib.cautaDupaNume(query);
    else if(opt == 2) rezultate = bib.cautaDupaCategorie(query);
    else if(opt == 3) rezultate = bib.cautaDupaEditura(query);
    
    std::vector<std::string> headers = {"Nr", "Titlu", "Autor", "Stoc", "Locatie/Format"};
    std::vector<std::vector<std::string>> rows;
    for(size_t i=0; i<rezultate.size(); ++i) {
        std::string loc = "";
        if (rezultate[i]->getTipFormat() == 0) {
            auto cf = std::dynamic_pointer_cast<CarteFizica>(rezultate[i]);
            loc = cf->getLocatieRaft().camera + " (" + cf->getLocatieRaft().raft + ")";
        } else if (rezultate[i]->getTipFormat() == 2) {
            auto ca = std::dynamic_pointer_cast<CarteAudio>(rezultate[i]);
            loc = "Audio (" + ca->getDurataFormatata() + ")";
        } else {
            auto cd = std::dynamic_pointer_cast<CarteDigitala>(rezultate[i]);
            loc = "Digital (" + cd->getFormatFisier() + ")";
        }
        auto tr = [](std::string s, size_t m) { return s.length() > m ? s.substr(0, m-3) + "..." : s; };
        rows.push_back({std::to_string(i+1), tr(rezultate[i]->getTitlu(), 20), tr(rezultate[i]->autoriToString(), 15), std::to_string(rezultate[i]->getStocDisponibil()), tr(loc, 20)});
    }
    
    int sel = -1;
    if(rezultate.empty()) {
        ConsoleUI::clearScreen();
        ConsoleUI::showToast("📭 Nu s-au gasit carti pentru cautarea data!", ConsoleUI::Yellow);
    } else {
        ConsoleUI::clearScreen();
        sel = ConsoleUI::showPaginatedTable(2, 2, "Rezultate", headers, rows);
    }
    
    if(sel >= 0 && sel < (int)rezultate.size()) {
        ConsoleUI::clearScreen();
        ConsoleUI::drawBox(2, 2, 70, 15, "Detalii Carte");
        ConsoleUI::printAt(4, 4, "Titlu: " + rezultate[sel]->getTitlu(), ConsoleUI::Cyan, true);
        ConsoleUI::printAt(4, 5, "Autor: " + rezultate[sel]->autoriToString());
        ConsoleUI::printAt(4, 6, "Editură: " + rezultate[sel]->getEditura());
        ConsoleUI::printAt(4, 7, "ISBN: " + rezultate[sel]->getIsbn());
        ConsoleUI::printAt(4, 8, "Stoc disp: " + std::to_string(rezultate[sel]->getStocDisponibil()));
        
        int current_y = 9;
        if (rezultate[sel]->getTipFormat() == 0) {
            auto cf = std::dynamic_pointer_cast<CarteFizica>(rezultate[sel]);
            Locatie l = cf->getLocatieRaft();
            ConsoleUI::printAt(4, current_y++, "Locatie: " + l.cladire + ", " + l.camera + ", " + l.culoar + "-" + l.raft, ConsoleUI::Cyan);
        } else if (rezultate[sel]->getTipFormat() == 2) {
            auto ca = std::dynamic_pointer_cast<CarteAudio>(rezultate[sel]);
            ConsoleUI::printAt(4, current_y++, "Narator: " + ca->getNarator(), ConsoleUI::Cyan);
            ConsoleUI::printAt(4, current_y++, "Durata: " + ca->getDurataFormatata(), ConsoleUI::Cyan);
        } else {
            auto cd = std::dynamic_pointer_cast<CarteDigitala>(rezultate[sel]);
            ConsoleUI::printAt(4, current_y++, "Format: " + cd->getFormatFisier() + " | " + std::to_string(cd->getDimensiuneMB()) + "MB", ConsoleUI::Cyan);
        }
        
        ConsoleUI::printAt(4, current_y++, "Rating: " + std::to_string(bib.calculeazaRatingMediu(rezultate[sel]->getIsbn())));
        
        ConsoleUI::printAt(4, current_y++, "--- RECENZII ---", ConsoleUI::Yellow, true);
        ConsoleUI::moveTo(4, current_y++);
        bib.afiseazaRecenziiCarte(std::cout, rezultate[sel]->getIsbn());
        std::cout << std::endl;
        
        pausare();
    }
}

void adaugaUtilizatorUI(Biblioteca& bib) {
    int opt = 0;
    opt = ConsoleUI::showInteractiveMenu(2, 2, "Tip Utilizator", {"Director", "Bibliotecar", "Îngrijitor", "Cititor"}, opt);
    if(opt < 0) return;
    
    ConsoleUI::clearScreen();
    ConsoleUI::drawBox(2, 2, 60, 15, "Date Utilizator");
    std::string n = ConsoleUI::citesteFormularLinie(4, 4, "Nume: ", 20);
    if (n.empty()) return;
    std::string pr = ConsoleUI::citesteFormularLinie(4, 5, "Prenume: ", 20);
    std::string e = ConsoleUI::citesteFormularLinie(4, 6, "Email: ", 30);
    std::string t = ConsoleUI::citesteFormularLinie(4, 7, "Telefon: ", 15);
    
    std::string prefix = (opt == 0) ? "DIR" : (opt == 1) ? "BIB" : (opt == 2) ? "ING" : "CIT";
    std::string id = generateUserId(prefix);
    ConsoleUI::printAt(4, 8, "ID Generat automat: " + id, ConsoleUI::Yellow, true);
    std::string p = ConsoleUI::citesteFormularLinie(4, 9, "Parola dorita: ", 20);
    
    if(opt == 3) {
        bib.adaugaCititor(id, p, n, pr, "0000000", e, t, "Adresa", "standard", "01/01/2026", "01/01/2027", 3);
    } else if(opt == 1) {
        bib.adaugaBibliotecar(id, p, n, pr, "000000", e, t, "Adresa", "Generala", 3500, "01/01/2024", "08-16", 100);
    } else if (opt == 2) {
        bib.adaugaIngrijitor(id, p, n, pr, "000000", e, t, "Adresa", "Curte", 3000, "01/01/2024", "08-16", "Mătura");
    } else {
        bib.adaugaDirector(id, p, n, pr, "000000", e, t, "Adresa", "Management", 8500, "01/01/2024", "Birou 1", 5);
    }
    
    bib.salveazaDate();
    ConsoleUI::showToast("✅ Utilizator generat! ID: " + id + " Parola: " + p);
}

void stergeUtilizatorUI(Biblioteca& bib, const std::string& id_actor) {
    int tip_opt = 0;
    tip_opt = ConsoleUI::showInteractiveMenu(2, 2, "Alege Tip Utilizator", {"Toti", "Director", "Bibliotecar", "Îngrijitor", "Cititor"}, tip_opt);
    if(tip_opt < 0) return;
    
    auto toti = bib.getTotiUtilizatorii();
    std::vector<std::shared_ptr<Utilizator>> utils;
    std::string tip_dorit = (tip_opt == 1) ? "DIRECTOR" : (tip_opt == 2) ? "BIBLIOTECAR" : (tip_opt == 3) ? "INGRIJITOR" : (tip_opt == 4) ? "CITITOR" : "TOTI";
    
    for(const auto& u : toti) {
        if(tip_opt == 0 || u->getTip() == tip_dorit) utils.push_back(u);
    }
    if(utils.empty()) { ConsoleUI::showToast("❌ Niciun utilizator gasit!"); return; }

    std::vector<std::string> headers = {"ID", "Nume", "Rol"};
    std::vector<std::vector<std::string>> rows;
    for(const auto& u : utils) rows.push_back({u->getId(), u->getNumeComplet(), u->getTip()});
    
    int sel = ConsoleUI::showPaginatedTable(2, 2, "Șterge Utilizator", headers, rows);
    if(sel >= 0) {
        if(bib.stergeUtilizator(utils[sel]->getId(), id_actor)) ConsoleUI::showToast("✅ Șters");
        else ConsoleUI::showToast("❌ Eroare", ConsoleUI::Red);
    }
}

void restaureazaUtilizatorUI(Biblioteca& bib, const std::string& id_actor) {
    auto utils = bib.getUtilizatoriStersi();
    if(utils.empty()) { ConsoleUI::showToast("❌ Niciun utilizator sters in stocare!"); return; }

    std::vector<std::string> headers = {"ID", "Nume", "Rol"};
    std::vector<std::vector<std::string>> rows;
    for(const auto& u : utils) rows.push_back({u->getId(), u->getNumeComplet(), u->getTip()});
    
    int sel = ConsoleUI::showPaginatedTable(2, 2, "Restaurează Utilizator", headers, rows);
    if(sel >= 0) {
        if(bib.restaureazaUtilizator(utils[sel]->getId(), id_actor)) ConsoleUI::showToast("✅ Restaurat!");
        else ConsoleUI::showToast("❌ Eroare", ConsoleUI::Red);
    }
}

void modificaUtilizatorUI(Biblioteca& bib, const std::string& id_actor) {
    int tip_opt = 0;
    tip_opt = ConsoleUI::showInteractiveMenu(2, 2, "Alege Tip Utilizator", {"Toti", "Director", "Bibliotecar", "Îngrijitor", "Cititor"}, tip_opt);
    if(tip_opt < 0) return;
    
    auto toti = bib.getTotiUtilizatorii();
    std::vector<std::shared_ptr<Utilizator>> utils;
    std::string tip_dorit = (tip_opt == 1) ? "DIRECTOR" : (tip_opt == 2) ? "BIBLIOTECAR" : (tip_opt == 3) ? "INGRIJITOR" : (tip_opt == 4) ? "CITITOR" : "TOTI";
    
    for(const auto& u : toti) {
        if(tip_opt == 0 || u->getTip() == tip_dorit) utils.push_back(u);
    }
    if(utils.empty()) { ConsoleUI::showToast("❌ Niciun utilizator gasit!"); return; }

    std::vector<std::string> headers = {"ID", "Nume", "Rol"};
    std::vector<std::vector<std::string>> rows;
    for(const auto& u : utils) rows.push_back({u->getId(), u->getNumeComplet(), u->getTip()});
    
    int sel = ConsoleUI::showPaginatedTable(2, 2, "Modifica Utilizator", headers, rows);
    if(sel >= 0) {
        ConsoleUI::clearScreen();
        ConsoleUI::drawBox(2, 2, 60, 15, "Modifica " + utils[sel]->getNumeComplet());
        std::string nume = ConsoleUI::citesteFormularLinie(4, 4, "Nume nou (sau gol pt pastrare): ", 30);
        std::string pren = ConsoleUI::citesteFormularLinie(4, 5, "Prenume nou: ", 30);
        std::string tel = ConsoleUI::citesteFormularLinie(4, 6, "Telefon nou: ", 15);
        std::string adr = ConsoleUI::citesteFormularLinie(4, 7, "Adresa noua: ", 30);
        
        nume = nume.empty() ? utils[sel]->getNume() : nume;
        pren = pren.empty() ? utils[sel]->getPrenume() : pren;
        tel = tel.empty() ? utils[sel]->getTelefon() : tel;
        adr = adr.empty() ? utils[sel]->getAdresa() : adr;

        if(bib.modificaUtilizator(utils[sel]->getId(), utils[sel]->getParola(), nume, pren, utils[sel]->getEmail(), tel, adr, id_actor)) 
            ConsoleUI::showToast("✅ Modificat cu succes!");
    }
}

void imprumutaCarteUI(Biblioteca& bib) {
    auto carti = bib.getToateCartile();
    std::vector<std::string> headers = {"Nr", "Titlu", "Stoc"};
    std::vector<std::vector<std::string>> rows;
    for(size_t i=0; i<carti.size(); ++i) rows.push_back({std::to_string(i+1), carti[i]->getTitlu(), std::to_string(carti[i]->getStocDisponibil())});
    int selCarte = ConsoleUI::showPaginatedTable(2, 2, "1. Alege Cartea de Împrumutat", headers, rows);
    if(selCarte < 0) return;
    
    auto utils = bib.getTotiUtilizatorii();
    std::vector<std::string> headersU = {"ID", "Nume", "Rol"};
    std::vector<std::vector<std::string>> rowsU;
    for(const auto& u : utils) {
        if(u->getTip() == "CITITOR") rowsU.push_back({u->getId(), u->getNumeComplet(), u->getTip()});
    }
    int selUser = ConsoleUI::showPaginatedTable(2, 2, "2. Alege Cititorul", headersU, rowsU);
    if(selUser < 0) return;
    
    if(bib.adaugaImprumut(carti[selCarte]->getIsbn(), rowsU[selUser][0], getDataCurenta(bib), getDataCurentaPlus(bib, 14), "Manual"))
        ConsoleUI::showToast("✅ Împrumutat cu succes");
    else ConsoleUI::showToast("❌ Eroare (Stoc insuficient sau limită atinsă)", ConsoleUI::Red);
}

void returneazaCarteUI(Biblioteca& bib, const std::string& id_actor) {
    auto imprumuturi = bib.getToateImprumuturile();
    std::vector<std::string> headers = {"Titlu Carte", "Nume Cititor", "Termen"};
    std::vector<std::vector<std::string>> rows;
    for(const auto& i : imprumuturi) {
        auto c = bib.gasesteCarte(i.getIdCarte());
        auto u = bib.gasesteUtilizator(i.getIdCititor());
        std::string titlu = c ? c->getTitlu() : i.getIdCarte();
        std::string nume = u ? u->getNumeComplet() : i.getIdCititor();
        rows.push_back({titlu, nume, i.getTermenLimita()});
    }
    
    ConsoleUI::clearScreen();
    int sel = ConsoleUI::showPaginatedTable(2, 2, "Returnare Manuala", headers, rows);
    if(sel >= 0) {
        if(bib.returneazaCarte(imprumuturi[sel].getIdCarte(), imprumuturi[sel].getIdCititor())) ConsoleUI::showToast("✅ Returnat");
    }
}

void toateImprumuturileUI(Biblioteca& bib) {
    auto imprumuturi = bib.getToateImprumuturile();
    std::vector<std::string> headers = {"Titlu Carte", "Nume Cititor", "Data Imprumut"};
    std::vector<std::vector<std::string>> rows;
    for(const auto& i : imprumuturi) {
        auto c = bib.gasesteCarte(i.getIdCarte());
        auto u = bib.gasesteUtilizator(i.getIdCititor());
        std::string titlu = c ? c->getTitlu() : i.getIdCarte();
        std::string nume = u ? u->getNumeComplet() : i.getIdCititor();
        rows.push_back({titlu, nume, i.getDataImprumut()});
    }
    ConsoleUI::clearScreen();
    ConsoleUI::showPaginatedTable(2, 2, "Toate Imprumuturile", headers, rows);
}

void imprumutaCarteCititorUI(Biblioteca& bib, const std::string& idCititor) {
    auto u = bib.gasesteUtilizator(idCititor);
    if(u && u->getRol() == "Cititor") {
        auto cit = std::dynamic_pointer_cast<Cititor>(u);
        if(cit && cit->getPenalizari() > 0) {
            ConsoleUI::showToast("❌ Nu poti imprumuta! Ai amenzi neplatite (sau in asteptare).", ConsoleUI::Red);
            return;
        }
        if(cit && !cit->poateImprumuta()) {
            ConsoleUI::showToast("❌ Nu poti imprumuta! Limita atinsa sau cont inactiv.", ConsoleUI::Red);
            return;
        }
    }
    
    auto carti = bib.getToateCartile();
    std::vector<std::string> headers = {"Nr", "Titlu", "Stoc"};
    std::vector<std::vector<std::string>> rows;
    for(size_t i=0; i<carti.size(); ++i) rows.push_back({std::to_string(i+1), carti[i]->getTitlu(), std::to_string(carti[i]->getStocDisponibil())});
    
    int sel = ConsoleUI::showPaginatedTable(2, 2, "Împrumută Carte", headers, rows);
    if(sel >= 0) {
        if(bib.adaugaImprumut(carti[sel]->getIsbn(), idCititor, getDataCurenta(bib), getDataCurentaPlus(bib, 14), "Auto"))
            ConsoleUI::showToast("✅ Împrumut activat!");
        else {
            ConsoleUI::showToast("❌ Stoc 0. Rezervati cartea din meniul Rezervari.", ConsoleUI::Red);
            pausare();
        }
    }
}

void returneazaCarteCititorUI(Biblioteca& bib, const std::string& idCititor) {
    auto imprumuturi = bib.getToateImprumuturile();
    auto returnari_pendinte = bib.getReturnariInAsteptare();
    std::vector<const Imprumut*> aleMele;
    
    for(const auto& i : imprumuturi) {
        if(i.getIdCititor() == idCititor) {
            bool dejaSpreReturnare = false;
            for(const auto& r : returnari_pendinte) {
                if(r.isbn == i.getIdCarte() && r.id_cititor == idCititor) {
                    dejaSpreReturnare = true;
                    break;
                }
            }
            if(!dejaSpreReturnare) aleMele.push_back(&i);
        }
    }
    
    if(aleMele.empty()) { ConsoleUI::showToast("📭 Niciun imprumut de returnat.", ConsoleUI::Yellow); return; }
    
    std::vector<std::string> headers = {"Titlu", "Data Imprumut", "Termen Limita"};
    std::vector<std::vector<std::string>> rows;
    for(const auto* i : aleMele) {
        auto carte = bib.gasesteCarte(i->getIdCarte());
        std::string titlu = carte ? carte->getTitlu() : i->getIdCarte();
        rows.push_back({titlu, i->getDataImprumut(), i->getTermenLimita()});
    }
    
    int sel = ConsoleUI::showPaginatedTable(2, 2, "Returnează Carte", headers, rows);
    if(sel >= 0) {
        if(bib.solicitaReturnare(aleMele[sel]->getIdCarte(), idCititor)) {
            ConsoleUI::showToast("✅ Solicitare trimisă!");
        } else {
            ConsoleUI::showToast("❌ Solicitare deja trimisă!", ConsoleUI::Red);
        }
    }
}

void confirmaReturnariUI(Biblioteca& bib, const std::string& id_actor) {
    auto ret = bib.getReturnariInAsteptare();
    if(ret.empty()) { ConsoleUI::showToast("📭 Nu exista returnari in asteptare.", ConsoleUI::Yellow); return; }
    
    std::vector<std::string> headers = {"Titlu Carte", "Nume Cititor"};
    std::vector<std::vector<std::string>> rows;
    for(const auto& r : ret) {
        auto c = bib.gasesteCarte(r.isbn);
        auto u = bib.gasesteUtilizator(r.id_cititor);
        std::string titlu = c ? c->getTitlu() : r.isbn;
        std::string nume = u ? u->getNumeComplet() : r.id_cititor;
        rows.push_back({titlu, nume});
    }
    
    int sel = ConsoleUI::showPaginatedTable(2, 2, "Confirmă Returnări", headers, rows);
    if(sel >= 0) {
        int opt = 0;
        opt = ConsoleUI::showInteractiveMenu(2, 15, "Actiune Returnare", {"Confirma (Stare BUNA)", "Declara DEFECTA / Refuza"}, opt);
        if(opt == 0) bib.confirmaReturnare(sel, id_actor);
        else if(opt == 1) bib.refuzaReturnareDefecta(sel, id_actor);
        bib.salveazaDate();
        ConsoleUI::showToast("✅ Returnare procesata cu succes!");
    }
}

void solicitaPlataUI(Biblioteca& bib, const std::shared_ptr<Utilizator>& u) {
    auto cit = std::dynamic_pointer_cast<Cititor>(u);
    if(!cit || cit->getPenalizari() <= 0) {
        ConsoleUI::showToast("❌ Fără amenzi", ConsoleUI::Yellow);
        return;
    }
    
    // Verifică dacă există deja o plată în așteptare
    auto plati = bib.getPlatiInAsteptare();
    bool dejaSolicitat = false;
    for(const auto& p : plati) {
        if(p.id_cititor == u->getId()) {
            dejaSolicitat = true;
            break;
        }
    }
    
    if(dejaSolicitat) {
        int idx = 0;
        ConsoleUI::showInteractiveMenu(2, 2, "Status Amenzi", {"Plata ta este în curs de validare de către conducere.", "Înapoi"}, idx);
        return;
    }
    
    char buf[128];
    snprintf(buf, sizeof(buf), "Plătește %.2f RON (Motiv: Cărți defecte/pierdute/întârzieri)", cit->getPenalizari());
    
    int opt = 0;
    opt = ConsoleUI::showInteractiveMenu(2, 2, "Meniu Amenzi", {std::string(buf), "Înapoi"}, opt);
    
    if(opt == 0) {
        if(bib.solicitaPlata(u->getId())) {
            bib.salveazaDate();
            ConsoleUI::showToast("✅ Solicitare trimisă către conducere!");
        } else {
            ConsoleUI::showToast("❌ Solicitare deja existentă!", ConsoleUI::Red);
        }
    }
}

void confirmaPlatiUI(Biblioteca& bib, const std::string& id_actor) {
    auto plati = bib.getPlatiInAsteptare();
    if(plati.empty()) { ConsoleUI::showToast("📭 Nu exista plati in asteptare.", ConsoleUI::Yellow); return; }
    std::vector<std::string> headers = {"Cititor", "Suma (RON)", "Data Cererii"};
    std::vector<std::vector<std::string>> rows;
    for(const auto& p : plati) {
        auto u = bib.gasesteUtilizator(p.id_cititor);
        std::string nume = u ? u->getNumeComplet() : p.id_cititor;
        char buf[32];
        snprintf(buf, sizeof(buf), "%.2f", p.suma);
        rows.push_back({nume, std::string(buf), p.data_solicitare});
    }
    
    int sel = ConsoleUI::showPaginatedTable(2, 2, "Confirmă Plăți", headers, rows);
    if(sel >= 0) {
        if(bib.confirmaPlata(sel, id_actor)) {
            bib.salveazaDate();
            ConsoleUI::showToast("✅ Plată confirmată! Contul a fost actualizat.");
        }
    }
}

void reconditioneazaCartiUI(Biblioteca& bib) {
    auto carti = bib.getToateCartile();
    std::vector<std::string> headers = {"Titlu", "ISBN", "Stare"};
    std::vector<std::vector<std::string>> rows;
    std::vector<std::string> isbns;
    for(size_t i=0; i<carti.size(); ++i) {
        if(carti[i]->getStareDefecta()) {
            rows.push_back({carti[i]->getTitlu(), carti[i]->getIsbn(), "DEFECTA"});
            isbns.push_back(carti[i]->getIsbn());
        }
    }
    
    if(rows.empty()) { ConsoleUI::showToast("📭 Nu exista carti defecte in sistem.", ConsoleUI::Green); return; }
    
    int sel = ConsoleUI::showPaginatedTable(2, 2, "Repară Cărți Defecte", headers, rows);
    if(sel >= 0) {
        if(bib.reparaCarte(isbns[sel])) {
            bib.salveazaDate();
            ConsoleUI::showToast("✅ Reparatie marcata cu succes!");
        }
    }
}

void rezervaCarteUI(Biblioteca& bib, const std::string& idCititor) {
    auto u = bib.gasesteUtilizator(idCititor);
    if(u && u->getRol() == "Cititor") {
        auto cit = std::dynamic_pointer_cast<Cititor>(u);
        if(cit && cit->getPenalizari() > 0) {
            ConsoleUI::showToast("❌ Nu poti rezerva! Ai amenzi neplatite (sau in asteptare).", ConsoleUI::Red);
            return;
        }
        if(cit && !cit->poateImprumuta()) {
            ConsoleUI::showToast("❌ Nu poti rezerva! Limita atinsa sau cont inactiv.", ConsoleUI::Red);
            return;
        }
    }
    
    auto carti = bib.getToateCartile();
    std::vector<std::string> headers = {"Titlu", "ISBN", "Stoc"};
    std::vector<std::vector<std::string>> rows;
    for(size_t i=0; i<carti.size(); ++i) {
        if(carti[i]->getStocDisponibil() == 0) {
            rows.push_back({carti[i]->getTitlu(), carti[i]->getIsbn(), "0"});
        }
    }
    
    if(rows.empty()) { ConsoleUI::showToast("📭 Nu exista carti cu stoc zero pentru rezervare.", ConsoleUI::Yellow); return; }
    
    int sel = ConsoleUI::showPaginatedTable(2, 2, "Rezervă Carte (Stoc Epuizat)", headers, rows);
    if(sel >= 0) {
        std::string isbn = rows[sel][1]; // Acum rows[sel][1] contine ISBN
        if(bib.adaugaRezervare(isbn, idCititor)) {
            bib.salveazaDate();
            ConsoleUI::showToast("✅ Rezervare adăugată");
        } else {
            ConsoleUI::showToast("❌ Eroare la rezervare", ConsoleUI::Red);
        }
    }
}

void confirmaRidicarRezervareUI(Biblioteca& bib, const std::string& id_actor) {
    auto all_rez = bib.getRezervari();
    std::vector<Rezervare> rez;
    for(const auto& r : all_rez) {
        if(r.alocata) rez.push_back(r);
    }
    
    if(rez.empty()) { ConsoleUI::showToast("📭 Nu exista rezervari gata de ridicat.", ConsoleUI::Yellow); return; }
    
    std::vector<std::string> headers = {"Cititor", "Titlu", "Data Exp."};
    std::vector<std::vector<std::string>> rows;
    for(size_t i=0; i<rez.size(); ++i) {
        auto c = bib.gasesteCarte(rez[i].isbn);
        auto u = bib.gasesteUtilizator(rez[i].id_cititor);
        std::string titlu = c ? c->getTitlu() : rez[i].isbn;
        std::string nume = u ? u->getNumeComplet() : rez[i].id_cititor;
        rows.push_back({nume, titlu, rez[i].data_expirare});
    }
    
    int sel = ConsoleUI::showPaginatedTable(2, 2, "Confirma Ridicare Rezervari", headers, rows);
    if(sel >= 0) {
        int real_idx = -1;
        for(size_t i=0; i<all_rez.size(); ++i) {
            if(all_rez[i].isbn == rez[sel].isbn && all_rez[i].id_cititor == rez[sel].id_cititor) {
                real_idx = i; // 0-based index for confirmaRidicare
                break;
            }
        }
        
        if(real_idx != -1 && bib.confirmaRidicare(real_idx, id_actor)) {
            bib.salveazaDate();
            ConsoleUI::showToast("✅ Rezervare confirmata. S-a transformat in imprumut!");
        } else {
            ConsoleUI::showToast("❌ Eroare la confirmare.", ConsoleUI::Red);
        }
    }
}

void scrieRecenzieUI(Biblioteca& bib, const std::string& idCititor, const std::string& numeCititor) {
    auto carti = bib.getToateCartile();
    std::vector<std::string> headers = {"Nr", "Titlu", "Autor"};
    std::vector<std::vector<std::string>> rows;
    for(size_t i=0; i<carti.size(); ++i) rows.push_back({std::to_string(i+1), carti[i]->getTitlu(), carti[i]->autoriToString()});
    
    int sel = ConsoleUI::showPaginatedTable(2, 2, "Alege carte pentru recenzie", headers, rows);
    if(sel >= 0) {
        ConsoleUI::clearScreen();
        
        int nota_opt = 0;
        nota_opt = ConsoleUI::showInteractiveMenu(2, 2, "Recenzie: " + carti[sel]->getTitlu() + " - Nota", {"⭐ 1 - Foarte slaba", "⭐⭐ 2 - Slaba", "⭐⭐⭐ 3 - Medie", "⭐⭐⭐⭐ 4 - Buna", "⭐⭐⭐⭐⭐ 5 - Excelenta"}, nota_opt);
        if(nota_opt < 0) return;
        int nota = nota_opt + 1;
        
        ConsoleUI::clearScreen();
        ConsoleUI::drawBox(2, 2, 60, 6, "Recenzie: " + carti[sel]->getTitlu());
        ConsoleUI::printAt(4, 4, "Nota selectata: " + std::to_string(nota) + "/5", ConsoleUI::Yellow, true);
        std::string text = ConsoleUI::citesteFormularLinie(4, 5, "Comentariu (optional): ", 50);
        if(bib.adaugaRecenzie(carti[sel]->getIsbn(), idCititor, numeCititor, nota, text)) ConsoleUI::showToast("\u2705 Recenzie Salvata");
    }
}

void rezervaSalaUI(Biblioteca& bib, const std::string& idCititor, const std::string& numeCititor) {
    int sala = 0;
    sala = ConsoleUI::showInteractiveMenu(2, 2, "Alege Sala", {"Sala A", "Sala B", "Sala C"}, sala);
    if(sala < 0) return;
    
    int interval = 0;
    interval = ConsoleUI::showInteractiveMenu(30, 2, "Alege Interval", {"08:00-10:00", "10:00-12:00", "12:00-14:00", "14:00-16:00", "16:00-18:00"}, interval);
    if(interval < 0) return;
    
    ConsoleUI::clearScreen();
    ConsoleUI::drawBox(2, 2, 50, 6, "Rezervare Sala");
    std::string default_date = getDataCurenta(bib);
    std::string data = ConsoleUI::citesteFormularLinie(4, 4, "Data (DD/MM/YYYY) [" + default_date + "]: ", 15);
    if(data.empty()) data = default_date;
    
    std::string nume_sala = (sala == 0) ? "Sala A" : (sala == 1) ? "Sala B" : "Sala C";
    std::vector<std::string> inter = {"08:00-10:00", "10:00-12:00", "12:00-14:00", "14:00-16:00", "16:00-18:00"};
    std::string int_sala = inter[interval];
    
    if(bib.rezervaSala(idCititor, numeCititor, nume_sala, data, int_sala)) {
        bib.salveazaDate();
        ConsoleUI::showToast("✅ Rezervat cu succes!");
    }
    else ConsoleUI::showToast("❌ Ocupat la acea data/ora", ConsoleUI::Red);
}


void veziImprumuturileMeleUI(Biblioteca& bib, const std::string& idCititor) {
    auto imprumuturi = bib.getToateImprumuturile();
    std::vector<std::string> headers = {"Titlu", "Data Imprumut", "Termen Limita"};
    std::vector<std::vector<std::string>> rows;
    for(const auto& i : imprumuturi) {
        if(i.getIdCititor() == idCititor) {
            auto carte = bib.gasesteCarte(i.getIdCarte());
            std::string titlu = carte ? carte->getTitlu() : i.getIdCarte();
            rows.push_back({titlu, i.getDataImprumut(), i.getTermenLimita()});
        }
    }
    if(rows.empty()) { ConsoleUI::showToast("📭 Niciun imprumut activ.", ConsoleUI::Yellow); return; }
    ConsoleUI::showPaginatedTable(2, 2, "Imprumuturile Mele", headers, rows);
}


void veziRezervariSaliMeleUI(Biblioteca& bib, const std::string& idCititor) {
    auto& sali = bib.getToateRezervariSali();
    std::vector<std::string> headers = {"Sala", "Data", "Interval Orar"};
    std::vector<std::vector<std::string>> rows;
    for(const auto& r : sali) {
        if(r.id_cititor == idCititor) {
            rows.push_back({r.sala, r.data, r.interval_orar});
        }
    }
    if(rows.empty()) { ConsoleUI::showToast("📭 Nicio rezervare de sala.", ConsoleUI::Yellow); return; }
    ConsoleUI::showPaginatedTable(2, 2, "Salile Mele Rezervate", headers, rows);
}

void achizitiiUI(Biblioteca& bib, const std::string& id_actor) {
    auto carti = bib.getToateCartile();
    std::vector<std::string> headers = {"Nr", "Titlu", "Stoc"};
    std::vector<std::vector<std::string>> rows;
    for(size_t i=0; i<carti.size(); ++i) rows.push_back({std::to_string(i+1), carti[i]->getTitlu(), std::to_string(carti[i]->getStocDisponibil())});
    
    int sel = ConsoleUI::showPaginatedTable(2, 2, "Achiziție stoc", headers, rows);
    if(sel >= 0) {
        ConsoleUI::clearScreen();
        ConsoleUI::drawBox(2, 2, 50, 6, "Achiziție din Buget");
        int cant = ConsoleUI::citesteFormularInt(4, 4, "Cantitate de cumpărat: ");
        if(bib.achizitioneazaExemplare(carti[sel]->getIsbn(), cant, id_actor)) ConsoleUI::showToast("✅ Achiziție reușită!");
    }
}

void casareCarteUI(Biblioteca& bib, const std::string& id_actor) {
    auto carti = bib.getToateCartile();
    std::vector<std::string> headers = {"Nr", "Titlu", "Stoc"};
    std::vector<std::vector<std::string>> rows;
    for(size_t i=0; i<carti.size(); ++i) rows.push_back({std::to_string(i+1), carti[i]->getTitlu(), std::to_string(carti[i]->getStocDisponibil())});
    
    int sel = ConsoleUI::showPaginatedTable(2, 2, "Casare Carte (Stergere Ex.)", headers, rows);
    if(sel >= 0) {
        // Pick user
        auto utilizatori = bib.getTotiUtilizatorii();
        std::vector<std::vector<std::string>> u_rows;
        std::vector<std::string> u_ids;
        u_rows.push_back({"Nimeni (Stoc Intern)", "N/A"});
        u_ids.push_back("");
        
        for(const auto& u : utilizatori) {
            if(u->getRol() == "Cititor") {
                u_rows.push_back({u->getId(), u->getNumeComplet()});
                u_ids.push_back(u->getId());
            }
        }
        
        int u_sel = ConsoleUI::showPaginatedTable(2, 2, "Cine a pierdut cartea?", {"ID", "Nume"}, u_rows);
        if(u_sel < 0) return;
        
        ConsoleUI::clearScreen();
        ConsoleUI::drawBox(2, 2, 50, 6, "Casare Carte");
        std::string id_cit = u_ids[u_sel];
        
        if(bib.caseazaCarte(carti[sel]->getIsbn(), id_cit, id_actor)) {
            bib.salveazaDate();
            ConsoleUI::showToast("✅ Casare reușită!");
        }
    }
}

void modificaProfilUI(Biblioteca& bib, const std::string& id_actor) {
    auto u = bib.gasesteUtilizator(id_actor);
    if(!u) return;
    ConsoleUI::clearScreen();
    ConsoleUI::drawBox(2, 2, 60, 15, "Modifica Profilul Meu");
    std::string par = ConsoleUI::citesteFormularLinie(4, 4, "Parola noua (sau gol): ", 30);
    std::string em = ConsoleUI::citesteFormularLinie(4, 5, "Email nou: ", 30);
    std::string tel = ConsoleUI::citesteFormularLinie(4, 6, "Telefon nou: ", 15);
    std::string adr = ConsoleUI::citesteFormularLinie(4, 7, "Adresa noua: ", 30);
    
    par = par.empty() ? u->getParola() : par;
    em = em.empty() ? u->getEmail() : em;
    tel = tel.empty() ? u->getTelefon() : tel;
    adr = adr.empty() ? u->getAdresa() : adr;
    
    if(bib.modificaUtilizator(id_actor, par, u->getNume(), u->getPrenume(), em, tel, adr, id_actor)) 
        ConsoleUI::showToast("✅ Profil actualizat!");
}

void gestiuneRafturiUI(Biblioteca& bib) {
    int menu_idx = 0;
    while(true) {
        ConsoleUI::clearScreen();
        int opt = ConsoleUI::showInteractiveMenu(5, 5, "Gestiune Rafturi & Sali", {
            "Vizualizare Rafturi",
            "Adauga Raft Nou",
            "Sterge Raft",
            "Inapoi"
        }, menu_idx);
        
        if (opt == 3 || opt == -1) break;
        
        if (opt == 0) {
            const auto& rafturi = bib.getToateRafturile();
            std::vector<std::vector<std::string>> linii;
            for(const auto& r : rafturi) {
                Locatie loc = {r.cladire, r.camera, r.culoar, r.nume_raft};
                int carti = bib.obtineNumarCartiFiziceRaft(loc);
                linii.push_back({r.id, std::to_string(carti) + "/" + std::to_string(r.capacitate_maxima)});
            }
            ConsoleUI::clearScreen();
            int sel = ConsoleUI::showPaginatedTable(2, 2, "Rafturi Existente", {"ID Raft", "Stoc / Capacitate"}, linii);
            if(sel >= 0) {
                Locatie sel_loc = {rafturi[sel].cladire, rafturi[sel].camera, rafturi[sel].culoar, rafturi[sel].nume_raft};
                std::vector<std::vector<std::string>> carti_str;
                for(const auto& c : bib.getToateCartile()) {
                    if(c->getTipFormat() == 0) {
                        auto cf = std::dynamic_pointer_cast<CarteFizica>(c);
                        if (cf->getLocatieRaft().cladire == sel_loc.cladire &&
                            cf->getLocatieRaft().camera == sel_loc.camera &&
                            cf->getLocatieRaft().culoar == sel_loc.culoar &&
                            cf->getLocatieRaft().raft == sel_loc.raft) {
                            carti_str.push_back({cf->getTitlu(), cf->getIsbn(), std::to_string(cf->getStocDisponibil())});
                        }
                    }
                }
                ConsoleUI::clearScreen();
                if(carti_str.empty()) {
                    ConsoleUI::showToast("📭 Raftul este gol!", ConsoleUI::Yellow);
                } else {
                    ConsoleUI::showPaginatedTable(2, 2, "Carti pe raft: " + rafturi[sel].id, {"Titlu", "ISBN", "Stoc"}, carti_str);
                }
            }
        } else if (opt == 1) {
            ConsoleUI::clearScreen();
            ConsoleUI::drawBox(2, 2, 80, 15, "Adaugare Raft Nou");
            
            int cladire_opt = 0;
            std::vector<std::string> cladiri = {"Centrala", "Filiala Nord", "Filiala Sud", "Depozit"};
            cladire_opt = ConsoleUI::showInteractiveMenu(50, 4, "Cladire", cladiri, cladire_opt);
            if(cladire_opt < 0) continue;
            std::string cladire = cladiri[cladire_opt];
            ConsoleUI::clearArea(50, 4, 40, cladiri.size() + 4);
            ConsoleUI::printAt(4, 4, "Cladire: " + cladire, ConsoleUI::Yellow, true);
            
            int camera_opt = 0;
            std::vector<std::string> camere = {"Sala Fictiune", "Sala Stiinta", "Sala Istorie", "Sala IT", "Sala Copii", "Sala Lectura A", "Depozit Subsol"};
            camera_opt = ConsoleUI::showInteractiveMenu(50, 5, "Camera", camere, camera_opt);
            if(camera_opt < 0) continue;
            std::string camera = camere[camera_opt];
            ConsoleUI::clearArea(50, 5, 40, camere.size() + 4);
            ConsoleUI::printAt(4, 5, "Camera: " + camera, ConsoleUI::Yellow, true);

            int culoar_opt = 0;
            std::vector<std::string> culoare = {"Culoar A", "Culoar B", "Culoar C", "Culoar D", "Culoar E", "Fara Culoar"};
            culoar_opt = ConsoleUI::showInteractiveMenu(50, 6, "Culoar", culoare, culoar_opt);
            if(culoar_opt < 0) continue;
            std::string culoar = culoare[culoar_opt];
            ConsoleUI::clearArea(50, 6, 40, culoare.size() + 4);
            ConsoleUI::printAt(4, 6, "Culoar: " + culoar, ConsoleUI::Yellow, true);

            std::string raft = ConsoleUI::citesteFormularLinie(4, 7, "Nume Raft: ", 20);
            if(raft.empty()) continue;
            std::string cap_s = ConsoleUI::citesteFormularLinie(4, 8, "Capacitate (max 50): ", 5);
            int cap = cap_s.empty() ? 50 : std::stoi(cap_s);
            if (bib.adaugaRaft(cladire, camera, culoar, raft, cap)) {
                ConsoleUI::showToast("✅ Raft creat cu succes!", ConsoleUI::Green);
                bib.salveazaDate();
            } else {
                ConsoleUI::showToast("❌ Eroare: Raftul exista deja!", ConsoleUI::Red);
            }
        } else if (opt == 2) {
            const auto& rafturi = bib.getToateRafturile();
            std::vector<std::vector<std::string>> linii;
            for(const auto& r : rafturi) linii.push_back({r.id});
            int sel = ConsoleUI::showPaginatedTable(2, 2, "Stergere Raft", {"ID Raft"}, linii);
            if(sel >= 0) {
                if(bib.stergeRaft(rafturi[sel].id)) {
                    ConsoleUI::showToast("✅ Raft sters!", ConsoleUI::Green);
                    bib.salveazaDate();
                } else {
                    ConsoleUI::showToast("❌ Eroare: Raftul nu este gol sau nu exista!", ConsoleUI::Red);
                }
            }
        }
    }
}

void mutaCarteFizicaUI(Biblioteca& bib) {
    const auto& carti = bib.getToateCartile();
    std::vector<std::shared_ptr<Carte>> fizice;
    std::vector<std::vector<std::string>> linii;
    for(const auto& c : carti) {
        if(c->getTipFormat() == 0) {
            fizice.push_back(c);
            auto cf = std::dynamic_pointer_cast<CarteFizica>(c);
            Locatie l = cf->getLocatieRaft();
            linii.push_back({c->getTitlu(), l.camera + "-" + l.raft});
        }
    }
    
    if(fizice.empty()) {
         ConsoleUI::showToast("Nu exista carti fizice!", ConsoleUI::Yellow);
         return;
    }
    
    int sel_c = ConsoleUI::showPaginatedTable(2, 2, "Selecteaza Cartea de Mutat", {"Titlu", "Locatie Curenta"}, linii);
    if(sel_c < 0) return;
    
    const auto& rafturi = bib.getToateRafturile();
    std::vector<std::vector<std::string>> rafturi_str;
    for(const auto& r : rafturi) {
        Locatie temp_loc = {r.cladire, r.camera, r.culoar, r.nume_raft};
        int curent = bib.obtineNumarCartiFiziceRaft(temp_loc);
        rafturi_str.push_back({r.id, std::to_string(curent) + "/" + std::to_string(r.capacitate_maxima)});
    }
    
    int sel_raft = ConsoleUI::showPaginatedTable(2, 2, "Selecteaza Noul Raft", {"ID Raft", "Ocupat/Capacitate"}, rafturi_str);
    if(sel_raft < 0) return;
    
    const auto& sel_r = rafturi[sel_raft];
    Locatie loc = {sel_r.cladire, sel_r.camera, sel_r.culoar, sel_r.nume_raft};
    
    if(bib.mutaCarte(fizice[sel_c]->getIsbn(), loc)) {
        ConsoleUI::showToast("✅ Carte mutata cu succes pe noul raft!", ConsoleUI::Green);
        bib.salveazaDate();
    } else {
        ConsoleUI::showToast("❌ Eroare: Nu exista suficient spatiu pe noul raft!", ConsoleUI::Red);
    }
}
