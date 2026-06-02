#include "Carte.h"
#include <algorithm>
#include <sstream>

Carte::Carte() : isbn(""), titlu(""), pret_intrare(0.0), categorie(""), 
               an_aparitie(0), nr_pagini(0), data_adaugarii(""), contor_imprumuturi(0), editura("Necunoscută") {}

Carte::Carte(const std::string& isbn, const std::string& titlu, const std::vector<std::string>& autori,
             const std::string& editura, double pret, const std::string& categorie, int an_aparitie, int nr_pagini,
             const std::string& data_adaugarii, int contor_imprumuturi)
    : isbn(isbn), titlu(titlu), autori(autori), editura(editura), pret_intrare(pret), categorie(categorie),
      an_aparitie(an_aparitie), nr_pagini(nr_pagini), data_adaugarii(data_adaugarii), contor_imprumuturi(contor_imprumuturi) {}

Carte::~Carte() {}

std::string Carte::getIsbn() const { return isbn; }
std::string Carte::getTitlu() const { return titlu; }
std::vector<std::string> Carte::getAutori() const { return autori; }
std::string Carte::getEditura() const { return editura; }
double Carte::getPretIntrare() const { return pret_intrare; }
std::string Carte::getCategorie() const { return categorie; }
int Carte::getAnAparitie() const { return an_aparitie; }
int Carte::getNrPagini() const { return nr_pagini; }
std::string Carte::getDataAdaugarii() const { return data_adaugarii; }
int Carte::getContorImprumuturi() const { return contor_imprumuturi; }

void Carte::setIsbn(const std::string& i) { isbn = i; }
void Carte::setTitlu(const std::string& t) { titlu = t; }
void Carte::setAutori(const std::vector<std::string>& a) { autori = a; }
void Carte::setEditura(const std::string& e) { editura = e; }
void Carte::setPretIntrare(double p) { pret_intrare = p; }
void Carte::setCategorie(const std::string& c) { categorie = c; }
void Carte::setAnAparitie(int an) { an_aparitie = an; }
void Carte::setNrPagini(int nr) { nr_pagini = nr; }
void Carte::setDataAdaugarii(const std::string& d) { data_adaugarii = d; }
void Carte::setContorImprumuturi(int c) { contor_imprumuturi = c; }

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
        size_t start = token.find_first_not_of(' ');
        size_t end = token.find_last_not_of(' ');
        if (start != std::string::npos) {
            result.push_back(token.substr(start, end - start + 1));
        }
    }
    return result;
}

void Carte::afisareCopertaASCII(std::ostream& os) const {
    std::string C = Color::Cyan;
    std::string Y = Color::Yellow;
    std::string G = Color::Green;
    std::string M = Color::Magenta;
    std::string B = Color::Bold;
    std::string R = Color::Reset;

    os << C << "  --------------------------------------------------" << R << "\n";
    os << C << " / \\                                                \\" << R << "\n";
    os << C << "|   |  ############################################  |" << R << "\n";
    os << C << "|   |  ##                                        ##  |" << R << "\n";
    
    std::string sp1 = "     ";
    std::string sp2 = "      ";
    os << C << "|   |  ##" << R << Y << sp1 << "  ____   ____   ____   _  __ " << sp2 << R << C << "##  |" << R << "\n";
    os << C << "|   |  ##" << R << Y << sp1 << " | __ ) / __ \\ / __ \\ | |/ / " << sp2 << R << C << "##  |" << R << "\n";
    os << C << "|   |  ##" << R << Y << sp1 << " |  _ \\| |  | | |  | || ' /  " << sp2 << R << C << "##  |" << R << "\n";
    os << C << "|   |  ##" << R << Y << sp1 << " | |_) || |__| | |__| || . \\  " << sp2 << R << C << "##  |" << R << "\n";
    os << C << "|   |  ##" << R << Y << sp1 << " |____/ \\____/ \\____/ |_|\\_\\ " << sp2 << R << C << "##  |" << R << "\n";
    
    os << C << "|   |  ##" << R << "                                        " << C << "##  |" << R << "\n";
    
    std::string t = getTitlu();
    if (t.length() > 40) t = t.substr(0, 37) + "...";
    int pad = 40 - t.length();
    int left_pad = pad / 2;
    int right_pad = pad - left_pad;
    os << C << "|   |  ##" << R << B << std::string(left_pad, ' ') << t << std::string(right_pad, ' ') << R << C << "##  |" << R << "\n";
    
    std::string a = "Autor: " + autoriToString();
    if (a.length() > 40) a = a.substr(0, 37) + "...";
    pad = 40 - a.length();
    left_pad = pad / 2;
    right_pad = pad - left_pad;
    os << C << "|   |  ##" << R << G << std::string(left_pad, ' ') << a << std::string(right_pad, ' ') << R << C << "##  |" << R << "\n";
    
    os << C << "|   |  ##                                        ##  |" << R << "\n";
    os << C << "|   |  ##" << R << M << "                 (o_o)                  " << R << C << "##  |" << R << "\n";
    os << C << "|   |  ##" << R << M << "                _|_|_                   " << R << C << "##  |" << R << "\n";
    os << C << "|   |  ##" << R << M << "               |     |                  " << R << C << "##  |" << R << "\n";
    os << C << "|   |  ##" << R << M << "               |_____|                  " << R << C << "##  |" << R << "\n";
    os << C << "|   |  ##" << R << M << "                |   |                   " << R << C << "##  |" << R << "\n";
    os << C << "|   |  ##                                        ##  |" << R << "\n";
    
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

std::ostream& operator<<(std::ostream& os, const Carte& c) {
    c.afisare(os);
    return os;
}
