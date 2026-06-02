#include "Biblioteca.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <cstdio>

Biblioteca::Biblioteca(const std::string& fisier_carti,
                       const std::string& fisier_imprumuturi,
                       const std::string& fisier_utilizatori,
                       const std::string& fisier_returnari,
                       const std::string& fisier_plati)
    : fisier_carti(fisier_carti), fisier_imprumuturi(fisier_imprumuturi),
      fisier_utilizatori(fisier_utilizatori), fisier_returnari(fisier_returnari),
      fisier_plati(fisier_plati) {
    incarcaDate();
}

Biblioteca::~Biblioteca() {
    salveazaDate();
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

    if (tip == "FIZICA" && campuri.size() >= 20) {
        auto autori = Carte::stringToAutori(campuri[2]);
        double pret = std::stod(campuri[4]);
        StareCarte stare = Carte::stringToStare(campuri[7]);
        int stoc_disp = std::stoi(campuri[8]);
        int stoc_tot = std::stoi(campuri[9]);
        int an = std::stoi(campuri[11]);
        int pag = std::stoi(campuri[12]);
        double greutate = std::stod(campuri[14]);
        Locatie loc(campuri[16], campuri[17], campuri[18], campuri[19]);

        return std::make_shared<CarteFizica>(
            campuri[1], autori, campuri[3], pret, campuri[5], campuri[6],
            stare, stoc_tot, stoc_disp, campuri[10], an, pag,
            campuri[13], greutate, campuri[15], loc
        );
    }
    else if (tip == "DIGITALA" && campuri.size() >= 16) {
        auto autori = Carte::stringToAutori(campuri[2]);
        double pret = std::stod(campuri[4]);
        StareCarte stare = Carte::stringToStare(campuri[7]);
        int stoc_disp = std::stoi(campuri[8]);
        int stoc_tot = std::stoi(campuri[9]);
        int an = std::stoi(campuri[11]);
        int pag = std::stoi(campuri[12]);
        double dim_mb = std::stod(campuri[14]);

        return std::make_shared<CarteDigitala>(
            campuri[1], autori, campuri[3], pret, campuri[5], campuri[6],
            stare, stoc_tot, stoc_disp, campuri[10], an, pag,
            campuri[13], dim_mb, campuri[15]
        );
    }
    return nullptr;
}

time_t Biblioteca::stringToTime(const std::string& data_str) {
    struct tm tm = {0};
    int d, m, y;
    if (sscanf(data_str.c_str(), "%d/%d/%d", &d, &m, &y) != 3) {
        return 0;
    }
    tm.tm_mday = d;
    tm.tm_mon = m - 1;
    tm.tm_year = y - 1900;
    return mktime(&tm);
}

int Biblioteca::calculeazaZileIntarziere(const std::string& data_limita_str) {
    time_t t_limita = stringToTime(data_limita_str);
    if (t_limita == 0) return 0;

    time_t t_acum = time(0);
    double secunde = difftime(t_acum, t_limita);
    if (secunde <= 0) return 0;

    return static_cast<int>(secunde / (60 * 60 * 24));
}

std::shared_ptr<Utilizator> Biblioteca::parseazaLinieUtilizator(const std::string& linie) const {
    auto c = splitLinie(linie, '|');
    if (c.empty()) return nullptr;

    std::string tip = c[0];

    // Format comun: TIP|id|parola|nume|prenume|cnp|email|telefon|adresa|activ|...specific...
    // Director/Bibliotecar/Ingrijitor au 15 campuri. Cititor are 16.
    if (c.size() < 10) return nullptr;

    bool activ = (c[9] == "1");

    if (tip == "DIRECTOR" && c.size() >= 15) {
        return std::make_shared<Director>(
            c[1], c[2], c[3], c[4], c[5], c[6], c[7], c[8], activ,
            c[10], std::stod(c[11]), c[12], c[13], std::stoi(c[14])
        );
    }
    else if (tip == "BIBLIOTECAR" && c.size() >= 15) {
        return std::make_shared<Bibliotecar>(
            c[1], c[2], c[3], c[4], c[5], c[6], c[7], c[8], activ,
            c[10], std::stod(c[11]), c[12], c[13], std::stoi(c[14])
        );
    }
    else if (tip == "INGRIJITOR" && c.size() >= 15) {
        return std::make_shared<Ingrijitor>(
            c[1], c[2], c[3], c[4], c[5], c[6], c[7], c[8], activ,
            c[10], std::stod(c[11]), c[12], c[13], c[14]
        );
    }
    else if (tip == "CITITOR" && c.size() >= 16) {
        return std::make_shared<Cititor>(
            c[1], c[2], c[3], c[4], c[5], c[6], c[7], c[8], activ,
            c[10], c[11], c[12], std::stoi(c[13]), std::stoi(c[14]), std::stod(c[15])
        );
    }

    return nullptr;
}

void Biblioteca::incarcaDate() {
    std::ifstream fin_carti(fisier_carti);
    if (fin_carti.is_open()) {
        std::string linie;
        while (std::getline(fin_carti, linie)) {
            if (linie.empty()) continue;
            auto carte = parseazaLinieCarte(linie);
            if (carte) carti.push_back(carte);
        }
        fin_carti.close();
    }

    std::ifstream fin_util(fisier_utilizatori);
    if (fin_util.is_open()) {
        std::string linie;
        while (std::getline(fin_util, linie)) {
            if (linie.empty()) continue;
            auto util = parseazaLinieUtilizator(linie);
            if (util) utilizatori.push_back(util);
        }
        fin_util.close();
    }

    std::ifstream fin_imp(fisier_imprumuturi);
    if (fin_imp.is_open()) {
        std::string linie;
        while (std::getline(fin_imp, linie)) {
            if (linie.empty()) continue;
            Imprumut imp = Imprumut::dinLinieFisier(linie);
            if (!imp.getIdCarte().empty()) {
                imprumuturi.push_back(imp);
            }
        }
        fin_imp.close();
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
}

void Biblioteca::salveazaDate() const {
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
        fout_plati.close();
    }
}

// Autentificare
std::shared_ptr<Utilizator> Biblioteca::autentificare(const std::string& id, const std::string& parola) const {
    for (const auto& u : utilizatori) {
        if (u->getId() == id && u->verificaParola(parola) && u->getActiv()) {
            return u;
        }
    }
    return nullptr;
}


// Cărți (fără modificări structurale)
void Biblioteca::adaugaCarteFizica(const std::string& titlu, const std::vector<std::string>& autori,
                                    const std::string& isbn, double pret, const std::string& serie,
                                    const std::string& poza, StareCarte stare, int stoc_tot, int stoc_disp,
                                    const std::string& categorie, int an, int pagini,
                                    const std::string& dimensiuni, double greutate,
                                    const std::string& coperta, const Locatie& loc) {
    carti.push_back(std::make_shared<CarteFizica>(
        titlu, autori, isbn, pret, serie, poza, stare, stoc_tot, stoc_disp,
        categorie, an, pagini, dimensiuni, greutate, coperta, loc
    ));
}

void Biblioteca::adaugaCarteDigitala(const std::string& titlu, const std::vector<std::string>& autori,
                                      const std::string& isbn, double pret, const std::string& serie,
                                      const std::string& poza, StareCarte stare, int stoc_tot, int stoc_disp,
                                      const std::string& categorie, int an, int pagini,
                                      const std::string& format_digital, double dimensiune_mb,
                                      const std::string& link_acces) {
    carti.push_back(std::make_shared<CarteDigitala>(
        titlu, autori, isbn, pret, serie, poza, stare, stoc_tot, stoc_disp,
        categorie, an, pagini, format_digital, dimensiune_mb, link_acces
    ));
}

bool Biblioteca::stergeCarte(const std::string& isbn) {
    auto it = std::remove_if(carti.begin(), carti.end(),
        [&isbn](const std::shared_ptr<Carte>& c) { return c->getIsbn() == isbn; });

    if (it != carti.end()) {
        carti.erase(it, carti.end());
        auto it_imp = std::remove_if(imprumuturi.begin(), imprumuturi.end(),
            [&isbn](const Imprumut& imp) { return imp.getIdCarte() == isbn; });
        imprumuturi.erase(it_imp, imprumuturi.end());
        return true;
    }
    return false;
}

bool Biblioteca::reparaCarte(const std::string& isbn) {
    auto carte = gasesteCarte(isbn);
    if (carte) {
        carte->setStareCarte(StareCarte::BUNA);
        return true;
    }
    return false;
}

std::shared_ptr<Carte> Biblioteca::gasesteCarte(const std::string& isbn) const {
    for (const auto& c : carti) {
        if (c->getIsbn() == isbn) return c;
    }
    return nullptr;
}


// Utilizatori (cu parola)
void Biblioteca::adaugaDirector(const std::string& id, const std::string& parola, const std::string& nume, const std::string& prenume,
                                const std::string& cnp, const std::string& email, const std::string& telefon,
                                const std::string& adresa,
                                const std::string& departament, double salariu,
                                const std::string& data_angajare, const std::string& birou, int nivel_acces) {
    utilizatori.push_back(std::make_shared<Director>(
        id, parola, nume, prenume, cnp, email, telefon, adresa, true,
        departament, salariu, data_angajare, birou, nivel_acces
    ));
}

void Biblioteca::adaugaBibliotecar(const std::string& id, const std::string& parola, const std::string& nume, const std::string& prenume,
                                   const std::string& cnp, const std::string& email, const std::string& telefon,
                                   const std::string& adresa,
                                   const std::string& sectie, double salariu,
                                   const std::string& data_angajare, const std::string& program_lucru,
                                   int nr_carti_gestionate) {
    utilizatori.push_back(std::make_shared<Bibliotecar>(
        id, parola, nume, prenume, cnp, email, telefon, adresa, true,
        sectie, salariu, data_angajare, program_lucru, nr_carti_gestionate
    ));
}

void Biblioteca::adaugaIngrijitor(const std::string& id, const std::string& parola, const std::string& nume, const std::string& prenume,
                                  const std::string& cnp, const std::string& email, const std::string& telefon,
                                  const std::string& adresa,
                                  const std::string& zona, double salariu,
                                  const std::string& data_angajare, const std::string& program_lucru,
                                  const std::string& echipament) {
    utilizatori.push_back(std::make_shared<Ingrijitor>(
        id, parola, nume, prenume, cnp, email, telefon, adresa, true,
        zona, salariu, data_angajare, program_lucru, echipament
    ));
}

void Biblioteca::adaugaCititor(const std::string& id, const std::string& parola, const std::string& nume, const std::string& prenume,
                               const std::string& cnp, const std::string& email, const std::string& telefon,
                               const std::string& adresa,
                               const std::string& tip_abonament, const std::string& data_inregistrare,
                               const std::string& data_expirare, int max_carti) {
    utilizatori.push_back(std::make_shared<Cititor>(
        id, parola, nume, prenume, cnp, email, telefon, adresa, true,
        tip_abonament, data_inregistrare, data_expirare, max_carti, 0, 0.0
    ));
}

bool Biblioteca::stergeUtilizator(const std::string& id) {
    auto it = std::remove_if(utilizatori.begin(), utilizatori.end(),
        [&id](const std::shared_ptr<Utilizator>& u) { return u->getId() == id; });

    if (it != utilizatori.end()) {
        utilizatori.erase(it, utilizatori.end());
        return true;
    }
    return false;
}

std::shared_ptr<Utilizator> Biblioteca::gasesteUtilizator(const std::string& id) const {
    for (const auto& u : utilizatori) {
        if (u->getId() == id) return u;
    }
    return nullptr;
}

bool Biblioteca::platestePenalizari(const std::string& id_cititor) {
    auto cititor = gasesteCititor(id_cititor);
    if (cititor) {
        cititor->setPenalizari(0.0);
        return true;
    }
    return false;
}

std::shared_ptr<Cititor> Biblioteca::gasesteCititor(const std::string& id) const {
    auto util = gasesteUtilizator(id);
    if (util && util->getTip() == "CITITOR") {
        return std::dynamic_pointer_cast<Cititor>(util);
    }
    return nullptr;
}

std::string Biblioteca::getIsbnDupaIndex(size_t index) const {
    if (index > 0 && index <= carti.size()) {
        return carti[index - 1]->getIsbn();
    }
    return "";
}

int Biblioteca::obtineNumarCartiFiziceRaft(const Locatie& loc) const {
    int count = 0;
    for (const auto& c : carti) {
        if (c->getTip() == "FIZICA") {
            auto cf = std::dynamic_pointer_cast<CarteFizica>(c);
            if (cf) {
                Locatie cloc = cf->getLocatie();
                if (cloc.cladire == loc.cladire && cloc.camera == loc.camera &&
                    cloc.culoar == loc.culoar && cloc.raft == loc.raft) {
                    count++;
                }
            }
        }
    }
    return count;
}

// Căutare Cărți
static std::string toLower(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::vector<std::shared_ptr<Carte>> Biblioteca::cautaDupaAutor(const std::string& autor) const {
    std::vector<std::shared_ptr<Carte>> rezultate;
    std::string autor_lower = toLower(autor);
    for (const auto& c : carti) {
        for (const auto& a : c->getAutori()) {
            if (toLower(a).find(autor_lower) != std::string::npos) {
                rezultate.push_back(c);
                break;
            }
        }
    }
    return rezultate;
}

std::vector<std::shared_ptr<Carte>> Biblioteca::cautaDupaNume(const std::string& nume) const {
    std::vector<std::shared_ptr<Carte>> rezultate;
    std::string n = toLower(nume);
    for (const auto& c : carti) {
        if (toLower(c->getTitlu()).find(n) != std::string::npos) rezultate.push_back(c);
    }
    return rezultate;
}

std::vector<std::shared_ptr<Carte>> Biblioteca::cautaDupaCategorie(const std::string& categorie) const {
    std::vector<std::shared_ptr<Carte>> rezultate;
    std::string cat = toLower(categorie);
    for (const auto& c : carti) {
        if (toLower(c->getCategorie()).find(cat) != std::string::npos) rezultate.push_back(c);
    }
    return rezultate;
}

std::vector<std::shared_ptr<Carte>> Biblioteca::cautaDupaAn(int an) const {
    std::vector<std::shared_ptr<Carte>> rezultate;
    for (const auto& c : carti) {
        if (c->getAnAparitie() == an) rezultate.push_back(c);
    }
    return rezultate;
}

std::vector<std::shared_ptr<Carte>> Biblioteca::cautaDupaPagini(int min_pag, int max_pag) const {
    std::vector<std::shared_ptr<Carte>> rezultate;
    for (const auto& c : carti) {
        int pag = c->getNrPagini();
        if (pag >= min_pag && pag <= max_pag) rezultate.push_back(c);
    }
    return rezultate;
}

// Căutare Utilizatori
std::vector<std::shared_ptr<Utilizator>> Biblioteca::cautaUtilizatorDupaNume(const std::string& nume) const {
    std::vector<std::shared_ptr<Utilizator>> rezultate;
    std::string n = toLower(nume);
    for (const auto& u : utilizatori) {
        if (toLower(u->getNumeComplet()).find(n) != std::string::npos) {
            rezultate.push_back(u);
        }
    }
    return rezultate;
}

std::vector<std::shared_ptr<Utilizator>> Biblioteca::cautaUtilizatorDupaRol(const std::string& rol) const {
    std::vector<std::shared_ptr<Utilizator>> rezultate;
    std::string r = toLower(rol);
    for (const auto& u : utilizatori) {
        if (toLower(u->getTip()).find(r) != std::string::npos) {
            rezultate.push_back(u);
        }
    }
    return rezultate;
}

// Împrumuturi
bool Biblioteca::adaugaImprumut(const std::string& isbn, const std::string& id_cititor,
                                 const std::string& data_imp, const std::string& termen,
                                 const std::string& obs) {
    auto carte = gasesteCarte(isbn);
    if (!carte) {
        std::cerr << "  [EROARE] Cartea cu ISBN '" << isbn << "' nu a fost găsită.\n";
        return false;
    }
    if (carte->getStocDisponibil() <= 0) {
        std::cerr << "  [EROARE] Cartea '" << carte->getTitlu() << "' nu mai are exemplare disponibile (Stoc: 0).\n";
        return false;
    }

    auto cititor = gasesteCititor(id_cititor);
    if (!cititor) {
        std::cerr << "  [EROARE] Cititorul cu ID '" << id_cititor << "' nu a fost găsit.\n";
        return false;
    }
    if (!cititor->poateImprumuta()) {
        std::cerr << "  [EROARE] Cititorul '" << cititor->getNumeComplet()
                  << "' nu poate împrumuta (limită atinsă sau penalizări active).\n";
        return false;
    }

    carte->setStocDisponibil(carte->getStocDisponibil() - 1);
    cititor->incrementeazaImprumuturi();
    imprumuturi.emplace_back(isbn, id_cititor, cititor->getNumeComplet(), data_imp, termen, obs);
    return true;
}

bool Biblioteca::returneazaCarte(const std::string& isbn, const std::string& id_cititor) {
    auto it = std::find_if(imprumuturi.begin(), imprumuturi.end(),
        [&isbn, &id_cititor](const Imprumut& imp) {
            return imp.getIdCarte() == isbn && imp.getIdCititor() == id_cititor;
        });

    if (it != imprumuturi.end()) {
        imprumuturi.erase(it);
        auto carte = gasesteCarte(isbn);
        if (carte) carte->setStocDisponibil(carte->getStocDisponibil() + 1);
        auto cititor = gasesteCititor(id_cititor);
        if (cititor) cititor->decrementeazaImprumuturi();
        return true;
    }
    return false;
}

const Imprumut* Biblioteca::getImprumutDupaIndex(size_t index) const {
    if (index > 0 && index <= imprumuturi.size()) {
        return &imprumuturi[index - 1];
    }
    return nullptr;
}

// ─── Returnare in 2 pasi ─────────────────────────────────────────────────────

bool Biblioteca::solicitaReturnare(const std::string& isbn, const std::string& id_cititor) {
    auto it = std::find_if(imprumuturi.begin(), imprumuturi.end(),
        [&isbn, &id_cititor](const Imprumut& imp) {
            return imp.getIdCarte() == isbn && imp.getIdCititor() == id_cititor;
        });
    if (it == imprumuturi.end()) return false;

    // Calculăm amenda de întârziere înainte de a șterge împrumutul
    int zile = calculeazaZileIntarziere(it->getTermenLimita());
    double amenda = zile * 2.0;

    std::string nume_cititor = it->getNumeCititor();
    imprumuturi.erase(it);

    // Cititor a predat cartea la ghiseu – decrementam contorul
    auto cititor = gasesteCititor(id_cititor);
    if (cititor) cititor->decrementeazaImprumuturi();

    // Cartea ramane indisponibila pana confirma bibliotecar
    auto carte = gasesteCarte(isbn);
    std::string titlu = carte ? carte->getTitlu() : isbn;

    time_t rawtime;
    struct tm* ti;
    char buf[20];
    time(&rawtime);
    ti = localtime(&rawtime);
    strftime(buf, sizeof(buf), "%d/%m/%Y", ti);

    ReturnareInAsteptare r;
    r.isbn           = isbn;
    r.id_cititor     = id_cititor;
    r.nume_cititor   = nume_cititor;
    r.titlu_carte    = titlu;
    r.data_returnare = std::string(buf);
    r.amenda_intarziere = amenda;
    returnari_in_asteptare.push_back(r);
    return true;
}

bool Biblioteca::confirmaReturnare(size_t index) {
    if (index == 0 || index > returnari_in_asteptare.size()) return false;
    const ReturnareInAsteptare& r = returnari_in_asteptare[index - 1];
    
    // Aplicăm amenda de întârziere (calculată în solicitaReturnare)
    auto cititor = gasesteCititor(r.id_cititor);
    if (cititor && r.amenda_intarziere > 0) {
        cititor->setPenalizari(cititor->getPenalizari() + r.amenda_intarziere);
    }
    
    auto carte = gasesteCarte(r.isbn);
    if (carte) carte->setStocDisponibil(carte->getStocDisponibil() + 1);
    returnari_in_asteptare.erase(returnari_in_asteptare.begin() + static_cast<int>(index - 1));
    return true;
}

bool Biblioteca::refuzaReturnareDefecta(size_t index) {
    if (index == 0 || index > returnari_in_asteptare.size()) return false;
    const ReturnareInAsteptare& r = returnari_in_asteptare[index - 1];
    auto carte = gasesteCarte(r.isbn);
    if (carte) {
        carte->setStocDisponibil(carte->getStocDisponibil() + 1);       // reintră în stoc
        carte->setStareCarte(StareCarte::DEFECTA); // dar marcată defectă
        
        // Aplicăm amenda pentru deteriorare (prețul cărții)
        auto cititor = gasesteCititor(r.id_cititor);
        if (cititor) {
            cititor->setPenalizari(cititor->getPenalizari() + carte->getPretIntrare());
        }
    }
    returnari_in_asteptare.erase(returnari_in_asteptare.begin() + static_cast<int>(index - 1));
    return true;
}

void Biblioteca::afiseazaReturnariInAsteptare(std::ostream& os) const {
    if (returnari_in_asteptare.empty()) {
        os << "\n  \u26a0  Nu exista returnari in asteptare.\n\n";
        return;
    }
    os << "\n";
    os << Color::Cyan;
    os << "  ╔══════════════════════════════════════════════════════════════════════════════════════════╗\n";
    os << "  ║                                 📋 RETURNĂRI ÎN AȘTEPTARE                                ║\n";
    os << "  ╠════╦══════════════════════════════╦═══════════╦═════════════════════╦════════════════════╣\n";
    os << "  ║ Nr ║ Titlu carte                  ║ ISBN      ║ Cititor             ║ Data ret.          ║\n";
    os << "  ╠════╬══════════════════════════════╬═══════════╬═════════════════════╬════════════════════╣\n";
    os << Color::Reset;
    for (size_t i = 0; i < returnari_in_asteptare.size(); ++i) {
        const auto& r = returnari_in_asteptare[i];
        std::string titlu = r.titlu_carte;
        if (titlu.length() > 28) titlu = titlu.substr(0, 25) + "...";
        std::string isbn = r.isbn;
        if (isbn.length() > 9) isbn = isbn.substr(0, 7) + "..";
        std::string cititor = r.nume_cititor;
        if (cititor.length() > 19) cititor = cititor.substr(0, 17) + "..";
        os << "  ║ " << std::setw(2) << (i + 1) << " ║ "
           << std::setw(28) << std::left << titlu << " ║ "
           << std::setw(9)  << std::left << isbn   << " ║ "
           << std::setw(19) << std::left << cititor << " ║ "
           << std::setw(18) << std::left << r.data_returnare << " ║\n";
    }
    os << Color::Cyan << "  ╚════╩══════════════════════════════╩═══════════╩═════════════════════╩════════════════════╝\n" << Color::Reset;
    os << "  Total în așteptare: " << returnari_in_asteptare.size() << "\n\n";
}

size_t Biblioteca::getNumarReturnariInAsteptare() const {
    return returnari_in_asteptare.size();
}

bool Biblioteca::solicitaPlata(const std::string& id_cititor) {
    auto cititor = gasesteCititor(id_cititor);
    if (!cititor || cititor->getPenalizari() <= 0) return false;

    // Verificăm dacă există deja o cerere pentru acest cititor
    for (const auto& p : plati_in_asteptare) {
        if (p.id_cititor == id_cititor) return false;
    }

    time_t rawtime;
    struct tm* ti;
    char buf[20];
    time(&rawtime);
    ti = localtime(&rawtime);
    strftime(buf, sizeof(buf), "%d/%m/%Y", ti);

    PlataInAsteptare p;
    p.id_cititor = id_cititor;
    p.nume_cititor = cititor->getNumeComplet();
    p.suma = cititor->getPenalizari();
    p.data_solicitare = std::string(buf);

    plati_in_asteptare.push_back(p);
    return true;
}

bool Biblioteca::confirmaPlata(size_t index) {
    if (index == 0 || index > plati_in_asteptare.size()) return false;
    const PlataInAsteptare& p = plati_in_asteptare[index - 1];
    
    if (platestePenalizari(p.id_cititor)) {
        plati_in_asteptare.erase(plati_in_asteptare.begin() + static_cast<int>(index - 1));
        return true;
    }
    return false;
}

bool Biblioteca::refuzaPlata(size_t index) {
    if (index == 0 || index > plati_in_asteptare.size()) return false;
    plati_in_asteptare.erase(plati_in_asteptare.begin() + static_cast<int>(index - 1));
    return true;
}

size_t Biblioteca::getNumarPlatiInAsteptare() const {
    return plati_in_asteptare.size();
}

void Biblioteca::afiseazaPlatiInAsteptare(std::ostream& os) const {
    if (plati_in_asteptare.empty()) {
        os << "\n  ⚠  Nu există cereri de plată în așteptare.\n\n";
        return;
    }
    os << "\n";
    os << Color::Cyan;
    os << "  ╔══════════════════════════════════════════════════════════════════════════════════════════╗\n";
    os << "  ║                                 📋 PLĂȚI ÎN AȘTEPTARE                                    ║\n";
    os << "  ╠════╦══════════════════════════╦══════════════╦═════════════════════╦════════════════════╣\n";
    os << "  ║ Nr ║ Nume Cititor             ║ Sumă (RON)   ║ ID Cititor          ║ Data Solicitării   ║\n";
    os << "  ╠════╬══════════════════════════╬══════════════╬═════════════════════╬════════════════════╣\n";
    os << Color::Reset;

    for (size_t i = 0; i < plati_in_asteptare.size(); ++i) {
        const auto& p = plati_in_asteptare[i];
        std::string nume = p.nume_cititor;
        if (nume.length() > 24) nume = nume.substr(0, 21) + "..";
        
        os << "  ║ " << std::setw(2) << (i + 1) << " ║ "
           << std::setw(24) << std::left << nume << " ║ "
           << std::setw(12) << std::right << std::fixed << std::setprecision(2) << p.suma << " ║ "
           << std::setw(19) << std::left << p.id_cititor << " ║ "
           << std::setw(18) << std::left << p.data_solicitare << " ║\n";
    }
    os << Color::Cyan << "  ╚════╩══════════════════════════╩══════════════╩═════════════════════╩════════════════════╝\n" << Color::Reset;
    os << "  Total cereri: " << plati_in_asteptare.size() << "\n\n";
}

// Afișare
void Biblioteca::afiseazaToateCartile(std::ostream& os) const {
    if (carti.empty()) { os << "\n  ⚠  Nu există cărți în bibliotecă.\n\n"; return; }
    os << "\n";
    for (size_t i = 0; i < carti.size(); ++i) {
        os << "  [" << (i + 1) << "]\n";
        carti[i]->afisare(os);
        os << "\n";
    }
}

void Biblioteca::afiseazaToateImprumuturile(std::ostream& os) const {
    if (imprumuturi.empty()) { os << "\n  ⚠  Nu există împrumuturi active.\n\n"; return; }
    os << "\n";
    for (size_t i = 0; i < imprumuturi.size(); ++i) {
        os << "  [Nr Împrumut: " << (i + 1) << "]\n";
        imprumuturi[i].afisare(os);
        os << "\n";
    }
}

void Biblioteca::afiseazaImprumuturiCititor(std::ostream& os, const std::string& id_cititor) const {
    bool gasit = false;
    os << "\n";
    for (size_t i = 0; i < imprumuturi.size(); ++i) {
        if (imprumuturi[i].getIdCititor() == id_cititor) {
            os << "  [Nr Împrumut: " << (i + 1) << "]\n";
            imprumuturi[i].afisare(os);
            os << "\n";
            gasit = true;
        }
    }
    if (!gasit) { os << "  ⚠  Nu aveți niciun împrumut activ.\n\n"; }
}

void Biblioteca::afiseazaInventarScurt(std::ostream& os) const {
    os << "\n";
    os << Color::Cyan;
    os << "  ╔═════════════════════════════════════════════════════════════════════════════════════════╗\n";
    os << "  ║                                 📚  INVENTAR BIBLIOTECĂ                                 ║\n";
    os << "  ╠════╦══════════════════════╦════════════════╦════════╦══════════════════╦════════════════╣\n";
    os << "  ║ Nr ║ Titlu                ║ ISBN           ║ Tip    ║ Locație          ║ Stoc (Disp/Tot)║\n";
    os << "  ╠════╬══════════════════════╬════════════════╬════════╬══════════════════╬════════════════╣\n";
    os << Color::Reset;

    for (size_t i = 0; i < carti.size(); ++i) {
        std::string titlu = carti[i]->getTitlu();
        if (titlu.length() > 20) titlu = titlu.substr(0, 17) + "...";
        std::string isbn = carti[i]->getIsbn();
        if (isbn.length() > 14) isbn = isbn.substr(0, 11) + "...";
        std::string loc = carti[i]->getLocatieScurta();
        if (loc.length() > 16) loc = loc.substr(0, 13) + "...";

        std::string stoc_str;
        if (carti[i]->getStocTotal() >= 900) {
            stoc_str = "Infinit";
        } else {
            stoc_str = std::to_string(carti[i]->getStocDisponibil()) + "/" + std::to_string(carti[i]->getStocTotal());
            if (carti[i]->getStareCarte() == StareCarte::DEFECTA) {
                stoc_str += " (!)";
            }
        }

        std::string color_stoc = (carti[i]->getStocDisponibil() > 0) ? Color::Green : Color::Red;

        os << "  ║ " << std::setw(2) << (i + 1) << " ║ "
           << std::setw(20) << std::left << titlu << " ║ "
           << std::setw(14) << std::left << isbn << " ║ "
           << std::setw(6) << std::left << (carti[i]->getTip() == "FIZICA" ? "Fizic" : "Digit") << " ║ "
           << std::setw(16) << std::left << loc << " ║ "
           << color_stoc
           << std::setw(14) << std::left << stoc_str
           << Color::Reset << " ║\n";
    }

    os << Color::Cyan << "  ╚════╩══════════════════════╩════════════════╩════════╩══════════════════╩════════════════╝\n" << Color::Reset;
    os << "  Total tipuri cărți: " << carti.size()
       << " | Exemplare disponibile: " << getNumarDisponibile()
       << " | Total exemplare fizice: " << getNumarCarti() << "\n\n";
}

void Biblioteca::afiseazaTotiUtilizatorii(std::ostream& os) const {
    if (utilizatori.empty()) { os << "\n  ⚠  Nu există utilizatori înregistrați.\n\n"; return; }
    os << "\n";
    for (size_t i = 0; i < utilizatori.size(); ++i) {
        os << "  [" << (i + 1) << "]\n";
        utilizatori[i]->afisare(os);
        os << "\n";
    }
}

void Biblioteca::afiseazaUtilizatoriScurt(std::ostream& os) const {
    os << "\n";
    os << Color::Cyan;
    os << "  ╔══════════════════════════════════════════════════════════════════════════════╗\n";
    os << "  ║                         👥  REGISTRU UTILIZATORI                            ║\n";
    os << "  ╠════╦══════════╦══════════════════════════╦══════════════╦════════════════════╣\n";
    os << "  ║ Nr ║ ID       ║ Nume complet             ║ Rol          ║ Status             ║\n";
    os << "  ╠════╬══════════╬══════════════════════════╬══════════════╬════════════════════╣\n";
    os << Color::Reset;

    for (size_t i = 0; i < utilizatori.size(); ++i) {
        std::string nume = utilizatori[i]->getNumeComplet();
        if (nume.length() > 24) nume = nume.substr(0, 21) + "...";
        std::string id = utilizatori[i]->getId();
        if (id.length() > 8) id = id.substr(0, 5) + "...";

        os << "  ║ " << std::setw(2) << (i + 1) << " ║ "
           << std::setw(8) << std::left << id << " ║ "
           << std::setw(24) << std::left << nume << " ║ "
           << std::setw(12) << std::left << utilizatori[i]->getRol() << " ║ ";
        if (utilizatori[i]->getActiv()) {
            os << Color::Green << "✅ Activ            " << Color::Reset;
        } else {
            os << Color::Red << "❌ Inactiv          " << Color::Reset;
        }
        os << " ║\n";
    }

    os << Color::Cyan << "  ╚════╩══════════╩══════════════════════════╩══════════════╩════════════════════╝\n" << Color::Reset;
    os << "  Total: " << utilizatori.size()
       << " | Angajați: " << getNumarAngajati()
       << " | Cititori: " << getNumarCititori() << "\n\n";
}

size_t Biblioteca::getNumarCarti() const { 
    size_t total = 0;
    for (const auto& c : carti) {
        if (c->getStocTotal() < 900) total += c->getStocTotal();
    }
    return total; 
}
size_t Biblioteca::getNumarImprumuturi() const { return imprumuturi.size(); }
size_t Biblioteca::getNumarDisponibile() const {
    size_t total = 0;
    for (const auto& c : carti) {
        if (c->getStocTotal() < 900) total += c->getStocDisponibil();
    }
    return total;
}
size_t Biblioteca::getNumarUtilizatori() const { return utilizatori.size(); }
size_t Biblioteca::getNumarCititori() const {
    return std::count_if(utilizatori.begin(), utilizatori.end(),
        [](const std::shared_ptr<Utilizator>& u) { return u->getTip() == "CITITOR"; });
}
size_t Biblioteca::getNumarAngajati() const {
    return std::count_if(utilizatori.begin(), utilizatori.end(),
        [](const std::shared_ptr<Utilizator>& u) {
            return u->getTip() == "DIRECTOR" || u->getTip() == "BIBLIOTECAR" || u->getTip() == "INGRIJITOR";
        });
}
