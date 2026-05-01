#include "CarteDigitala.h"
#include <iomanip>

// ─────────────────────────────────────────────
//  Constructor implicit
// ─────────────────────────────────────────────
CarteDigitala::CarteDigitala()
    : Carte(), format_digital(""), dimensiune_mb(0.0), link_acces("") {}

// ─────────────────────────────────────────────
//  Constructor parametrizat
// ─────────────────────────────────────────────
CarteDigitala::CarteDigitala(const std::string& titlu, const std::vector<std::string>& autori,
                             const std::string& issn, double pret, const std::string& serie,
                             const std::string& poza, StareCarte stare, bool disp,
                             const std::string& categorie, int an_aparitie, int nr_pagini,
                             const std::string& format_digital, double dimensiune_mb,
                             const std::string& link_acces)
    : Carte(titlu, autori, issn, pret, serie, poza, stare, disp, categorie, an_aparitie, nr_pagini),
      format_digital(format_digital), dimensiune_mb(dimensiune_mb), link_acces(link_acces) {}

// ─────────────────────────────────────────────
//  Destructor
// ─────────────────────────────────────────────
CarteDigitala::~CarteDigitala() {}

// ─────────────────────────────────────────────
//  Getteri
// ─────────────────────────────────────────────
std::string CarteDigitala::getFormatDigital() const { return format_digital; }
double CarteDigitala::getDimensiuneMb() const { return dimensiune_mb; }
std::string CarteDigitala::getLinkAcces() const { return link_acces; }

// ─────────────────────────────────────────────
//  Setteri
// ─────────────────────────────────────────────
void CarteDigitala::setFormatDigital(const std::string& f) { format_digital = f; }
void CarteDigitala::setDimensiuneMb(double d) { dimensiune_mb = d; }
void CarteDigitala::setLinkAcces(const std::string& l) { link_acces = l; }

// ─────────────────────────────────────────────
//  Afișare detaliată în consolă
// ─────────────────────────────────────────────
void CarteDigitala::afisare(std::ostream& os) const {
    os << "══════════════════════════════════════════\n";
    os << "  💻  CARTE DIGITALĂ\n";
    os << "══════════════════════════════════════════\n";
    os << "  Titlu:           " << titlu << "\n";
    os << "  Autori:          " << autoriToString() << "\n";
    os << "  ISSN:            " << issn << "\n";
    os << "  Categorie:       " << categorie << "\n";
    os << "  An apariție:     " << an_aparitie << "\n";
    os << "  Nr. pagini:      " << nr_pagini << "\n";
    os << "  Preț intrare:    " << std::fixed << std::setprecision(2) << pret_intrare << " RON\n";
    os << "  Serie contabilă: " << serie_contabila << "\n";
    os << "  Stare:           " << stareToString(stare_carte) << "\n";
    os << "  Disponibilă:     " << (disponibilitate ? "Da" : "Nu") << "\n";
    os << "  ────────────────────────────────────\n";
    os << "  Format digital:  " << format_digital << "\n";
    os << "  Dimensiune:      " << dimensiune_mb << " MB\n";
    os << "  Link acces:      " << link_acces << "\n";
    os << "══════════════════════════════════════════\n";
}

// ─────────────────────────────────────────────
//  Format pentru salvare în fișier (delimitator '|')
// ─────────────────────────────────────────────
std::string CarteDigitala::formatFisier() const {
    std::ostringstream oss;
    oss << "DIGITALA|"
        << titlu << "|"
        << autoriToString() << "|"
        << issn << "|"
        << pret_intrare << "|"
        << serie_contabila << "|"
        << poza_path << "|"
        << stareToString(stare_carte) << "|"
        << (disponibilitate ? "1" : "0") << "|"
        << categorie << "|"
        << an_aparitie << "|"
        << nr_pagini << "|"
        << format_digital << "|"
        << dimensiune_mb << "|"
        << link_acces;
    return oss.str();
}

// ─────────────────────────────────────────────
//  Returnează tipul clasei
// ─────────────────────────────────────────────
std::string CarteDigitala::getTip() const {
    return "DIGITALA";
}

// ─────────────────────────────────────────────
//  Returnează locația scurtă pentru tabel
// ─────────────────────────────────────────────
std::string CarteDigitala::getLocatieScurta() const {
    return "Online (Digital)";
}
