#ifndef IMPRUMUT_H
#define IMPRUMUT_H

#include <string>
#include <iostream>
#include <sstream>

// ─────────────────────────────────────────────
//  Clasa Imprumut – leagă o carte de un cititor
// ─────────────────────────────────────────────
class Imprumut {
private:
    std::string id_carte_imprumutata;   // ISBN-ul cărții
    std::string id_cititor;             // ID-ul cititorului
    std::string nume_cititor;           // Nume complet (pentru afișare rapidă)
    std::string data_imprumut;          // format: DD/MM/YYYY
    std::string termen_limita;          // format: DD/MM/YYYY
    std::string observatii_probleme;

public:
    // Constructori
    Imprumut();
    Imprumut(const std::string& id_carte, const std::string& id_cititor,
             const std::string& nume_cititor, const std::string& data_imp,
             const std::string& termen, const std::string& obs);
    ~Imprumut();

    // Getteri
    std::string getIdCarte() const;
    std::string getIdCititor() const;
    std::string getNumeCititor() const;
    std::string getDataImprumut() const;
    std::string getTermenLimita() const;
    std::string getObservatii() const;

    // Setteri
    void setIdCarte(const std::string& id);
    void setIdCititor(const std::string& id);
    void setNumeCititor(const std::string& n);
    void setDataImprumut(const std::string& d);
    void setTermenLimita(const std::string& t);
    void setObservatii(const std::string& o);

    // Afișare
    void afisare(std::ostream& os) const;

    // Serializare pentru fișier
    std::string formatFisier() const;

    // Deserializare din linie fișier
    static Imprumut dinLinieFisier(const std::string& linie);

    // Supraîncărcare operator
    friend std::ostream& operator<<(std::ostream& os, const Imprumut& imp);
};

#endif // IMPRUMUT_H
