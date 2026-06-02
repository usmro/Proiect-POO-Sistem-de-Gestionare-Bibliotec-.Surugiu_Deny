#include "Director.h"
#include <iomanip>

Director::Director() : Utilizator(), departament(""), salariu(0), data_angajare(""), birou(""), nivel_acces(5) {}

Director::Director(const std::string& id, const std::string& parola,
                   const std::string& nume, const std::string& prenume,
                   const std::string& cnp, const std::string& email, const std::string& telefon,
                   const std::string& adresa, bool activ,
                   const std::string& departament, double salariu,
                   const std::string& data_angajare, const std::string& birou, int nivel_acces)
    : Utilizator(id, parola, nume, prenume, cnp, email, telefon, adresa, activ),
      departament(departament), salariu(salariu), data_angajare(data_angajare),
      birou(birou), nivel_acces(nivel_acces) {}

Director::~Director() {}

std::string Director::getDepartament() const { return departament; }
double Director::getSalariu() const { return salariu; }
std::string Director::getDataAngajare() const { return data_angajare; }
std::string Director::getBirou() const { return birou; }
int Director::getNivelAcces() const { return nivel_acces; }

void Director::setDepartament(const std::string& d) { departament = d; }
void Director::setSalariu(double s) { salariu = s; }
void Director::setDataAngajare(const std::string& d) { data_angajare = d; }
void Director::setBirou(const std::string& b) { birou = b; }
void Director::setNivelAcces(int n) { nivel_acces = n; }

void Director::afisare(std::ostream& os) const {
    os << "══════════════════════════════════════════\n";
    os << "  👔  DIRECTOR\n";
    os << "══════════════════════════════════════════\n";
    os << "  ID:              " << id << "\n";
    os << "  Nume:            " << prenume << " " << nume << "\n";
    os << "  CNP:             " << cnp << "\n";
    os << "  Email:           " << email << "\n";
    os << "  Telefon:         " << telefon << "\n";
    os << "  Adresă:          " << adresa << "\n";
    os << "  Activ:           " << (activ ? "Da" : "Nu") << "\n";
    os << "  ────────────────────────────────────\n";
    os << "  Departament:     " << departament << "\n";
    os << "  Salariu:         " << std::fixed << std::setprecision(2) << salariu << " RON\n";
    os << "  Data angajare:   " << data_angajare << "\n";
    os << "  Birou:           " << birou << "\n";
    os << "  Nivel acces:     " << nivel_acces << "/5\n";
    os << "══════════════════════════════════════════\n";
}

std::string Director::formatFisier() const {
    std::ostringstream oss;
    oss << "DIRECTOR|" << id << "|" << parola << "|" << nume << "|" << prenume << "|"
        << cnp << "|" << email << "|" << telefon << "|" << adresa << "|"
        << (activ ? "1" : "0") << "|" << departament << "|" << salariu << "|"
        << data_angajare << "|" << birou << "|" << nivel_acces;
    return oss.str();
}

std::string Director::getTip() const { return "DIRECTOR"; }
std::string Director::getRol() const { return "Director"; }
