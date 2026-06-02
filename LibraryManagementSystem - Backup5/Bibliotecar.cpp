#include "Bibliotecar.h"
#include <iomanip>

Bibliotecar::Bibliotecar()
    : Utilizator(), sectie(""), salariu(0), data_angajare(""), program_lucru(""), nr_carti_gestionate(0) {}

Bibliotecar::Bibliotecar(const std::string& id, const std::string& parola,
                         const std::string& nume, const std::string& prenume,
                         const std::string& cnp, const std::string& email, const std::string& telefon,
                         const std::string& adresa, bool activ,
                         const std::string& sectie, double salariu,
                         const std::string& data_angajare, const std::string& program_lucru,
                         int nr_carti_gestionate)
    : Utilizator(id, parola, nume, prenume, cnp, email, telefon, adresa, activ),
      sectie(sectie), salariu(salariu), data_angajare(data_angajare),
      program_lucru(program_lucru), nr_carti_gestionate(nr_carti_gestionate) {}

Bibliotecar::~Bibliotecar() {}

std::string Bibliotecar::getSectie() const { return sectie; }
double Bibliotecar::getSalariu() const { return salariu; }
std::string Bibliotecar::getDataAngajare() const { return data_angajare; }
std::string Bibliotecar::getProgramLucru() const { return program_lucru; }
int Bibliotecar::getNrCartiGestionate() const { return nr_carti_gestionate; }

void Bibliotecar::setSectie(const std::string& s) { sectie = s; }
void Bibliotecar::setSalariu(double s) { salariu = s; }
void Bibliotecar::setDataAngajare(const std::string& d) { data_angajare = d; }
void Bibliotecar::setProgramLucru(const std::string& p) { program_lucru = p; }
void Bibliotecar::setNrCartiGestionate(int n) { nr_carti_gestionate = n; }

void Bibliotecar::afisare(std::ostream& os) const {
    os << "══════════════════════════════════════════\n";
    os << "  📚  BIBLIOTECAR\n";
    os << "══════════════════════════════════════════\n";
    os << "  ID:              " << id << "\n";
    os << "  Nume:            " << prenume << " " << nume << "\n";
    os << "  CNP:             " << cnp << "\n";
    os << "  Email:           " << email << "\n";
    os << "  Telefon:         " << telefon << "\n";
    os << "  Adresă:          " << adresa << "\n";
    os << "  Activ:           " << (activ ? "Da" : "Nu") << "\n";
    os << "  ────────────────────────────────────\n";
    os << "  Secție:          " << sectie << "\n";
    os << "  Salariu:         " << std::fixed << std::setprecision(2) << salariu << " RON\n";
    os << "  Data angajare:   " << data_angajare << "\n";
    os << "  Program lucru:   " << program_lucru << "\n";
    os << "  Cărți gestionate:" << nr_carti_gestionate << "\n";
    os << "══════════════════════════════════════════\n";
}

std::string Bibliotecar::formatFisier() const {
    std::ostringstream oss;
    oss << "BIBLIOTECAR|" << id << "|" << parola << "|" << nume << "|" << prenume << "|"
        << cnp << "|" << email << "|" << telefon << "|" << adresa << "|"
        << (activ ? "1" : "0") << "|" << sectie << "|" << salariu << "|"
        << data_angajare << "|" << program_lucru << "|" << nr_carti_gestionate;
    return oss.str();
}

std::string Bibliotecar::getTip() const { return "BIBLIOTECAR"; }
std::string Bibliotecar::getRol() const { return "Bibliotecar"; }
