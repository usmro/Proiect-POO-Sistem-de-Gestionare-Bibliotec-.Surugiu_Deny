#ifndef UTILIZATOR_H
#define UTILIZATOR_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

// ─────────────────────────────────────────────
//  Clasa de bază abstractă: Utilizator
// ─────────────────────────────────────────────
class Utilizator {
protected:
    std::string id;
    std::string parola;
    std::string nume;
    std::string prenume;
    std::string cnp;
    std::string email;
    std::string telefon;
    std::string adresa;
    bool activ;

public:
    Utilizator();
    Utilizator(const std::string& id, const std::string& parola,
               const std::string& nume, const std::string& prenume,
               const std::string& cnp, const std::string& email,
               const std::string& telefon, const std::string& adresa, bool activ);
    virtual ~Utilizator();

    // ── Getteri ──
    std::string getId() const;
    std::string getParola() const;
    std::string getNume() const;
    std::string getPrenume() const;
    std::string getNumeComplet() const;
    std::string getCnp() const;
    std::string getEmail() const;
    std::string getTelefon() const;
    std::string getAdresa() const;
    bool getActiv() const;

    // ── Setteri ──
    void setId(const std::string& id);
    void setParola(const std::string& p);
    void setNume(const std::string& n);
    void setPrenume(const std::string& p);
    void setCnp(const std::string& c);
    void setEmail(const std::string& e);
    void setTelefon(const std::string& t);
    void setAdresa(const std::string& a);
    void setActiv(bool a);

    // ── Autentificare ──
    bool verificaParola(const std::string& p) const;

    // ── Metode virtuale pure ──
    virtual void afisare(std::ostream& os) const = 0;
    virtual std::string formatFisier() const = 0;
    virtual std::string getTip() const = 0;
    virtual std::string getRol() const = 0;

    friend std::ostream& operator<<(std::ostream& os, const Utilizator& u);
};

#endif // UTILIZATOR_H
