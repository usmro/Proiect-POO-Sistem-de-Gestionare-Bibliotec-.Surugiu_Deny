#ifndef CARTE_FIZICA_H
#define CARTE_FIZICA_H

#include "Carte.h"

// ─────────────────────────────────────────────
//  Struct pentru locația fizică în bibliotecă
// ─────────────────────────────────────────────
struct Locatie {
    std::string cladire;
    std::string camera;
    std::string culoar;
    std::string raft;

    Locatie() : cladire(""), camera(""), culoar(""), raft("") {}
    Locatie(const std::string& cl, const std::string& cam,
            const std::string& cul, const std::string& r)
        : cladire(cl), camera(cam), culoar(cul), raft(r) {}
};

// ─────────────────────────────────────────────
//  Clasă derivată: CarteFizica
// ─────────────────────────────────────────────
class CarteFizica : public Carte {
private:
    std::string format_dimensiuni;   // ex: "21x15x3 cm"
    double greutate;                 // în grame
    std::string tip_coperta;         // hardcover / softcover / spiralata
    Locatie locatie;

public:
    // Constructori
    CarteFizica();
    CarteFizica(const std::string& titlu, const std::vector<std::string>& autori,
                const std::string& issn, double pret, const std::string& serie,
                const std::string& poza, StareCarte stare, bool disp,
                const std::string& categorie, int an_aparitie, int nr_pagini,
                const std::string& format_dim, double greutate,
                const std::string& tip_coperta, const Locatie& loc);
    ~CarteFizica() override;

    // Getteri specifici
    std::string getFormatDimensiuni() const;
    double getGreutate() const;
    std::string getTipCoperta() const;
    Locatie getLocatie() const;

    // Setteri specifici
    void setFormatDimensiuni(const std::string& f);
    void setGreutate(double g);
    void setTipCoperta(const std::string& t);
    void setLocatie(const Locatie& l);

    // Implementare metode virtuale
    void afisare(std::ostream& os) const override;
    std::string formatFisier() const override;
    std::string getTip() const override;
    std::string getLocatieScurta() const override;
};

#endif // CARTE_FIZICA_H
