#include "CarteFizica.h"
#include <iomanip>

// ─────────────────────────────────────────────
//  Constructor implicit
// ─────────────────────────────────────────────
CarteFizica::CarteFizica()
    : Carte(), format_dimensiuni(""), greutate(0.0), tip_coperta(""), locatie() {}

// ─────────────────────────────────────────────
//  Constructor parametrizat
// ─────────────────────────────────────────────
CarteFizica::CarteFizica(const std::string& titlu, const std::vector<std::string>& autori,
                         const std::string& issn, double pret, const std::string& serie,
                         const std::string& poza, StareCarte stare, bool disp,
                         const std::string& categorie, int an_aparitie, int nr_pagini,
                         const std::string& format_dim, double greutate,
                         const std::string& tip_coperta, const Locatie& loc)
    : Carte(titlu, autori, issn, pret, serie, poza, stare, disp, categorie, an_aparitie, nr_pagini),
      format_dimensiuni(format_dim), greutate(greutate),
      tip_coperta(tip_coperta), locatie(loc) {}

// ─────────────────────────────────────────────
//  Destructor
// ─────────────────────────────────────────────
CarteFizica::~CarteFizica() {}

// ─────────────────────────────────────────────
//  Getteri
// ─────────────────────────────────────────────
std::string CarteFizica::getFormatDimensiuni() const { return format_dimensiuni; }
double CarteFizica::getGreutate() const { return greutate; }
std::string CarteFizica::getTipCoperta() const { return tip_coperta; }
Locatie CarteFizica::getLocatie() const { return locatie; }

// ─────────────────────────────────────────────
//  Setteri
// ─────────────────────────────────────────────
void CarteFizica::setFormatDimensiuni(const std::string& f) { format_dimensiuni = f; }
void CarteFizica::setGreutate(double g) { greutate = g; }
void CarteFizica::setTipCoperta(const std::string& t) { tip_coperta = t; }
void CarteFizica::setLocatie(const Locatie& l) { locatie = l; }

// ─────────────────────────────────────────────
//  Afișare detaliată în consolă
// ─────────────────────────────────────────────
void CarteFizica::afisare(std::ostream& os) const {
    os << "══════════════════════════════════════════\n";
    os << "  📖  CARTE FIZICĂ\n";
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
    os << "  Dimensiuni:      " << format_dimensiuni << "\n";
    os << "  Greutate:        " << greutate << " g\n";
    os << "  Tip copertă:     " << tip_coperta << "\n";
    os << "  Locație:         Clădire: " << locatie.cladire
       << " | Cameră: " << locatie.camera
       << " | Culoar: " << locatie.culoar
       << " | Raft: " << locatie.raft << "\n";
    os << "══════════════════════════════════════════\n";
}

// ─────────────────────────────────────────────
//  Format pentru salvare în fișier (delimitator '|')
// ─────────────────────────────────────────────
std::string CarteFizica::formatFisier() const {
    std::ostringstream oss;
    oss << "FIZICA|"
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
        << format_dimensiuni << "|"
        << greutate << "|"
        << tip_coperta << "|"
        << locatie.cladire << "|"
        << locatie.camera << "|"
        << locatie.culoar << "|"
        << locatie.raft;
    return oss.str();
}

// ─────────────────────────────────────────────
//  Returnează tipul clasei
// ─────────────────────────────────────────────
std::string CarteFizica::getTip() const {
    return "FIZICA";
}

// ─────────────────────────────────────────────
//  Returnează locația scurtă pentru tabel
// ─────────────────────────────────────────────
std::string CarteFizica::getLocatieScurta() const {
    if (locatie.camera != "n/a" && !locatie.camera.empty()) {
        return "Cam: " + locatie.camera + " / Rft: " + locatie.raft;
    }
    return "Fizică";
}
