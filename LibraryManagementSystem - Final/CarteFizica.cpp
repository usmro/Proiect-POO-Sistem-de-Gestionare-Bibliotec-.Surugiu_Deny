#include "CarteFizica.h"

CarteFizica::CarteFizica() : Carte(), stoc_disponibil(0), tip_coperta("Hardcover"), stare_defecta(false) {}

CarteFizica::CarteFizica(const std::string& isbn, const std::string& titlu, const std::vector<std::string>& autori,
                         const std::string& editura, double pret, const std::string& categorie, int an_aparitie, int nr_pagini,
                         const std::string& data_adaugarii, int contor_imprumuturi,
                         int stoc_disponibil, const Locatie& loc, const std::string& coperta, bool stare_defecta)
    : Carte(isbn, titlu, autori, editura, pret, categorie, an_aparitie, nr_pagini, data_adaugarii, contor_imprumuturi),
      stoc_disponibil(stoc_disponibil), locatie_raft(loc), tip_coperta(coperta), stare_defecta(stare_defecta) {}

int CarteFizica::getStocDisponibil() const { return stoc_disponibil; }
Locatie CarteFizica::getLocatieRaft() const { return locatie_raft; }
std::string CarteFizica::getTipCoperta() const { return tip_coperta; }
bool CarteFizica::getStareDefecta() const { return stare_defecta; }

void CarteFizica::setStocDisponibil(int stoc) { stoc_disponibil = stoc; }
void CarteFizica::setLocatieRaft(const Locatie& loc) { locatie_raft = loc; }
void CarteFizica::setTipCoperta(const std::string& coperta) { tip_coperta = coperta; }
void CarteFizica::setStareDefecta(bool stare) { stare_defecta = stare; }

void CarteFizica::afisare(std::ostream& os) const {
    os << "Carte Fizica: " << titlu << "\n  Editura: " << editura << "\n (" << an_aparitie << ") - " << stoc_disponibil << " buc.";
}

std::string CarteFizica::formatFisier() const {
    // 0|ISBN|Titlu|Autori|Pret|Categorie|An|Pagini|DataAdaugarii|Contor|Stoc|Cladire|Camera|Culoar|Raft|Coperta|StareDefecta|Editura
    std::stringstream ss;
    ss << "0|" << isbn << "|" << titlu << "|" << autoriToString() << "|" << pret_intrare << "|"
       << categorie << "|" << an_aparitie << "|" << nr_pagini << "|" << data_adaugarii << "|"
       << contor_imprumuturi << "|" << stoc_disponibil << "|"
       << locatie_raft.cladire << "|" << locatie_raft.camera << "|" << locatie_raft.culoar << "|" << locatie_raft.raft << "|"
       << tip_coperta << "|" << (stare_defecta ? "1" : "0") << "|" << editura;
    return ss.str();
}

std::string CarteFizica::getLocatieScurta() const {
    return locatie_raft.cladire + ", " + locatie_raft.camera + " - " + locatie_raft.raft;
}
