#include "CarteDigitala.h"

CarteDigitala::CarteDigitala() : Carte(), format_fisier("PDF"), dimensiune_mb(0.0), link_acces("") {}

CarteDigitala::CarteDigitala(const std::string& isbn, const std::string& titlu, const std::vector<std::string>& autori,
                             const std::string& editura, double pret, const std::string& categorie, int an_aparitie, int nr_pagini,
                             const std::string& data_adaugarii, int contor_imprumuturi,
                             const std::string& format, double dimensiune, const std::string& link)
    : Carte(isbn, titlu, autori, editura, pret, categorie, an_aparitie, nr_pagini, data_adaugarii, contor_imprumuturi),
      format_fisier(format), dimensiune_mb(dimensiune), link_acces(link) {}

std::string CarteDigitala::getFormatFisier() const { return format_fisier; }
double CarteDigitala::getDimensiuneMB() const { return dimensiune_mb; }
std::string CarteDigitala::getLinkAcces() const { return link_acces; }

void CarteDigitala::setFormatFisier(const std::string& format) { format_fisier = format; }
void CarteDigitala::setDimensiuneMB(double dim) { dimensiune_mb = dim; }
void CarteDigitala::setLinkAcces(const std::string& link) { link_acces = link; }

void CarteDigitala::afisare(std::ostream& os) const {
    os << "Carte Digitala: " << titlu << "\n  Editura: " << editura << "\n (" << an_aparitie << ") [" << format_fisier << ", " << dimensiune_mb << " MB]";
}

std::string CarteDigitala::formatFisier() const {
    // 1|ISBN|Titlu|Autori|Pret|Categorie|An|Pagini|DataAdaugarii|Contor|Format|Dimensiune|Link|Editura
    std::stringstream ss;
    ss << "1|" << isbn << "|" << titlu << "|" << autoriToString() << "|" << pret_intrare << "|"
       << categorie << "|" << an_aparitie << "|" << nr_pagini << "|" << data_adaugarii << "|"
       << contor_imprumuturi << "|"
       << format_fisier << "|" << dimensiune_mb << "|" << link_acces << "|" << editura;
    return ss.str();
}
