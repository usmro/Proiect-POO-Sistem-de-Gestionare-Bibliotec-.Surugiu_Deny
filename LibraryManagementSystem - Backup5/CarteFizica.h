#ifndef CARTE_FIZICA_H
#define CARTE_FIZICA_H

#include "Carte.h"

struct Locatie {
    std::string cladire;
    std::string camera;
    std::string culoar;
    std::string raft;
};

class CarteFizica : public Carte {
private:
    int stoc_disponibil;
    Locatie locatie_raft;
    std::string tip_coperta;
    bool stare_defecta;

public:
    CarteFizica();
    CarteFizica(const std::string& isbn, const std::string& titlu, const std::vector<std::string>& autori,
                const std::string& editura, double pret, const std::string& categorie, int an_aparitie, int nr_pagini,
                const std::string& data_adaugarii, int contor_imprumuturi,
                int stoc_disponibil, const Locatie& locatie, const std::string& tip_coperta, bool stare_defecta);

    int getStocDisponibil() const override;
    Locatie getLocatieRaft() const;
    std::string getTipCoperta() const;
    bool getStareDefecta() const;

    void setStocDisponibil(int stoc);
    void setLocatieRaft(const Locatie& loc);
    void setTipCoperta(const std::string& coperta);
    void setStareDefecta(bool stare);

    void afisare(std::ostream& os) const override;
    std::string formatFisier() const override;
    std::string getTip() const override { return "FIZICA"; }
    int getTipFormat() const override { return 0; }
    std::string getLocatieScurta() const override;
};

#endif // CARTE_FIZICA_H
