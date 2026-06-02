#define _CRT_SECURE_NO_WARNINGS
#include "Biblioteca.h"
#include <iostream>
#include <string>
#include <limits>
#include <cstdlib>
#include <ctime>
#include <sstream>

// ═══════════════════════════════════════════════
//  FUNCȚII UTILITARE
// ═══════════════════════════════════════════════

static std::string getDataCurenta() {
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%d/%m/%Y", timeinfo);
    return std::string(buffer);
}

static std::string getDataCurentaPlus(int zile) {
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];
    time(&rawtime);
    rawtime += zile * 24 * 60 * 60;
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%d/%m/%Y", timeinfo);
    return std::string(buffer);
}


static void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

static void pausare() {
    std::cout << "\n  " << Color::Yellow << "Apasă ENTER pentru a continua..." << Color::Reset;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static std::string citesteLinie(const std::string& prompt) {
    std::cout << Color::Cyan << prompt << Color::Reset;
    std::string val;
    std::getline(std::cin, val);
    return val;
}

static int citesteInt(const std::string& prompt) {
    std::cout << Color::Cyan << prompt << Color::Reset;
    int val;
    while (!(std::cin >> val)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  " << Color::Red << "❌ Introduceți un număr valid: " << Color::Reset;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return val;
}

static double citesteDouble(const std::string& prompt) {
    std::cout << Color::Cyan << prompt << Color::Reset;
    double val;
    while (!(std::cin >> val)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  " << Color::Red << "❌ Introduceți un număr valid: " << Color::Reset;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return val;
}

// ═══════════════════════════════════════════════
//  BANNER
// ═══════════════════════════════════════════════

static void afiseazaBanner(const std::shared_ptr<Utilizator>& curent = nullptr) {
    clearScreen();
    std::cout << "\n";
    std::cout << Color::Cyan;
    std::cout << "  ╔══════════════════════════════════════════════════════════════╗\n";
    std::cout << "  ║                                                              ║\n";
    std::cout << "  ║     📚  " << Color::Bold << "SISTEM DE GESTIUNE A BIBLIOTECII" << Color::Reset << Color::Cyan << "  📚                ║\n";
    std::cout << "  ║                                                              ║\n";
    std::cout << "  ║         Versiune 2.1  ·  Cu Autentificare                    ║\n";
    std::cout << "  ║                                                              ║\n";
    if (curent) {
        std::cout << "  ╠══════════════════════════════════════════════════════════════╣\n";
        std::cout << "  ║  Conectat ca: " << Color::Green << curent->getNumeComplet() << " (" << curent->getRol() << ")" << Color::Cyan << "\n";
    }
    std::cout << "  ╚══════════════════════════════════════════════════════════════╝\n\n";
    std::cout << Color::Reset;
}

// ═══════════════════════════════════════════════
//  UI CĂRȚI
// ═══════════════════════════════════════════════

static void adaugaCarteFizicaUI(Biblioteca& bib) {
    std::cout << "\n  ── Adaugă Carte Fizică ──────────────────\n\n";
    std::string titlu = citesteLinie("  Titlu: ");
    std::string auto_s = citesteLinie("  Autori (,): ");
    std::string isbn = citesteLinie("  ISBN: ");
    double pret = citesteDouble("  Preț (RON): ");
    std::string serie = citesteLinie("  Serie: ");
    std::string poza = citesteLinie("  Poza path: ");
    std::string cat = citesteLinie("  Categorie: ");
    int an = citesteInt("  An: ");
    int pag = citesteInt("  Pagini: ");
    
    std::cout << "  [Stare] 1. BUNA  2. DEFECTA\n";
    int st_opt = citesteInt("  Alege starea: ");
    StareCarte st = (st_opt == 2) ? StareCarte::DEFECTA : StareCarte::BUNA;

    std::string dim = citesteLinie("  Dimensiuni: ");
    double g = citesteDouble("  Greutate (g): ");
    
    std::cout << "  [Copertă] 1. Hardcover  2. Softcover  3. Spiralată\n";
    int cop_opt = citesteInt("  Alege coperta: ");
    std::string coperta = (cop_opt == 1) ? "Hardcover" : (cop_opt == 2) ? "Softcover" : "Spiralată";

    Locatie loc;
    bool locatie_valida = false;
    while (!locatie_valida) {
        std::cout << "\n  [Clădire] 1. Centrală  2. Anexa Nord\n";
        int cld_opt = citesteInt("  Alege clădirea: ");
        loc.cladire = (cld_opt == 2) ? "Anexa Nord" : "Centrală";

        std::cout << "  [Cameră] 1. Sala Lectură  2. Sala Împrumut  3. Depozit\n";
        int cam_opt = citesteInt("  Alege camera: ");
        loc.camera = (cam_opt == 2) ? "Sala Împrumut" : (cam_opt == 3) ? "Depozit" : "Sala Lectură";

        std::cout << "  [Culoar] 1. Culoarul A  2. Culoarul B  3. Culoarul C\n";
        int cul_opt = citesteInt("  Alege culoarul: ");
        loc.culoar = (cul_opt == 2) ? "Culoarul B" : (cul_opt == 3) ? "Culoarul C" : "Culoarul A";

        loc.raft = "Raftul " + std::to_string(citesteInt("  Introdu numărul Raftului (ex. 1, 2, 5): "));

        int count = bib.obtineNumarCartiFiziceRaft(loc);
        if (count >= 50) {
            std::cout << "  ❌ Eroare: " << loc.raft << " este plin (" << count << "/50 cărți)! Alege alt raft.\n";
        } else {
            locatie_valida = true;
        }
    }

    int stoc = citesteInt("  Stoc inițial: ");

    bib.adaugaCarteFizica(titlu, Carte::stringToAutori(auto_s), isbn, pret, serie, poza, st, stoc, stoc, cat, an, pag, dim, g, coperta, loc);
    std::cout << "\n  " << Color::Green << "✅ Adăugat cu succes!" << Color::Reset << "\n";
}

static void adaugaCarteDigitalaUI(Biblioteca& bib) {
    std::cout << "\n  ── Adaugă Carte Digitală ────────────────\n\n";
    std::string titlu = citesteLinie("  Titlu: ");
    std::string auto_s = citesteLinie("  Autori (,): ");
    std::string isbn = citesteLinie("  ISBN: ");
    double pret = citesteDouble("  Preț (RON): ");
    std::string serie = citesteLinie("  Serie: ");
    std::string poza = citesteLinie("  Poza path: ");
    std::string cat = citesteLinie("  Categorie: ");
    int an = citesteInt("  An: ");
    int pag = citesteInt("  Pagini: ");
    
    std::cout << "  [Stare] 1. BUNA  2. DEFECTA\n";
    int st_opt = citesteInt("  Alege starea: ");
    StareCarte st = (st_opt == 2) ? StareCarte::DEFECTA : StareCarte::BUNA;

    std::string format = citesteLinie("  Format: ");
    double dim = citesteDouble("  Dimensiune (MB): ");
    std::string link = citesteLinie("  Link: ");
    bib.adaugaCarteDigitala(titlu, Carte::stringToAutori(auto_s), isbn, pret, serie, poza, st, 999, 999, cat, an, pag, format, dim, link);
    std::cout << "\n  " << Color::Green << "✅ Adăugat cu succes!" << Color::Reset << "\n";
}

static void stergeCarteUI(Biblioteca& bib) {
    std::cout << "\n  ── Cărți înregistrate ───────────────────\n";
    bib.afiseazaInventarScurt(std::cout);
    std::cout << "  ─────────────────────────────────────────\n";
    int nr = citesteInt("  Nr Carte (0 pt anulare): ");
    if (nr == 0) return;
    std::string isbn = bib.getIsbnDupaIndex(nr);
    if (isbn.empty()) {
        std::cout << "  " << Color::Red << "❌ Număr invalid." << Color::Reset << "\n";
        return;
    }
    if (bib.stergeCarte(isbn)) std::cout << "  " << Color::Green << "✅ Șters!" << Color::Reset << "\n";
    else std::cout << "  " << Color::Red << "❌ Eroare." << Color::Reset << "\n";
}

static void afiseazaRezultateCarte(const std::vector<std::shared_ptr<Carte>>& rezultate) {
    if (rezultate.empty()) std::cout << "\n  ⚠ Nu s-au găsit rezultate.\n";
    else { std::cout << "\n  ✓ " << rezultate.size() << " rezultate:\n"; for (const auto& c : rezultate) { c->afisare(std::cout); std::cout << "\n"; } }
}

static void cautaCarteUI(Biblioteca& bib) {
    std::cout << "  1.Autor 2.Titlu 3.Categorie 4.An 5.Pagini\n";
    int opt = citesteInt("  Alege: ");
    switch (opt) {
        case 1: afiseazaRezultateCarte(bib.cautaDupaAutor(citesteLinie("  Autor: "))); break;
        case 2: afiseazaRezultateCarte(bib.cautaDupaNume(citesteLinie("  Titlu: "))); break;
        case 3: afiseazaRezultateCarte(bib.cautaDupaCategorie(citesteLinie("  Categorie: "))); break;
        case 4: afiseazaRezultateCarte(bib.cautaDupaAn(citesteInt("  An: "))); break;
        case 5: afiseazaRezultateCarte(bib.cautaDupaPagini(citesteInt(" Min: "), citesteInt(" Max: "))); break;
    }
}

// ═══════════════════════════════════════════════
// UI UTILIZATORI
// ═══════════════════════════════════════════════

static void adaugaUtilizatorUI(Biblioteca& bib) {
    std::cout << "  1.Director 2.Bibliotecar 3.Îngrijitor 4.Cititor\n";
    int opt = citesteInt("  Tip: ");
    std::string id = citesteLinie("  ID: "), p = citesteLinie("  Parola: "), n = citesteLinie("  Nume: "), pr = citesteLinie("  Prenume: ");
    std::string c = citesteLinie("  CNP: "), e = citesteLinie("  Email: "), t = citesteLinie("  Tel: "), a = citesteLinie("  Adresa: ");

    if (opt == 1) bib.adaugaDirector(id, p, n, pr, c, e, t, a, citesteLinie("  Dept: "), citesteDouble("  Salariu: "), citesteLinie("  Data ang: "), citesteLinie("  Birou: "), citesteInt("  Nivel: "));
    else if (opt == 2) bib.adaugaBibliotecar(id, p, n, pr, c, e, t, a, citesteLinie("  Sectie: "), citesteDouble("  Salariu: "), citesteLinie("  Data ang: "), citesteLinie("  Program: "), citesteInt("  Nr carti: "));
    else if (opt == 3) bib.adaugaIngrijitor(id, p, n, pr, c, e, t, a, citesteLinie("  Zona: "), citesteDouble("  Salariu: "), citesteLinie("  Data ang: "), citesteLinie("  Program: "), citesteLinie("  Echipament: "));
    else if (opt == 4) bib.adaugaCititor(id, p, n, pr, c, e, t, a, citesteLinie("  Abonament: "), citesteLinie("  Inregistrare: "), citesteLinie("  Expirare: "), citesteInt("  Max carti: "));
    std::cout << "\n  " << Color::Green << "✅ Utilizator adăugat." << Color::Reset << "\n";
}

static void stergeUtilizatorUI(Biblioteca& bib) {
    if (bib.stergeUtilizator(citesteLinie("  ID: "))) std::cout << "  " << Color::Green << "✅ Șters." << Color::Reset << "\n";
    else std::cout << "  " << Color::Red << "❌ Eroare." << Color::Reset << "\n";
}

// ═══════════════════════════════════════════════
// UI ÎMPRUMUTURI
// ═══════════════════════════════════════════════

static void imprumutaCarteUI(Biblioteca& bib) {
    std::cout << "\n  ── Cărți disponibile ────────────────────\n";
    bib.afiseazaInventarScurt(std::cout);
    std::cout << "  ─────────────────────────────────────────\n";
    int nr = citesteInt("  Nr Carte (0 pt anulare): ");
    if (nr == 0) return;
    std::string isbn = bib.getIsbnDupaIndex(nr);
    if (isbn.empty()) {
        std::cout << "  " << Color::Red << "❌ Număr invalid." << Color::Reset << "\n";
        return;
    }
    std::string idCititor = citesteLinie("  ID Cititor: ");
    std::string data = getDataCurenta();
    std::string termen = getDataCurentaPlus(14);
    std::string obs = citesteLinie("  Obs: ");
    if (bib.adaugaImprumut(isbn, idCititor, data, termen, obs)) {
        std::cout << "  " << Color::Green << "✅ Împrumut înregistrat: " << data << " -> " << termen << "." << Color::Reset << "\n";
        auto carte = bib.gasesteCarte(isbn);
        if (carte) {
            std::cout << "  ℹ️  Locație din care trebuie preluată: " << carte->getLocatieScurta() << "\n";
        }
    }
}

static void returneazaCarteUI(Biblioteca& bib) {
    std::cout << "\n  ── Situație împrumuturi ─────────────────\n";
    bib.afiseazaToateImprumuturile(std::cout);
    std::cout << "  ─────────────────────────────────────────\n";
    int nr = citesteInt("  Nr Împrumut (0 pt anulare): ");
    if (nr == 0) return;
    auto imp = bib.getImprumutDupaIndex(nr);
    if (!imp) {
        std::cout << "  " << Color::Red << "❌ Număr invalid." << Color::Reset << "\n";
        return;
    }
    if (bib.returneazaCarte(imp->getIdCarte(), imp->getIdCititor()))
        std::cout << "  " << Color::Green << "✅ Returnare înregistrată." << Color::Reset << "\n";
    else std::cout << "  " << Color::Red << "❌ Eroare." << Color::Reset << "\n";
}

static void imprumutaCarteCititorUI(Biblioteca& bib, const std::string& idCititor) {
    std::cout << "\n  ── Cărți disponibile ────────────────────\n";
    bib.afiseazaInventarScurt(std::cout);
    std::cout << "  ─────────────────────────────────────────\n";
    int nr = citesteInt("  Nr Carte (0 pt anulare): ");
    if (nr == 0) return;
    std::string isbn = bib.getIsbnDupaIndex(nr);
    if (isbn.empty()) {
        std::cout << "  " << Color::Red << "❌ Număr invalid." << Color::Reset << "\n";
        return;
    }
    std::string data = getDataCurenta();
    std::string termen = getDataCurentaPlus(14);
    if (bib.adaugaImprumut(isbn, idCititor, data, termen, "Auto-imprumut (14 zile)")) {
        std::cout << "  " << Color::Green << "✅ Împrumut înregistrat automat până pe " << termen << "." << Color::Reset << "\n";
        auto carte = bib.gasesteCarte(isbn);
        if (carte) {
            if (carte->getTip() == "FIZICA") {
                std::cout << "  🚶 Te așteptăm la bibliotecă să o ridici din: " << carte->getLocatieScurta() << "\n";
            } else {
                std::cout << "  💻 Ești gata! Aceasta este o carte digitală.\n";
            }
        }
    }
}

static void returneazaCarteCititorUI(Biblioteca& bib, const std::string& idCititor) {
    std::cout << "\n  \u2500\u2500 \u00cemprumuturIle mele \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n";
    bib.afiseazaImprumuturiCititor(std::cout, idCititor);
    std::cout << "  \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n";
    int nr = citesteInt("  Nr Imprumut (0 pt anulare): ");
    if (nr == 0) return;
    auto imp = bib.getImprumutDupaIndex(nr);
    if (!imp || imp->getIdCititor() != idCititor) {
        std::cout << "  " << Color::Red << "❌ Număr invalid sau nu vă aparține." << Color::Reset << "\n";
        return;
    }
    // Salveaza ISBN inainte de stergere din lista
    std::string isbn = imp->getIdCarte();
    if (bib.solicitaReturnare(isbn, idCititor)) {
        std::cout << "\n  " << Color::Green << "✅ Cerere de returnare înregistrată!" << Color::Reset << "\n";
        std::cout << "  ℹ️  Aduceți cartea la ghișeu.\n";
        std::cout << "  " << Color::Yellow << "⏳ Un bibliotecar o va confirma și o va reintroduce în stoc." << Color::Reset << "\n";
    } else {
        std::cout << "  " << Color::Red << "❌ Eroare la înregistrarea returnării." << Color::Reset << "\n";
    }
}


// ═══════════════════════════════════════════════
//  MENIURI PE ROL
// ═══════════════════════════════════════════════

static void confirmaReturnariUI(Biblioteca& bib) {
    std::cout << "\n  \u2500\u2500 Returnari in asteptare \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n";
    bib.afiseazaReturnariInAsteptare(std::cout);
    if (bib.getNumarReturnariInAsteptare() == 0) return;
    std::cout << "  \u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\n";
    int nr = citesteInt("  Nr returnare de procesat (0 pt anulare): ");
    if (nr == 0) return;
    std::cout << "\n  Ce actiune doriti?\n";
    std::cout << "  1. Confirma returnare (carte buna)\n";
    std::cout << "  2. Marcheaza carte DEFECTA (reintra in stoc, dar marcata defecta)\n";
    std::cout << "  0. Anuleaza\n";
    int actiune = citesteInt("  ➤ Alege o acțiune: ");
    if (actiune == 1) {
        if (bib.confirmaReturnare(static_cast<size_t>(nr)))
            std::cout << "\n  " << Color::Green << "✅ Returnare confirmată! Cartea a fost reintrodusă în stoc." << Color::Reset << "\n";
        else
            std::cout << "  " << Color::Red << "❌ Număr invalid." << Color::Reset << "\n";
    } else if (actiune == 2) {
        if (bib.refuzaReturnareDefecta(static_cast<size_t>(nr)))
            std::cout << "\n  " << Color::Yellow << "⚠️  Carte marcată DEFECTĂ și reintrodusă în stoc." << Color::Reset << "\n";
        else
            std::cout << "  " << Color::Red << "❌ Număr invalid." << Color::Reset << "\n";
    }
}


static void solicitaPlataUI(Biblioteca& bib, const std::shared_ptr<Utilizator>& u) {
    auto cititor = std::dynamic_pointer_cast<Cititor>(u);
    if (!cititor) return;

    if (cititor->getPenalizari() <= 0) {
        std::cout << "  " << Color::Green << "✅ Nu aveți amenzi de plătit." << Color::Reset << "\n";
        return;
    }

    std::cout << "\n  ── Solicitare Plată Amendă ────────────────\n\n";
    std::cout << "  Suma datorată: " << Color::Red << cititor->getPenalizari() << " RON" << Color::Reset << "\n";
    std::cout << "  [INFO] Solicitarea va fi trimisă unui bibliotecar spre confirmare.\n";
    
    std::string confirm = citesteLinie("  Trimite solicitarea de plată? (da/nu): ");
    if (confirm == "da") {
        if (bib.solicitaPlata(cititor->getId())) {
            std::cout << "  " << Color::Green << "✅ Solicitare trimisă! Te rugăm să mergi la ghișeu pentru a achita." << Color::Reset << "\n";
        } else {
            std::cout << "  " << Color::Yellow << "⚠ Există deja o solicitare în așteptare pentru acest cont." << Color::Reset << "\n";
        }
    }
}

static void confirmaPlatiUI(Biblioteca& bib) {
    std::cout << "\n  ── Confirmare Plăți Amenzi ───────────────\n";
    bib.afiseazaPlatiInAsteptare(std::cout);
    
    if (bib.getNumarPlatiInAsteptare() == 0) return;

    int nr = citesteInt("  Alege Nr Cerere (0 pt anulare): ");
    if (nr <= 0) return;

    std::cout << "  [1] Confirmă Plata  [2] Refuză Cererea\n";
    int actiune = citesteInt("  Alege acțiunea: ");

    if (actiune == 1) {
        if (bib.confirmaPlata(static_cast<size_t>(nr)))
            std::cout << "\n  " << Color::Green << "✅ Plată confirmată. Cititorul poate împrumuta din nou!" << Color::Reset << "\n";
        else
            std::cout << "  " << Color::Red << "❌ Număr invalid." << Color::Reset << "\n";
    } else if (actiune == 2) {
        if (bib.refuzaPlata(static_cast<size_t>(nr)))
            std::cout << "\n  " << Color::Yellow << "⚠ Cerere de plată refuzată." << Color::Reset << "\n";
        else
            std::cout << "  " << Color::Red << "❌ Număr invalid." << Color::Reset << "\n";
    }
}

static void reconditioneazaCartiUI(Biblioteca& bib) {
    std::cout << "\n  ── Recondiționare Cărți Defecte ─────────\n\n";
    std::vector<std::shared_ptr<Carte>> defecte;
    // Căutăm manual cărțile defecte
    for (int i = 1; i <= 500; ++i) { // presupunem un range rezonabil pentru indexi
        std::string isbn = bib.getIsbnDupaIndex(i);
        if (isbn.empty()) continue;
        auto c = bib.gasesteCarte(isbn);
        if (c && c->getStareCarte() == StareCarte::DEFECTA) {
            defecte.push_back(c);
            std::cout << "  [" << defecte.size() << "] " << c->getTitlu() << " (ISBN: " << c->getIsbn() << ")\n";
        }
    }

    if (defecte.empty()) {
        std::cout << "  " << Color::Green << "✅ Nu există cărți defecte în sistem." << Color::Reset << "\n";
        return;
    }

    int nr = citesteInt("\n  Alege Nr Carte pentru recondiționare (0 pt anulare): ");
    if (nr <= 0 || nr > defecte.size()) return;

    if (bib.reparaCarte(defecte[nr - 1]->getIsbn())) {
        std::cout << "  " << Color::Green << "✅ Cartea a fost recondiționată și este acum disponibilă!" << Color::Reset << "\n";
    } else {
        std::cout << "  " << Color::Red << "❌ Eroare la recondiționare." << Color::Reset << "\n";
    }
}

static bool MeniuDirector(Biblioteca& bib, const std::shared_ptr<Utilizator>& u) {
    afiseazaBanner(u);
    std::cout << "  " << Color::Bold << "[ Director ]" << Color::Reset << " Acces complet\n\n";
    std::cout << "  " << Color::Cyan << "--- 📚 GESTIUNE CĂRȚI ---" << Color::Reset << "\n";
    std::cout << "   1. Adaugă carte          3. Inventar\n";
    std::cout << "   2. Șterge carte          4. Caută carte\n\n";
    std::cout << "  " << Color::Cyan << "--- 👥 GESTIUNE UTILIZATORI ---" << Color::Reset << "\n";
    std::cout << "  10. Adaugă utilizator    12. Afișează utilizatori\n";
    std::cout << "  11. Șterge utilizator\n\n";
    std::cout << "  " << Color::Cyan << "--- 📋 ÎMPRUMUTURI ȘI RETURURI ---" << Color::Reset << "\n";
    std::cout << "  20. Împrumută            22. Listă împrumuturi\n";
    std::cout << "  21. Returnează           23. Confirmă returnări";
    if (bib.getNumarReturnariInAsteptare() > 0)
        std::cout << Color::Yellow << " [" << bib.getNumarReturnariInAsteptare() << " în așteptare!]" << Color::Reset;
    std::cout << "\n  24. Confirmă Plăți Amenzi";
    if (bib.getNumarPlatiInAsteptare() > 0)
        std::cout << Color::Yellow << " [" << bib.getNumarPlatiInAsteptare() << " noi!]" << Color::Reset;
    std::cout << "\n\n  " << Color::Cyan << "--- ⚙️ SISTEM ---" << Color::Reset << "\n";
    std::cout << "   0. Delogare\n\n";
    
    int opt = citesteInt("  ➤ Alege o opțiune: ");
    switch (opt) {
        case 1: std::cout<<"1.Fizica 2.Digitala\n"; if(citesteInt("  > ")==1) adaugaCarteFizicaUI(bib); else adaugaCarteDigitalaUI(bib); break;
        case 2: stergeCarteUI(bib); break;
        case 3: bib.afiseazaInventarScurt(std::cout); break;
        case 4: cautaCarteUI(bib); break;
        case 10: adaugaUtilizatorUI(bib); break;
        case 11: stergeUtilizatorUI(bib); break;
        case 12: bib.afiseazaUtilizatoriScurt(std::cout); break;
        case 20: imprumutaCarteUI(bib); break;
        case 21: returneazaCarteUI(bib); break;
        case 22: bib.afiseazaToateImprumuturile(std::cout); break;
        case 23: confirmaReturnariUI(bib); break;
        case 24: confirmaPlatiUI(bib); break;
        case 0: return false;
    }
    if(opt != 0) pausare();
    return true;
}

static bool MeniuBibliotecar(Biblioteca& bib, const std::shared_ptr<Utilizator>& u) {
    afiseazaBanner(u);
    std::cout << "  " << Color::Bold << "[ Bibliotecar ]" << Color::Reset << " Gestiune cărți și împrumuturi\n\n";
    std::cout << "  " << Color::Cyan << "--- 📚 GESTIUNE CĂRȚI ---" << Color::Reset << "\n";
    std::cout << "   1. Adaugă carte          3. Inventar\n";
    std::cout << "   2. Șterge carte          4. Caută carte\n\n";
    std::cout << "  " << Color::Cyan << "--- 👥 GESTIUNE UTILIZATORI ---" << Color::Reset << "\n";
    std::cout << "  10. Afișează utilizatori\n\n";
    std::cout << "  " << Color::Cyan << "--- 📋 ÎMPRUMUTURI ȘI RETURURI ---" << Color::Reset << "\n";
    std::cout << "  20. Împrumută            22. Listă împrumuturi\n";
    std::cout << "  21. Returnează           23. Confirmă returnări";
    if (bib.getNumarReturnariInAsteptare() > 0)
        std::cout << Color::Yellow << " [" << bib.getNumarReturnariInAsteptare() << " în așteptare!]" << Color::Reset;
    std::cout << "\n  24. Confirmă Plăți Amenzi";
    if (bib.getNumarPlatiInAsteptare() > 0)
        std::cout << Color::Yellow << " [" << bib.getNumarPlatiInAsteptare() << " noi!]" << Color::Reset;
    std::cout << "\n\n  " << Color::Cyan << "--- ⚙️ SISTEM ---" << Color::Reset << "\n";
    std::cout << "   0. Delogare\n\n";
    
    int opt = citesteInt("  ➤ Alege o opțiune: ");
    switch (opt) {
        case 1: std::cout<<"1.Fizica 2.Digitala\n"; if(citesteInt("  > ")==1) adaugaCarteFizicaUI(bib); else adaugaCarteDigitalaUI(bib); break;
        case 2: stergeCarteUI(bib); break;
        case 3: bib.afiseazaInventarScurt(std::cout); break;
        case 4: cautaCarteUI(bib); break;
        case 10: bib.afiseazaUtilizatoriScurt(std::cout); break;
        case 20: imprumutaCarteUI(bib); break;
        case 21: returneazaCarteUI(bib); break;
        case 22: bib.afiseazaToateImprumuturile(std::cout); break;
        case 23: confirmaReturnariUI(bib); break;
        case 24: confirmaPlatiUI(bib); break;
        case 0: return false;
    }
    if(opt != 0) pausare();
    return true;
}


static bool MeniuIngrijitor(Biblioteca& bib, const std::shared_ptr<Utilizator>& u) {
    afiseazaBanner(u);
    std::cout << "  " << Color::Bold << "[ Îngrijitor ]" << Color::Reset << " Vizualizare resurse\n\n";
    std::cout << "  " << Color::Cyan << "--- 📋 RESURSE ---" << Color::Reset << "\n";
    std::cout << "   1. Inventar cărți\n";
    std::cout << "   2. Listă simplificată colegi\n";
    std::cout << "   3. Repară cărți defecte\n\n";
    std::cout << "  " << Color::Cyan << "--- ⚙️ SISTEM ---" << Color::Reset << "\n";
    std::cout << "   0. Delogare\n\n";
    
    int opt = citesteInt("  ➤ Alege o opțiune: ");
    switch (opt) {
        case 1: bib.afiseazaInventarScurt(std::cout); break;
        case 2: bib.afiseazaUtilizatoriScurt(std::cout); break;
        case 3: reconditioneazaCartiUI(bib); break;
        case 0: return false;
    }
    if(opt != 0) pausare();
    return true;
}

static bool MeniuCititor(Biblioteca& bib, const std::shared_ptr<Utilizator>& u) {
    afiseazaBanner(u);
    std::cout << "  " << Color::Bold << "[ Cititor ]" << Color::Reset << " Acces public\n\n";
    std::cout << "  " << Color::Cyan << "--- 📚 CATALOG ---" << Color::Reset << "\n";
    std::cout << "   1. Inventar cărți\n";
    std::cout << "   2. Caută carte\n\n";
    std::cout << "  " << Color::Cyan << "--- 📋 ÎMPRUMUTURILE MELE ---" << Color::Reset << "\n";
    std::cout << "   3. Vezi împrumuturile mele\n";
    std::cout << "   4. Împrumută o carte\n";
    std::cout << "   5. Returnează o carte\n\n";

    auto cititor = std::dynamic_pointer_cast<Cititor>(u);
    if (cititor && cititor->getPenalizari() > 0) {
        std::cout << "  " << Color::Red << "⚠  AMENDĂ DE PLATĂ: " << cititor->getPenalizari() << " RON" << Color::Reset << "\n";
        std::cout << "   6. Solicită plată amendă\n\n";
    }

    std::cout << "  " << Color::Cyan << "--- ⚙️ CONT ---" << Color::Reset << "\n";
    std::cout << "   0. Delogare\n\n";
    
    int opt = citesteInt("  ➤ Alege o opțiune: ");
    switch (opt) {
        case 1: bib.afiseazaInventarScurt(std::cout); break;
        case 2: cautaCarteUI(bib); break;
        case 3: bib.afiseazaImprumuturiCititor(std::cout, u->getId()); break;
        case 4: imprumutaCarteCititorUI(bib, u->getId()); break;
        case 5: returneazaCarteCititorUI(bib, u->getId()); break;
        case 6: solicitaPlataUI(bib, u); break;
        case 0: return false;
    }
    if(opt != 0) pausare();
    return true;
}

// ═══════════════════════════════════════════════
//  MAIN (Login Loop)
// ═══════════════════════════════════════════════

int main() {
    Biblioteca bib("db_carti.txt", "db_imprumuturi.txt", "db_utilizatori.txt", "db_returnari.txt", "db_plati.txt");

    while (true) {
        afiseazaBanner();
        std::cout << "  ── Autentificare ────────────────────\n\n";
        std::cout << "  Introduceți 'exit' ca ID pentru a închide aplicația.\n\n";
        
        std::string id = citesteLinie("  ID Utilizator: ");
        if (id == "exit") break;
        std::string p = citesteLinie("  Parola: ");

        auto u = bib.autentificare(id, p);
        if (!u) {
            std::cout << "\n  ❌ Autentificare eșuată! ID sau parolă incorectă.\n";
            pausare();
            continue;
        }

        std::cout << "\n  " << Color::Green << "✅ Autentificare reușită! Bine ai venit, " << u->getPrenume() << "!" << Color::Reset << "\n";
        pausare();

        std::string tip = u->getTip();
        bool conectat = true;

        while (conectat) {
            if (tip == "DIRECTOR") conectat = MeniuDirector(bib, u);
            else if (tip == "BIBLIOTECAR") conectat = MeniuBibliotecar(bib, u);
            else if (tip == "INGRIJITOR") conectat = MeniuIngrijitor(bib, u);
            else if (tip == "CITITOR") conectat = MeniuCititor(bib, u);
        }
        
        // La delogare se salveaza mereu datele (in caz ca s-au facut modificari)
        bib.salveazaDate();
    }

    bib.salveazaDate();
    std::cout << "\n  💾 La revedere! 👋\n\n";
    return 0;
}
