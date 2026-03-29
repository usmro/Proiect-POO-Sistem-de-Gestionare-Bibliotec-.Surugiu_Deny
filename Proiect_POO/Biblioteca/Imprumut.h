#ifndef IMPRUMUT_H
#define IMPRUMUT_H

#include <string>
using namespace std;

class Imprumut {
private:
    int idCarte, idCititor;
    string data, dataReturnare;

public:
    Imprumut(int idCarte, int idCititor, string data, string dataReturnare);
    int getIdCarte();
    string getDataReturnare();
    string toCSV();
    string toString();
};

#endif
