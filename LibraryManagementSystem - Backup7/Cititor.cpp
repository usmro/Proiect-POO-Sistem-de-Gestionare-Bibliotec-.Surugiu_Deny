#include "Cititor.h"
#include <iomanip>

Cititor::Cititor()
    : Utilizator(), tip_abonament("standard"), data_inregistrare(""), data_expirare(""),
      max_carti_permise(3), nr_carti_imprumutate(0), penalizari(0.0) {}

Cititor::Cititor(const std::string& id, const std::string& parola,
                 const std::string& nume, const std::string& prenume,
                 const std::string& cnp, const std::string& email, const std::string& telefon,
                 const std::string& adresa, bool activ,
                 const std::string& tip_abonament, const std::string& data_inregistrare,
                 const std::string& data_expirare, int max_carti, int nr_imprumutate, double penalizari)
    : Utilizator(id, parola, nume, prenume, cnp, email, telefon, adresa, activ),
      tip_abonament(tip_abonament), data_inregistrare(data_inregistrare),
      data_expirare(data_expirare), max_carti_permise(max_carti),
      nr_carti_imprumutate(nr_imprumutate), penalizari(penalizari) {}

Cititor::~Cititor() {}

std::string Cititor::getTipAbonament() const { return tip_abonament; }
std::string Cititor::getDataInregistrare() const { return data_inregistrare; }
std::string Cititor::getDataExpirare() const { return data_expirare; }
int Cititor::getMaxCartiPermise() const { return max_carti_permise; }
int Cititor::getNrCartiImprumutate() const { return nr_carti_imprumutate; }
double Cititor::getPenalizari() const { return penalizari; }

void Cititor::setTipAbonament(const std::string& t) { tip_abonament = t; }
void Cititor::setDataInregistrare(const std::string& d) { data_inregistrare = d; }
void Cititor::setDataExpirare(const std::string& d) { data_expirare = d; }
void Cititor::setMaxCartiPermise(int m) { max_carti_permise = m; }
void Cititor::setNrCartiImprumutate(int n) { nr_carti_imprumutate = n; }
void Cititor::setPenalizari(double p) { penalizari = p; }

bool Cititor::poateImprumuta() const {
    return activ && (nr_carti_imprumutate < max_carti_permise) && (penalizari <= 0.0);
}

void Cititor::incrementeazaImprumuturi() {
    nr_carti_imprumutate++;
}

void Cititor::decrementeazaImprumuturi() {
    if (nr_carti_imprumutate > 0) nr_carti_imprumutate--;
}

void Cititor::afisare(std::ostream& os) const {
    os << "══════════════════════════════════════════\n";
    os << "  📖  CITITOR\n";
    os << "══════════════════════════════════════════\n";
    os << "  ID:              " << id << "\n";
    os << "  Nume:            " << prenume << " " << nume << "\n";
    os << "  CNP:             " << cnp << "\n";
    os << "  Email:           " << email << "\n";
    os << "  Telefon:         " << telefon << "\n";
    os << "  Adresă:          " << adresa << "\n";
    os << "  Activ:           " << (activ ? "Da" : "Nu") << "\n";
    os << "  ────────────────────────────────────\n";
    os << "  Abonament:       " << tip_abonament << "\n";
    os << "  Înregistrat:     " << data_inregistrare << "\n";
    os << "  Expiră:          " << data_expirare << "\n";
    os << "  Împrumutate:     " << nr_carti_imprumutate << " / " << max_carti_permise << "\n";
    os << "  Penalizări:      " << std::fixed << std::setprecision(2) << penalizari << " RON\n";
    os << "  Poate împrumuta: " << (poateImprumuta() ? "✅ Da" : "❌ Nu") << "\n";
    os << "══════════════════════════════════════════\n";
}

std::string Cititor::formatFisier() const {
    std::ostringstream oss;
    oss << "CITITOR|" << id << "|" << parola << "|" << nume << "|" << prenume << "|"
        << cnp << "|" << email << "|" << telefon << "|" << adresa << "|"
        << (activ ? "1" : "0") << "|" << tip_abonament << "|"
        << data_inregistrare << "|" << data_expirare << "|"
        << max_carti_permise << "|" << nr_carti_imprumutate << "|" << penalizari;
    return oss.str();
}

std::string Cititor::getTip() const { return "CITITOR"; }
std::string Cititor::getRol() const { return "Cititor"; }
