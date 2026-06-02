#include "Utilizator.h"

Utilizator::Utilizator()
    : id(""), parola(""), nume(""), prenume(""), cnp(""), email(""),
      telefon(""), adresa(""), activ(true) {}

Utilizator::Utilizator(const std::string& id, const std::string& parola,
                       const std::string& nume, const std::string& prenume,
                       const std::string& cnp, const std::string& email,
                       const std::string& telefon, const std::string& adresa, bool activ)
    : id(id), parola(parola), nume(nume), prenume(prenume), cnp(cnp),
      email(email), telefon(telefon), adresa(adresa), activ(activ) {}

Utilizator::~Utilizator() {}

// Getteri
std::string Utilizator::getId() const { return id; }
std::string Utilizator::getParola() const { return parola; }
std::string Utilizator::getNume() const { return nume; }
std::string Utilizator::getPrenume() const { return prenume; }
std::string Utilizator::getNumeComplet() const { return prenume + " " + nume; }
std::string Utilizator::getCnp() const { return cnp; }
std::string Utilizator::getEmail() const { return email; }
std::string Utilizator::getTelefon() const { return telefon; }
std::string Utilizator::getAdresa() const { return adresa; }
bool Utilizator::getActiv() const { return activ; }

// Setteri
void Utilizator::setId(const std::string& id) { this->id = id; }
void Utilizator::setParola(const std::string& p) { parola = p; }
void Utilizator::setNume(const std::string& n) { nume = n; }
void Utilizator::setPrenume(const std::string& p) { prenume = p; }
void Utilizator::setCnp(const std::string& c) { cnp = c; }
void Utilizator::setEmail(const std::string& e) { email = e; }
void Utilizator::setTelefon(const std::string& t) { telefon = t; }
void Utilizator::setAdresa(const std::string& a) { adresa = a; }
void Utilizator::setActiv(bool a) { activ = a; }

// Autentificare
bool Utilizator::verificaParola(const std::string& p) const {
    return parola == p;
}

std::ostream& operator<<(std::ostream& os, const Utilizator& u) {
    u.afisare(os);
    return os;
}
