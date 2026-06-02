#ifndef CARTE_AUDIO_H
#define CARTE_AUDIO_H

#include "Carte.h"

class CarteAudio : public Carte {
private:
    int durata_minute;        // durata totală în minute
    std::string narator;      // cine narează audiobook-ul
    std::string link_acces;   // link-ul către fișierul audio

public:
    CarteAudio();
    CarteAudio(const std::string& isbn, const std::string& titlu, const std::vector<std::string>& autori,
               const std::string& editura, double pret, const std::string& categorie, int an_aparitie, int nr_pagini,
               const std::string& data_adaugarii, int contor_imprumuturi,
               int durata_minute, const std::string& narator, const std::string& link);

    int getDurataMinute() const;
    std::string getNarator() const;
    std::string getLinkAcces() const;

    int getStocDisponibil() const override { return 999; }
    void setStocDisponibil(int) override {}

    void setDurataMinute(int d);
    void setNarator(const std::string& n);
    void setLinkAcces(const std::string& link);

    // Helpers pentru afișare
    int getOre() const { return durata_minute / 60; }
    int getMinuteRamase() const { return durata_minute % 60; }
    std::string getDurataFormatata() const;

    void afisare(std::ostream& os) const override;
    std::string formatFisier() const override;
    std::string getTip() const override { return "AUDIOBOOK"; }
    int getTipFormat() const override { return 2; }
    std::string getLocatieScurta() const override { return "Audio"; }
};

#endif // CARTE_AUDIO_H
