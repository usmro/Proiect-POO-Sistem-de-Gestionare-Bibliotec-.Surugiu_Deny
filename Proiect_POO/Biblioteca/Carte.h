#ifndef CARTE_H
#define CARTE_H

#include <string>
using namespace std;

class Carte {
private:
    int id;
    string titlu, autor, editura, isbn;
    int an, pagini;
    bool disponibila;

public:
    Carte(int id, string titlu, string autor, string editura,
          int an, int pagini, string isbn);

    int getId();
    string getTitlu();
    string getAutor();
    bool esteDisponibila();
    void imprumuta();
    void returneaza();
    string toCSV();
    string toString();
};
