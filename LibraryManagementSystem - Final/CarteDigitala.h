#ifndef CARTE_DIGITALA_H
#define CARTE_DIGITALA_H

#include "Carte.h"

class CarteDigitala : public Carte {
private:
    std::string format_fisier;
    double dimensiune_mb;
    std::string link_acces;

public:
    CarteDigitala();
    CarteDigitala(const std::string& isbn, const std::string& titlu, const std::vector<std::string>& autori,
                  const std::string& editura, double pret, const std::string& categorie, int an_aparitie, int nr_pagini,
                  const std::string& data_adaugarii, int contor_imprumuturi,
                  const std::string& format, double dimensiune, const std::string& link);

    std::string getFormatFisier() const;
    double getDimensiuneMB() const;
    std::string getLinkAcces() const;
    
    int getStocDisponibil() const override { return 999; }
    void setStocDisponibil(int stoc) override {}

    void setFormatFisier(const std::string& format);
    void setDimensiuneMB(double dim);
    void setLinkAcces(const std::string& link);

    void afisare(std::ostream& os) const override;
    std::string formatFisier() const override;
    std::string getTip() const override { return "DIGITALA"; }
    int getTipFormat() const override { return 1; }
    std::string getLocatieScurta() const override { return "Digital"; }
};

#endif // CARTE_DIGITALA_H
