#include "Ingrijitor.h"
#include <iomanip>

Ingrijitor::Ingrijitor()
    : Utilizator(), zona_responsabilitate(""), salariu(0), data_angajare(""),
      program_lucru(""), echipament_atribuit("") {}

Ingrijitor::Ingrijitor(const std::string& id, const std::string& parola,
                       const std::string& nume, const std::string& prenume,
                       const std::string& cnp, const std::string& email, const std::string& telefon,
                       const std::string& adresa, bool activ,
                       const std::string& zona, double salariu,
                       const std::string& data_angajare, const std::string& program_lucru,
                       const std::string& echipament)
    : Utilizator(id, parola, nume, prenume, cnp, email, telefon, adresa, activ),
      zona_responsabilitate(zona), salariu(salariu), data_angajare(data_angajare),
      program_lucru(program_lucru), echipament_atribuit(echipament) {}

Ingrijitor::~Ingrijitor() {}

std::string Ingrijitor::getZonaResponsabilitate() const { return zona_responsabilitate; }
double Ingrijitor::getSalariu() const { return salariu; }
std::string Ingrijitor::getDataAngajare() const { return data_angajare; }
std::string Ingrijitor::getProgramLucru() const { return program_lucru; }
std::string Ingrijitor::getEchipamentAtribuit() const { return echipament_atribuit; }

void Ingrijitor::setZonaResponsabilitate(const std::string& z) { zona_responsabilitate = z; }
void Ingrijitor::setSalariu(double s) { salariu = s; }
void Ingrijitor::setDataAngajare(const std::string& d) { data_angajare = d; }
void Ingrijitor::setProgramLucru(const std::string& p) { program_lucru = p; }
void Ingrijitor::setEchipamentAtribuit(const std::string& e) { echipament_atribuit = e; }

void Ingrijitor::afisare(std::ostream& os) const {
    os << "══════════════════════════════════════════\n";
    os << "  🧹  ÎNGRIJITOR\n";
    os << "══════════════════════════════════════════\n";
    os << "  ID:              " << id << "\n";
    os << "  Nume:            " << prenume << " " << nume << "\n";
    os << "  CNP:             " << cnp << "\n";
    os << "  Email:           " << email << "\n";
    os << "  Telefon:         " << telefon << "\n";
    os << "  Adresă:          " << adresa << "\n";
    os << "  Activ:           " << (activ ? "Da" : "Nu") << "\n";
    os << "  ────────────────────────────────────\n";
    os << "  Zonă:            " << zona_responsabilitate << "\n";
    os << "  Salariu:         " << std::fixed << std::setprecision(2) << salariu << " RON\n";
    os << "  Data angajare:   " << data_angajare << "\n";
    os << "  Program lucru:   " << program_lucru << "\n";
    os << "  Echipament:      " << echipament_atribuit << "\n";
    os << "══════════════════════════════════════════\n";
}

std::string Ingrijitor::formatFisier() const {
    std::ostringstream oss;
    oss << "INGRIJITOR|" << id << "|" << parola << "|" << nume << "|" << prenume << "|"
        << cnp << "|" << email << "|" << telefon << "|" << adresa << "|"
        << (activ ? "1" : "0") << "|" << zona_responsabilitate << "|" << salariu << "|"
        << data_angajare << "|" << program_lucru << "|" << echipament_atribuit;
    return oss.str();
}

std::string Ingrijitor::getTip() const { return "INGRIJITOR"; }
std::string Ingrijitor::getRol() const { return "Îngrijitor"; }
