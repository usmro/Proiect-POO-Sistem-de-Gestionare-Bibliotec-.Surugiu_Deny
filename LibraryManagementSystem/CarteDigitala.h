#ifndef CARTE_DIGITALA_H
#define CARTE_DIGITALA_H

#include "Carte.h"

// ─────────────────────────────────────────────
//  Clasă derivată: CarteDigitala
// ─────────────────────────────────────────────
class CarteDigitala : public Carte {
private:
    std::string format_digital;   // pdf / audiobook / film
    double dimensiune_mb;          // dimensiunea fișierului în MB
    std::string link_acces;        // URL sau cale de acces

public:
    // Constructori
    CarteDigitala();
    CarteDigitala(const std::string& titlu, const std::vector<std::string>& autori,
                  const std::string& issn, double pret, const std::string& serie,
                  const std::string& poza, StareCarte stare, bool disp,
                  const std::string& categorie, int an_aparitie, int nr_pagini,
                  const std::string& format_digital, double dimensiune_mb,
                  const std::string& link_acces);
    ~CarteDigitala() override;

    // Getteri specifici
    std::string getFormatDigital() const;
    double getDimensiuneMb() const;
    std::string getLinkAcces() const;

    // Setteri specifici
    void setFormatDigital(const std::string& f);
    void setDimensiuneMb(double d);
    void setLinkAcces(const std::string& l);

    // Implementare metode virtuale
    void afisare(std::ostream& os) const override;
    std::string formatFisier() const override;
    std::string getTip() const override;
    std::string getLocatieScurta() const override;
};

#endif // CARTE_DIGITALA_H
