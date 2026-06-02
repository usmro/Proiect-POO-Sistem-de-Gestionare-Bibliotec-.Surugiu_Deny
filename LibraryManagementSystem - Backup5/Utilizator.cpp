#include "Utilizator.h"
#include "Carte.h"
#include <algorithm>

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
void Utilizator::setParola(const std::string& p) { parola = p; } // fara encodare
void Utilizator::setParolaRaw(const std::string& p) { parola = p; }

void Utilizator::setNume(const std::string& n) { nume = n; }
void Utilizator::setPrenume(const std::string& p) { prenume = p; }
void Utilizator::setCnp(const std::string& c) { cnp = c; }
void Utilizator::setEmail(const std::string& e) { email = e; }
void Utilizator::setTelefon(const std::string& t) { telefon = t; }
void Utilizator::setAdresa(const std::string& a) { adresa = a; }
void Utilizator::setActiv(bool a) { activ = a; }

// Autentificare plain-text
bool Utilizator::verificaParola(const std::string& p) const {
    return parola == p;
}

// XOR Encoding (scop didactic)
std::string Utilizator::xorEncode(const std::string& text) {
    const std::string cheie = "BibliotecaSecreta2025";
    std::string result = text;
    for (size_t i = 0; i < text.size(); i++)
        result[i] = text[i] ^ cheie[i % cheie.size()];
    return toHex(result);
}

std::string Utilizator::toHex(const std::string& data) {
    std::ostringstream oss;
    for (unsigned char c : data)
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)c;
    return oss.str();
}

std::string Utilizator::fromHex(const std::string& hex) {
    std::string result;
    for (size_t i = 0; i + 1 < hex.size(); i += 2) {
        unsigned int byte;
        std::istringstream(hex.substr(i, 2)) >> std::hex >> byte;
        result += static_cast<char>(byte);
    }
    return result;
}

bool Utilizator::esteHexEncoded(const std::string& s) {
    if (s.empty() || s.size() % 2 != 0) return false;
    for (char c : s) {
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')))
            return false;
    }
    return true;
}

void Utilizator::afisareCardBiblioteca(std::ostream& os) const {
    std::string C = Color::Cyan;
    std::string Y = Color::Yellow;
    std::string G = Color::Green;
    std::string M = Color::Magenta;
    std::string B = Color::Bold;
    std::string R = Color::Reset;

    os << C << "  ________________________________________" << R << "\n";
    os << C << " /                                        \\" << R << "\n";
    os << C << "|  ╔════════════════════════════════════╗  |" << R << "\n";
    os << C << "|  ║" << R << Y << B << "  CARD DE BIBLIOTECA                " << R << C << "║  |" << R << "\n";
    os << C << "|  ╠════════════════════════════════════╣  |" << R << "\n";
    os << C << "|  ║                                    ║  |" << R << "\n";
    
    // Name
    std::string n = getNumeComplet();
    if (n.length() > 26) n = n.substr(0, 23) + "...";
    int pad = 28 - n.length();
    os << C << "|  ║  " << R << G << "Nume: " << n << std::string(pad, ' ') << R << C << "║  |" << R << "\n";
    
    // ID
    std::string id_str = getId();
    if (id_str.length() > 26) id_str = id_str.substr(0, 23) + "...";
    pad = 28 - id_str.length();
    os << C << "|  ║  " << R << G << "ID:   " << id_str << std::string(pad, ' ') << R << C << "║  |" << R << "\n";
    
    // Role
    std::string r = getRol();
    if (r.length() > 26) r = r.substr(0, 23) + "...";
    pad = 28 - r.length();
    os << C << "|  ║  " << R << G << "Rol:  " << r << std::string(pad, ' ') << R << C << "║  |" << R << "\n";
    
    os << C << "|  ║                                    ║  |" << R << "\n";
    
    // Status
    std::string status = getActiv() ? "[Valid]" : "[Inactiv]";
    std::string status_color = getActiv() ? Color::Green : Color::Red;
    pad = 22 - status.length();
    os << C << "|  ║" << R << M << "       (^_^)  " << R << status_color << status << std::string(pad, ' ') << R << C << "║  |" << R << "\n";
    
    os << C << "|  ║                                    ║  |" << R << "\n";
    os << C << "|  ╚════════════════════════════════════╝  |" << R << "\n";
    os << C << " \\________________________________________/" << R << "\n";
}

std::ostream& operator<<(std::ostream& os, const Utilizator& u) {
    u.afisare(os);
    return os;
}
