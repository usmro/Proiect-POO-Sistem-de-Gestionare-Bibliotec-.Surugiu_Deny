#include "Carte.h"

Carte::Carte(int id, string titlu, string autor, string editura,
             int an, int pagini, string isbn)
{
    this->id = id;
    this->titlu = titlu;
    this->autor = autor;
    this->editura = editura;
    this->an = an;
    this->pagini = pagini;
    this->isbn = isbn;
    this->disponibila = true;
}

int Carte::getId() { return id; }
string Carte::getTitlu() { return titlu; }
string Carte::getAutor() { return autor; }
bool Carte::esteDisponibila() { return disponibila; }

void Carte::imprumuta() { disponibila = false; }
void Carte::returneaza() { disponibila = true; }

string Carte::toCSV() {
}

string Carte::toString() {
}