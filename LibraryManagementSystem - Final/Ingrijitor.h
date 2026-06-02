#ifndef INGRIJITOR_H
#define INGRIJITOR_H

#include "Utilizator.h"

class Ingrijitor : public Utilizator {
private:
    std::string zona_responsabilitate;
    double salariu;
    std::string data_angajare;
    std::string program_lucru;
    std::string echipament_atribuit;

public:
    Ingrijitor();
    Ingrijitor(const std::string& id, const std::string& parola,
               const std::string& nume, const std::string& prenume,
               const std::string& cnp, const std::string& email, const std::string& telefon,
               const std::string& adresa, bool activ,
               const std::string& zona, double salariu,
               const std::string& data_angajare, const std::string& program_lucru,
               const std::string& echipament);
    ~Ingrijitor() override;

    std::string getZonaResponsabilitate() const;
    double getSalariu() const;
    std::string getDataAngajare() const;
    std::string getProgramLucru() const;
    std::string getEchipamentAtribuit() const;

    void setZonaResponsabilitate(const std::string& z);
    void setSalariu(double s);
    void setDataAngajare(const std::string& d);
    void setProgramLucru(const std::string& p);
    void setEchipamentAtribuit(const std::string& e);

    void afisare(std::ostream& os) const override;
    std::string formatFisier() const override;
    std::string getTip() const override;
    std::string getRol() const override;
};

#endif
