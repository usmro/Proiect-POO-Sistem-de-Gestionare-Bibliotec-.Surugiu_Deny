#include "Imprumut.h"

Imprumut::Imprumut(int idCarte, int idCititor, string data, string dataReturnare) {
    this->idCarte = idCarte;
    this->idCititor = idCititor;
    this->data = data;
    this->dataReturnare = dataReturnare;
}

int Imprumut::getIdCarte() { return idCarte; }
string Imprumut::getDataReturnare() { return dataReturnare; }

string Imprumut::toCSV() {
}

string Imprumut::toString() {
}
