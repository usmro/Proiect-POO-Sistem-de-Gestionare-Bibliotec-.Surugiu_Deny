#include "Carte.h"
#include <algorithm>

// ─────────────────────────────────────────────
//  Constructor implicit
// ─────────────────────────────────────────────
Carte::Carte()
    : titlu(""), issn(""), pret_intrare(0.0), serie_contabila(""),
      poza_path(""), stare_carte(StareCarte::BUNA), disponibilitate(true),
      categorie(""), an_aparitie(0), nr_pagini(0) {}

// ─────────────────────────────────────────────
//  Constructor parametrizat
// ─────────────────────────────────────────────
Carte::Carte(const std::string& titlu, const std::vector<std::string>& autori,
             const std::string& issn, double pret, const std::string& serie,
             const std::string& poza, StareCarte stare, bool disp,
             const std::string& categorie, int an_aparitie, int nr_pagini)
    : titlu(titlu), autori(autori), issn(issn), pret_intrare(pret),
      serie_contabila(serie), poza_path(poza), stare_carte(stare),
      disponibilitate(disp), categorie(categorie), an_aparitie(an_aparitie),
      nr_pagini(nr_pagini) {}

// ─────────────────────────────────────────────
//  Destructor virtual
// ─────────────────────────────────────────────
Carte::~Carte() {}

// ─────────────────────────────────────────────
//  Getteri
// ─────────────────────────────────────────────
std::string Carte::getTitlu() const { return titlu; }
std::vector<std::string> Carte::getAutori() const { return autori; }
std::string Carte::getIssn() const { return issn; }
double Carte::getPretIntrare() const { return pret_intrare; }
std::string Carte::getSerieContabila() const { return serie_contabila; }
std::string Carte::getPozaPath() const { return poza_path; }
StareCarte Carte::getStareCarte() const { return stare_carte; }
bool Carte::getDisponibilitate() const { return disponibilitate; }
std::string Carte::getCategorie() const { return categorie; }
int Carte::getAnAparitie() const { return an_aparitie; }
int Carte::getNrPagini() const { return nr_pagini; }

// ─────────────────────────────────────────────
//  Setteri
// ─────────────────────────────────────────────
void Carte::setTitlu(const std::string& t) { titlu = t; }
void Carte::setAutori(const std::vector<std::string>& a) { autori = a; }
void Carte::setIssn(const std::string& i) { issn = i; }
void Carte::setPretIntrare(double p) { pret_intrare = p; }
void Carte::setSerieContabila(const std::string& s) { serie_contabila = s; }
void Carte::setPozaPath(const std::string& p) { poza_path = p; }
void Carte::setStareCarte(StareCarte s) { stare_carte = s; }
void Carte::setDisponibilitate(bool d) { disponibilitate = d; }
void Carte::setCategorie(const std::string& c) { categorie = c; }
void Carte::setAnAparitie(int an) { an_aparitie = an; }
void Carte::setNrPagini(int nr) { nr_pagini = nr; }

// ─────────────────────────────────────────────
//  Utilități pentru autori (serializare/deserializare)
// ─────────────────────────────────────────────
std::string Carte::autoriToString() const {
    std::string result;
    for (size_t i = 0; i < autori.size(); ++i) {
        result += autori[i];
        if (i < autori.size() - 1) result += ",";
    }
    return result;
}

std::vector<std::string> Carte::stringToAutori(const std::string& s) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string token;
    while (std::getline(ss, token, ',')) {
        // Trim leading/trailing spaces
        size_t start = token.find_first_not_of(' ');
        size_t end = token.find_last_not_of(' ');
        if (start != std::string::npos) {
            result.push_back(token.substr(start, end - start + 1));
        }
    }
    return result;
}

// ─────────────────────────────────────────────
//  Conversie stare carte <-> string
// ─────────────────────────────────────────────
std::string Carte::stareToString(StareCarte s) {
    return (s == StareCarte::BUNA) ? "BUNA" : "DEFECTA";
}

StareCarte Carte::stringToStare(const std::string& s) {
    return (s == "DEFECTA") ? StareCarte::DEFECTA : StareCarte::BUNA;
}

// ─────────────────────────────────────────────
//  Supraîncărcare operator <<
// ─────────────────────────────────────────────
std::ostream& operator<<(std::ostream& os, const Carte& c) {
    c.afisare(os);
    return os;
}
