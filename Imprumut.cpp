#include "Imprumut.h"
#include <vector>

// ─────────────────────────────────────────────
//  Constructor implicit
// ─────────────────────────────────────────────
Imprumut::Imprumut()
    : id_carte_imprumutata(""), id_cititor(""), nume_cititor(""),
      data_imprumut(""), termen_limita(""), observatii_probleme("") {}

// ─────────────────────────────────────────────
//  Constructor parametrizat
// ─────────────────────────────────────────────
Imprumut::Imprumut(const std::string& id_carte, const std::string& id_cititor,
                   const std::string& nume_cititor, const std::string& data_imp,
                   const std::string& termen, const std::string& obs)
    : id_carte_imprumutata(id_carte), id_cititor(id_cititor), nume_cititor(nume_cititor),
      data_imprumut(data_imp), termen_limita(termen), observatii_probleme(obs) {}

// ─────────────────────────────────────────────
//  Destructor
// ─────────────────────────────────────────────
Imprumut::~Imprumut() {}

// ─────────────────────────────────────────────
//  Getteri
// ─────────────────────────────────────────────
std::string Imprumut::getIdCarte() const { return id_carte_imprumutata; }
std::string Imprumut::getIdCititor() const { return id_cititor; }
std::string Imprumut::getNumeCititor() const { return nume_cititor; }
std::string Imprumut::getDataImprumut() const { return data_imprumut; }
std::string Imprumut::getTermenLimita() const { return termen_limita; }
std::string Imprumut::getObservatii() const { return observatii_probleme; }

// ─────────────────────────────────────────────
//  Setteri
// ─────────────────────────────────────────────
void Imprumut::setIdCarte(const std::string& id) { id_carte_imprumutata = id; }
void Imprumut::setIdCititor(const std::string& id) { id_cititor = id; }
void Imprumut::setNumeCititor(const std::string& n) { nume_cititor = n; }
void Imprumut::setDataImprumut(const std::string& d) { data_imprumut = d; }
void Imprumut::setTermenLimita(const std::string& t) { termen_limita = t; }
void Imprumut::setObservatii(const std::string& o) { observatii_probleme = o; }

// ─────────────────────────────────────────────
//  Afișare detaliată
// ─────────────────────────────────────────────
void Imprumut::afisare(std::ostream& os) const {
    os << "┌──────────────────────────────────────────┐\n";
    os << "│  📋  ÎMPRUMUT                            │\n";
    os << "├──────────────────────────────────────────┤\n";
    os << "  ISBN carte:      " << id_carte_imprumutata << "\n";
    os << "  ID cititor:      " << id_cititor << "\n";
    os << "  Cititor:         " << nume_cititor << "\n";
    os << "  Data împrumut:   " << data_imprumut << "\n";
    os << "  Termen limită:   " << termen_limita << "\n";
    os << "  Observații:      " << observatii_probleme << "\n";
    os << "└──────────────────────────────────────────┘\n";
}

// ─────────────────────────────────────────────
//  Serializare pentru fișier
// ─────────────────────────────────────────────
std::string Imprumut::formatFisier() const {
    std::ostringstream oss;
    oss << id_carte_imprumutata << "|"
        << id_cititor << "|"
        << nume_cititor << "|"
        << data_imprumut << "|"
        << termen_limita << "|"
        << observatii_probleme;
    return oss.str();
}

// ─────────────────────────────────────────────
//  Deserializare din linie fișier
// ─────────────────────────────────────────────
Imprumut Imprumut::dinLinieFisier(const std::string& linie) {
    std::vector<std::string> campuri;
    std::stringstream ss(linie);
    std::string token;
    while (std::getline(ss, token, '|')) {
        campuri.push_back(token);
    }

    if (campuri.size() >= 6) {
        // Formatul nou: isbn|id_cititor|nume_cititor|data|termen|obs
        return Imprumut(campuri[0], campuri[1], campuri[2], campuri[3], campuri[4], campuri[5]);
    }
    else if (campuri.size() >= 5) {
        // Compatibilitate cu formatul vechi: isbn|nume_cititor|data|termen|obs
        return Imprumut(campuri[0], "", campuri[1], campuri[2], campuri[3], campuri[4]);
    }

    return Imprumut();
}

// ─────────────────────────────────────────────
//  Supraîncărcare operator <<
// ─────────────────────────────────────────────
std::ostream& operator<<(std::ostream& os, const Imprumut& imp) {
    imp.afisare(os);
    return os;
}
