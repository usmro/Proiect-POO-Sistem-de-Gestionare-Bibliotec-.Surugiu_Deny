#ifndef BIBLIOTECAR_H
#define BIBLIOTECAR_H

#include "Utilizator.h"

class Bibliotecar : public Utilizator {
private:
    std::string sectie;
    double salariu;
    std::string data_angajare;
    std::string program_lucru;
    int nr_carti_gestionate;

public:
    Bibliotecar();
    Bibliotecar(const std::string& id, const std::string& parola,
                const std::string& nume, const std::string& prenume,
                const std::string& cnp, const std::string& email, const std::string& telefon,
                const std::string& adresa, bool activ,
                const std::string& sectie, double salariu,
                const std::string& data_angajare, const std::string& program_lucru,
                int nr_carti_gestionate);
    ~Bibliotecar() override;

    std::string getSectie() const;
    double getSalariu() const;
    std::string getDataAngajare() const;
    std::string getProgramLucru() const;
    int getNrCartiGestionate() const;

    void setSectie(const std::string& s);
    void setSalariu(double s);
    void setDataAngajare(const std::string& d);
    void setProgramLucru(const std::string& p);
    void setNrCartiGestionate(int n);

    void afisare(std::ostream& os) const override;
    std::string formatFisier() const override;
    std::string getTip() const override;
    std::string getRol() const override;
};

#endif
