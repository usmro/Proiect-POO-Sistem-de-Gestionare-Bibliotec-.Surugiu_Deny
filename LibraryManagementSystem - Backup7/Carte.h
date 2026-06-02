#ifndef CARTE_H
#define CARTE_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

namespace Color {
    const std::string Reset   = "\033[0m";
    const std::string Red     = "\033[31m";
    const std::string Green   = "\033[32m";
    const std::string Yellow  = "\033[33m";
    const std::string Cyan    = "\033[36m";
    const std::string Magenta = "\033[35m";
    const std::string Bold    = "\033[1m";
}

class Carte {
protected:
    std::string isbn;
    std::string titlu;
    std::vector<std::string> autori;
    std::string editura;
    double pret_intrare;
    std::string categorie;
    int an_aparitie;
    int nr_pagini;
    std::string data_adaugarii;
    int contor_imprumuturi;

public:
    Carte();
    Carte(const std::string& isbn, const std::string& titlu, const std::vector<std::string>& autori,
          const std::string& editura, double pret, const std::string& categorie, int an_aparitie, int nr_pagini,
          const std::string& data_adaugarii, int contor_imprumuturi);
    virtual ~Carte();

    std::string getIsbn() const;
    std::string getTitlu() const;
    std::vector<std::string> getAutori() const;
    std::string getEditura() const;
    double getPretIntrare() const;
    std::string getCategorie() const;
    int getAnAparitie() const;
    int getNrPagini() const;
    std::string getDataAdaugarii() const;
    int getContorImprumuturi() const;

    void setIsbn(const std::string& i);
    void setTitlu(const std::string& t);
    void setAutori(const std::vector<std::string>& a);
    void setEditura(const std::string& e);
    void setPretIntrare(double p);
    void setCategorie(const std::string& c);
    void setAnAparitie(int an);
    void setNrPagini(int nr);
    void setDataAdaugarii(const std::string& d);
    void setContorImprumuturi(int c);

    virtual void afisare(std::ostream& os) const = 0;
    virtual std::string formatFisier() const = 0;
    virtual std::string getTip() const = 0;
    virtual int getTipFormat() const = 0; // 0 = Fizic, 1 = Digital
    virtual std::string getLocatieScurta() const = 0;
    virtual int getStocDisponibil() const = 0;
    virtual void setStocDisponibil(int stoc) = 0;
    virtual bool getStareDefecta() const { return false; }
    virtual void setStareDefecta(bool stare) {}

    void afisareCopertaASCII(std::ostream& os) const;

    std::string autoriToString() const;
    static std::vector<std::string> stringToAutori(const std::string& s);

    friend std::ostream& operator<<(std::ostream& os, const Carte& c);
};

#endif // CARTE_H
