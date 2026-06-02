#ifndef DIRECTOR_H
#define DIRECTOR_H

#include "Utilizator.h"

class Director : public Utilizator {
private:
    std::string departament;
    double salariu;
    std::string data_angajare;
    std::string birou;
    int nivel_acces;

public:
    Director();
    Director(const std::string& id, const std::string& parola,
             const std::string& nume, const std::string& prenume,
             const std::string& cnp, const std::string& email, const std::string& telefon,
             const std::string& adresa, bool activ,
             const std::string& departament, double salariu,
             const std::string& data_angajare, const std::string& birou, int nivel_acces);
    ~Director() override;

    std::string getDepartament() const;
    double getSalariu() const;
    std::string getDataAngajare() const;
    std::string getBirou() const;
    int getNivelAcces() const;

    void setDepartament(const std::string& d);
    void setSalariu(double s);
    void setDataAngajare(const std::string& d);
    void setBirou(const std::string& b);
    void setNivelAcces(int n);

    void afisare(std::ostream& os) const override;
    std::string formatFisier() const override;
    std::string getTip() const override;
    std::string getRol() const override;
};

#endif
