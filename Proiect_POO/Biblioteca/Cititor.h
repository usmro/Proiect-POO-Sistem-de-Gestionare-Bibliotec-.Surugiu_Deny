#ifndef CITITOR_H
#define CITITOR_H

#include <string>
using namespace std;

class Cititor {
private:
    int id;
    string nume, prenume;

public:
    Cititor(int id, string nume, string prenume);
    int getId();
    string getNume();
    string getPrenume();
    string toCSV();
    string toString();
};

#endif
