#include "Carte.h"
#include <algorithm>

// ─────────────────────────────────────────────
//  Constructor implicit
// ─────────────────────────────────────────────
Carte::Carte()
    : titlu(""), isbn(""), pret_intrare(0.0), serie_contabila(""),
      poza_path(""), stare_carte(StareCarte::BUNA), stoc_total(0), stoc_disponibil(0),
      categorie(""), an_aparitie(0), nr_pagini(0) {}

// ─────────────────────────────────────────────
//  Constructor parametrizat
// ─────────────────────────────────────────────
Carte::Carte(const std::string& titlu, const std::vector<std::string>& autori,
             const std::string& isbn, double pret, const std::string& serie,
             const std::string& poza, StareCarte stare, int stoc_tot, int stoc_disp,
             const std::string& categorie, int an_aparitie, int nr_pagini)
    : titlu(titlu), autori(autori), isbn(isbn), pret_intrare(pret),
      serie_contabila(serie), poza_path(poza), stare_carte(stare),
      stoc_total(stoc_tot), stoc_disponibil(stoc_disp), categorie(categorie), an_aparitie(an_aparitie),
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
std::string Carte::getIsbn() const { return isbn; }
double Carte::getPretIntrare() const { return pret_intrare; }
std::string Carte::getSerieContabila() const { return serie_contabila; }
std::string Carte::getPozaPath() const { return poza_path; }
StareCarte Carte::getStareCarte() const { return stare_carte; }
int Carte::getStocTotal() const { return stoc_total; }
int Carte::getStocDisponibil() const { return stoc_disponibil; }
std::string Carte::getCategorie() const { return categorie; }
int Carte::getAnAparitie() const { return an_aparitie; }
int Carte::getNrPagini() const { return nr_pagini; }

// ─────────────────────────────────────────────
//  Setteri
// ─────────────────────────────────────────────
void Carte::setTitlu(const std::string& t) { titlu = t; }
void Carte::setAutori(const std::vector<std::string>& a) { autori = a; }
void Carte::setIsbn(const std::string& i) { isbn = i; }
void Carte::setPretIntrare(double p) { pret_intrare = p; }
void Carte::setSerieContabila(const std::string& s) { serie_contabila = s; }
void Carte::setPozaPath(const std::string& p) { poza_path = p; }
void Carte::setStareCarte(StareCarte s) { stare_carte = s; }
void Carte::setStocTotal(int st) { stoc_total = st; }
void Carte::setStocDisponibil(int sd) { stoc_disponibil = sd; }
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

void Carte::afisareCopertaASCII(std::ostream& os) const {
    std::string C = Color::Cyan;
    std::string Y = Color::Yellow;
    std::string G = Color::Green;
    std::string M = Color::Magenta;
    std::string B = Color::Bold;
    std::string R = Color::Reset;

    // Width of inner content is 40 chars.
    
    os << C << "  --------------------------------------------------" << R << "\n";
    os << C << " / \\                                                \\" << R << "\n";
    os << C << "|   |  ############################################  |" << R << "\n";
    os << C << "|   |  ##                                        ##  |" << R << "\n";
    
    // Big BOOK text (centered in 40 chars)
    // "  ____   ____   ____   _  __ " (29 chars)
    // Pad = 40 - 29 = 11. Left = 5, Right = 6.
    std::string sp1 = "     ";
    std::string sp2 = "      ";
    os << C << "|   |  ##" << R << Y << sp1 << "  ____   ____   ____   _  __ " << sp2 << R << C << "##  |" << R << "\n";
    os << C << "|   |  ##" << R << Y << sp1 << " | __ ) / __ \\ / __ \\ | |/ / " << sp2 << R << C << "##  |" << R << "\n";
    os << C << "|   |  ##" << R << Y << sp1 << " |  _ \\| |  | | |  | || ' /  " << sp2 << R << C << "##  |" << R << "\n";
    os << C << "|   |  ##" << R << Y << sp1 << " | |_) || |__| | |__| || . \\  " << sp2 << R << C << "##  |" << R << "\n";
    os << C << "|   |  ##" << R << Y << sp1 << " |____/ \\____/ \\____/ |_|\\_\\ " << sp2 << R << C << "##  |" << R << "\n";
    
    os << C << "|   |  ##" << R << "                                        " << C << "##  |" << R << "\n";
    
    // Title
    std::string t = getTitlu();
    if (t.length() > 40) t = t.substr(0, 37) + "...";
    int pad = 40 - t.length();
    int left_pad = pad / 2;
    int right_pad = pad - left_pad;
    os << C << "|   |  ##" << R << B << std::string(left_pad, ' ') << t << std::string(right_pad, ' ') << R << C << "##  |" << R << "\n";
    
    // Author
    std::string a = "Autor: " + autoriToString();
    if (a.length() > 40) a = a.substr(0, 37) + "...";
    pad = 40 - a.length();
    left_pad = pad / 2;
    right_pad = pad - left_pad;
    os << C << "|   |  ##" << R << G << std::string(left_pad, ' ') << a << std::string(right_pad, ' ') << R << C << "##  |" << R << "\n";
    
    os << C << "|   |  ##" << R << "                                        " << C << "##  |" << R << "\n";
    
    // Drawing (Robot or something)
    os << C << "|   |  ##" << R << M << "                 (o_o)                  " << R << C << "##  |" << R << "\n";
    os << C << "|   |  ##" << R << M << "                _|_|_                   " << R << C << "##  |" << R << "\n";
    os << C << "|   |  ##" << R << M << "               |     |                  " << R << C << "##  |" << R << "\n";
    os << C << "|   |  ##" << R << M << "               |_____|                  " << R << C << "##  |" << R << "\n";
    os << C << "|   |  ##" << R << M << "                |   |                   " << R << C << "##  |" << R << "\n";
    
    os << C << "|   |  ##" << R << "                                        " << C << "##  |" << R << "\n";
    
    // Category
    std::string cat = "Categorie: " + getCategorie();
    if (cat.length() > 40) cat = cat.substr(0, 37) + "...";
    pad = 40 - cat.length();
    left_pad = pad / 2;
    right_pad = pad - left_pad;
    os << C << "|   |  ##" << R << Y << std::string(left_pad, ' ') << cat << std::string(right_pad, ' ') << R << C << "##  |" << R << "\n";
    
    os << C << "|   |  ##                                        ##  |" << R << "\n";
    os << C << "|   |  ############################################  |" << R << "\n";
    os << C << " \\_/--------------------------------------------------/" << R << "\n";
}

// ─────────────────────────────────────────────
//  Supraîncărcare operator <<
// ─────────────────────────────────────────────
std::ostream& operator<<(std::ostream& os, const Carte& c) {
    c.afisare(os);
    return os;
}
