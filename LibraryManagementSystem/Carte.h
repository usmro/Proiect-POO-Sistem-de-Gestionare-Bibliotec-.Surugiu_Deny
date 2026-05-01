#ifndef CARTE_H
#define CARTE_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

// ─────────────────────────────────────────────
//  Enum pentru starea fizică a cărții
// ─────────────────────────────────────────────
enum class StareCarte { BUNA, DEFECTA };

// ─────────────────────────────────────────────
//  Clasa de bază abstractă: Carte
// ─────────────────────────────────────────────
class Carte {
protected:
    std::string titlu;
    std::vector<std::string> autori;
    std::string issn;
    double pret_intrare;
    std::string serie_contabila;
    std::string poza_path;
    StareCarte stare_carte;
    bool disponibilitate;

    // Atribute suplimentare pentru motorul de căutare
    std::string categorie;
    int an_aparitie;
    int nr_pagini;

public:
    // Constructori
    Carte();
    Carte(const std::string& titlu, const std::vector<std::string>& autori,
          const std::string& issn, double pret, const std::string& serie,
          const std::string& poza, StareCarte stare, bool disp,
          const std::string& categorie, int an_aparitie, int nr_pagini);
    virtual ~Carte();

    // ── Getteri ──
    std::string getTitlu() const;
    std::vector<std::string> getAutori() const;
    std::string getIssn() const;
    double getPretIntrare() const;
    std::string getSerieContabila() const;
    std::string getPozaPath() const;
    StareCarte getStareCarte() const;
    bool getDisponibilitate() const;
    std::string getCategorie() const;
    int getAnAparitie() const;
    int getNrPagini() const;

    // ── Setteri ──
    void setTitlu(const std::string& t);
    void setAutori(const std::vector<std::string>& a);
    void setIssn(const std::string& i);
    void setPretIntrare(double p);
    void setSerieContabila(const std::string& s);
    void setPozaPath(const std::string& p);
    void setStareCarte(StareCarte s);
    void setDisponibilitate(bool d);
    void setCategorie(const std::string& c);
    void setAnAparitie(int an);
    void setNrPagini(int nr);

    // ── Metode virtuale pure ──
    virtual void afisare(std::ostream& os) const = 0;
    virtual std::string formatFisier() const = 0;
    virtual std::string getTip() const = 0;
    virtual std::string getLocatieScurta() const = 0;

    // ── Utilități ──
    std::string autoriToString() const;
    static std::vector<std::string> stringToAutori(const std::string& s);
    static std::string stareToString(StareCarte s);
    static StareCarte stringToStare(const std::string& s);

    // ── Supraîncărcare operator ──
    friend std::ostream& operator<<(std::ostream& os, const Carte& c);
};

#endif // CARTE_H
