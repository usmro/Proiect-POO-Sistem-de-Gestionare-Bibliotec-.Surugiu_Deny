#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#include <vector>
#include "Carte.h"
#include "Cititor.h"
#include "Imprumut.h"

class Biblioteca {
private:
    vector<Carte> carti;
    vector<Cititor> cititori;
    vector<Imprumut> imprumuturi;

    int nextIdCarte = 1;
    int nextIdCititor = 1;

public:
    void adaugaCarte(string, string, string, int, int, string);
    void afiseazaCarti();
    void stergeCarte(int);
    void adaugaCititor(string, string);
    void afiseazaCititori();
    void stergeCititor(int);
    void imprumutaCarte(int, int);
    void returneazaCarte(int);
    void afiseazaImprumuturi();
    void cautaDupaAutor(string);
    void cautaDupaTitlu(string);
    void sorteazaCartiTitlu();
    void sorteazaCartiAutor();
    void statistici();
    void salveazaDate();
    void incarcaDate();
};

#endif
