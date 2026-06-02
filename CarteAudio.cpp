#include "CarteAudio.h"

CarteAudio::CarteAudio()
    : Carte(), durata_minute(0), narator(""), link_acces("") {}

CarteAudio::CarteAudio(const std::string &isbn, const std::string &titlu,
                       const std::vector<std::string> &autori,
                       const std::string &editura, double pret,
                       const std::string &categorie, int an_aparitie,
                       int nr_pagini, const std::string &data_adaugarii,
                       int contor_imprumuturi, int durata_minute,
                       const std::string &narator, const std::string &link)
    : Carte(isbn, titlu, autori, editura, pret, categorie, an_aparitie,
            nr_pagini, data_adaugarii, contor_imprumuturi),
      durata_minute(durata_minute), narator(narator), link_acces(link) {}

int CarteAudio::getDurataMinute() const { return durata_minute; }
std::string CarteAudio::getNarator() const { return narator; }
std::string CarteAudio::getLinkAcces() const { return link_acces; }

void CarteAudio::setDurataMinute(int d) { durata_minute = d; }
void CarteAudio::setNarator(const std::string &n) { narator = n; }
void CarteAudio::setLinkAcces(const std::string &link) { link_acces = link; }

std::string CarteAudio::getDurataFormatata() const {
  int ore = durata_minute / 60;
  int min = durata_minute % 60;
  std::string result;
  if (ore > 0)
    result += std::to_string(ore) + " ore";
  if (ore > 0 && min > 0)
    result += " ";
  if (min > 0)
    result += std::to_string(min) + " min";
  if (result.empty())
    result = "0 min";
  return result;
}

void CarteAudio::afisare(std::ostream &os) const {
  os << "Audiobook: " << titlu << "\n  Editura: " << editura << "\n  ("
     << an_aparitie << ") [Narator: " << narator
     << ", Durata: " << getDurataFormatata() << "]";
}

std::string CarteAudio::formatFisier() const {
  // 2|ISBN|Titlu|Autori|Pret|Categorie|An|Pagini|DataAdaugarii|Contor|DurataMinute|Narator|Link|Editura
  std::stringstream ss;
  ss << "2|" << isbn << "|" << titlu << "|" << autoriToString() << "|"
     << pret_intrare << "|" << categorie << "|" << an_aparitie << "|"
     << nr_pagini << "|" << data_adaugarii << "|" << contor_imprumuturi << "|"
     << durata_minute << "|" << narator << "|" << link_acces << "|" << editura;
  return ss.str();
}
