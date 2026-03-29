#include "Cititor.h"

Cititor::Cititor(int id, string nume, string prenume) {
    this->id = id;
    this->nume = nume;
    this->prenume = prenume;
}

int Cititor::getId() { return id; }
string Cititor::getNume() { return nume; }
string Cititor::getPrenume() { return prenume; }

string Cititor::toCSV() {
}

string Cititor::toString() {
}
