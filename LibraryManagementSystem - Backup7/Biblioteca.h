#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#include "Carte.h"
#include "CarteFizica.h"
#include "CarteDigitala.h"
#include "CarteAudio.h"
#include "Imprumut.h"
#include "Utilizator.h"
#include "Director.h"
#include "Bibliotecar.h"
#include "Ingrijitor.h"
#include "Cititor.h"

#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <fstream>
#include <tuple>
#include <map>
#include <ctime>
#include <sstream>
#include <sqlite3.h>


// Structură pentru o returnare în așteptare de confirmare de bibliotecar
struct ReturnareInAsteptare {
    std::string isbn;
    std::string id_cititor;
    std::string nume_cititor;
    std::string titlu_carte;
    std::string data_returnare;
    double amenda_intarziere;
};

struct PlataInAsteptare {
    std::string id_cititor;
    std::string nume_cititor;
    double suma;
    std::string data_solicitare;
};

// ═══════════════════════════════════════════════
//  STRUCTURI NOI
// ═══════════════════════════════════════════════

struct Rezervare {
    std::string isbn;
    std::string id_cititor;
    std::string nume_cititor;
    std::string data_rezervare;     // DD/MM/YYYY
    std::string data_expirare;      // DD/MM/YYYY (setat doar la alocare)
    bool alocata;                   // true = cartea e gata de ridicat
};


struct Raft {
    std::string id;
    std::string cladire;
    std::string camera;
    std::string culoar;
    std::string nume_raft;
    int capacitate_maxima;
    
    std::string formatFisier() const {
        return id + "|" + cladire + "|" + camera + "|" + culoar + "|" + nume_raft + "|" + std::to_string(capacitate_maxima);
    }
};

struct InregistrareIstoric {
    std::string isbn;
    std::string titlu_carte;
    std::string id_cititor;
    std::string data_imprumut;
    std::string data_returnare;
};

struct Recenzie {
    std::string isbn;
    std::string id_cititor;
    std::string nume_cititor;
    int nota;                       // 1-5
    std::string text;
    std::string data;
};

struct RezervareStudiu {
    std::string id_cititor;
    std::string nume_cititor;
    std::string sala;               // "Sala A", "Sala B", "Sala C"
    std::string data;               // DD/MM/YYYY
    std::string interval_orar;      // "08:00-10:00", etc.
};

struct Notificare {
    std::string id_destinatar;
    std::string mesaj;
    std::string data;
    bool citita;
};

class Biblioteca {
private:
    std::vector<std::shared_ptr<Carte>> carti;
    std::vector<std::shared_ptr<Carte>> carti_sterse;
    std::vector<Imprumut> imprumuturi;
    std::vector<std::shared_ptr<Utilizator>> utilizatori;
    std::vector<std::shared_ptr<Utilizator>> utilizatori_stersi;
    std::vector<ReturnareInAsteptare> returnari_in_asteptare;
    std::vector<PlataInAsteptare> plati_in_asteptare;
    std::map<std::string, int> stoc_defect;

    // Vectori noi
    std::vector<Rezervare> rezervari;
    std::vector<InregistrareIstoric> istoric_lectura;
    std::vector<Recenzie> recenzii;
    std::vector<RezervareStudiu> rezervari_sali;
    std::vector<Notificare> notificari;
    std::vector<Raft> rafturi;

    sqlite3* db;
    std::string fisier_db;
    double buget;
    time_t offset_timp;
    time_t ultima_plata_salarii;

    // Funcții interne de baze de date
    void incarcaDateDinSQL();
    void incarcaBuget();
    void salveazaBuget() const;
    void incarcaTimp();
    void salveazaTimp() const;

    std::vector<std::string> splitLinie(const std::string& linie, char delim) const;
    std::shared_ptr<Carte> parseazaLinieCarte(const std::string& linie) const;
    std::shared_ptr<Utilizator> parseazaLinieUtilizator(const std::string& linie) const;

    // Utilități dată
    static time_t stringToTime(const std::string& data_str);
    int calculeazaZileIntarziere(const std::string& data_limita_str) const;
    std::string getDataCurentaStr() const;
    std::string getDataCurentaPlusStr(int zile) const;

public:
    Biblioteca(const std::string& fisier_db = "biblioteca.db");
    ~Biblioteca();

    void incarcaDate();
    void salveazaDate() const;

    // Autentificare
    std::shared_ptr<Utilizator> autentificare(const std::string& id, const std::string& parola) const;

    // Cărți
    bool adaugaCarteFizica(const std::string& isbn, const std::string& titlu, const std::vector<std::string>& autori,
                           const std::string& editura, double pret, const std::string& categorie, int an_aparitie, int nr_pagini,
                           const std::string& data_adaugarii, int contor_imprumuturi,
                           int stoc_disponibil, const Locatie& loc, const std::string& coperta, bool stare_defecta);

    void adaugaCarteDigitala(const std::string& isbn, const std::string& titlu, const std::vector<std::string>& autori,
                             const std::string& editura, double pret, const std::string& categorie, int an_aparitie, int nr_pagini,
                             const std::string& data_adaugarii, int contor_imprumuturi,
                             const std::string& format, double dimensiune, const std::string& link);

    void adaugaCarteAudio(const std::string& isbn, const std::string& titlu, const std::vector<std::string>& autori,
                          const std::string& editura, double pret, const std::string& categorie, int an_aparitie, int nr_pagini,
                          const std::string& data_adaugarii, int contor_imprumuturi,
                          int durata_minute, const std::string& narator, const std::string& link);

    bool stergeCarte(const std::string& isbn, const std::string& id_actor = "");
    bool modificaCarte(const std::string& isbn, const std::string& noul_titlu, const std::vector<std::string>& noii_autori, const std::string& noua_editura, double noul_pret, const std::string& noua_categorie, int noul_an, int noile_pagini, const std::string& id_actor = "");
    std::shared_ptr<Carte> gasesteCarte(const std::string& isbn) const;
    bool reparaCarte(const std::string& isbn);
    int obtineNumarCartiFiziceRaft(const Locatie& loc) const;

    // Gestiune Financiară
    void simuleazaTrecereTimp(int zile);
    void platesteSalarii();
    double getBuget() const;
    double calculeazaSalariiTotale() const;
    time_t getVirtualTime() const;
    double calculeazaPenalizariTotale(const std::string& id_cititor) const;

    // Utilizatori
    void adaugaDirector(const std::string& id, const std::string& parola, const std::string& nume, const std::string& prenume,
                        const std::string& cnp, const std::string& email, const std::string& telefon,
                        const std::string& adresa,
                        const std::string& departament, double salariu,
                        const std::string& data_angajare, const std::string& birou, int nivel_acces);

    void adaugaBibliotecar(const std::string& id, const std::string& parola, const std::string& nume, const std::string& prenume,
                           const std::string& cnp, const std::string& email, const std::string& telefon,
                           const std::string& adresa,
                           const std::string& sectie, double salariu,
                           const std::string& data_angajare, const std::string& program_lucru,
                           int nr_carti_gestionate);

    void adaugaIngrijitor(const std::string& id, const std::string& parola, const std::string& nume, const std::string& prenume,
                          const std::string& cnp, const std::string& email, const std::string& telefon,
                          const std::string& adresa,
                          const std::string& zona, double salariu,
                          const std::string& data_angajare, const std::string& program_lucru,
                          const std::string& echipament);

    void adaugaCititor(const std::string& id, const std::string& parola, const std::string& nume, const std::string& prenume,
                       const std::string& cnp, const std::string& email, const std::string& telefon,
                       const std::string& adresa,
                       const std::string& tip_abonament, const std::string& data_inregistrare,
                       const std::string& data_expirare, int max_carti);

    bool stergeUtilizator(const std::string& id, const std::string& id_actor = "");
    bool modificaUtilizator(const std::string& id, const std::string& noua_parola, const std::string& noul_nume, const std::string& noul_prenume, const std::string& noul_email, const std::string& noul_telefon, const std::string& noua_adresa, const std::string& id_actor = "");
    std::shared_ptr<Utilizator> gasesteUtilizator(const std::string& id) const;
    std::shared_ptr<Cititor> gasesteCititor(const std::string& id) const;
    bool solicitaPlata(const std::string& id_cititor);
    bool confirmaPlata(size_t index, const std::string& id_actor = "");
    bool refuzaPlata(size_t index);
    void afiseazaPlatiInAsteptare(std::ostream& os) const;
    size_t getNumarPlatiInAsteptare() const;
    bool platestePenalizari(const std::string& id_cititor);
    const std::map<std::string, int>& getStocDefect() const { return stoc_defect; }
    std::string getIsbnDupaIndex(size_t index) const;
    std::shared_ptr<Carte> getCarteDupaIndex(size_t index) const;

    
    // Rafturi
    const std::vector<Raft>& getToateRafturile() const { return rafturi; }
    bool adaugaRaft(const std::string& cladire, const std::string& camera, const std::string& culoar, const std::string& nume_raft, int capacitate);
    bool stergeRaft(const std::string& id_raft);
    bool mutaCarte(const std::string& isbn, const Locatie& loc_noua);
    bool existaRaft(const Locatie& loc) const;
    int capacitateRaft(const Locatie& loc) const;

    // Getters pentru TUI
    const std::vector<std::shared_ptr<Carte>>& getToateCartile() const { return carti; }
    std::vector<std::string> getCategoriiDistincte() const;
    const std::vector<std::shared_ptr<Carte>>& getCartiSterse() const { return carti_sterse; }
    
    bool restaureazaCarte(const std::string& isbn, const std::string& id_actor = "");
    const std::vector<Imprumut>& getToateImprumuturile() const { return imprumuturi; }
    const std::vector<RezervareStudiu>& getToateRezervariSali() const { return rezervari_sali; }
    const std::vector<std::shared_ptr<Utilizator>>& getTotiUtilizatorii() const { return utilizatori; }
    const std::vector<std::shared_ptr<Utilizator>>& getUtilizatoriStersi() const { return utilizatori_stersi; }
    
    bool restaureazaUtilizator(const std::string& id, const std::string& id_actor = "");
    const std::vector<ReturnareInAsteptare>& getReturnariInAsteptare() const { return returnari_in_asteptare; }
    const std::vector<PlataInAsteptare>& getPlatiInAsteptare() const { return plati_in_asteptare; }


    // Căutare Cărți
    std::vector<std::shared_ptr<Carte>> cautaDupaAutor(const std::string& autor) const;
    std::vector<std::shared_ptr<Carte>> cautaDupaNume(const std::string& nume) const;
    std::vector<std::shared_ptr<Carte>> cautaDupaCategorie(const std::string& categorie) const;
    std::vector<std::shared_ptr<Carte>> cautaDupaEditura(const std::string& editura) const;
    std::vector<std::shared_ptr<Carte>> cautaDupaAn(int an) const;
    std::vector<std::shared_ptr<Carte>> cautaDupaPagini(int min_pag, int max_pag) const;

    // Căutare Utilizatori
    std::vector<std::shared_ptr<Utilizator>> cautaUtilizatorDupaNume(const std::string& nume) const;
    std::vector<std::shared_ptr<Utilizator>> cautaUtilizatorDupaRol(const std::string& rol) const;

    // Împrumuturi
    bool adaugaImprumut(const std::string& isbn, const std::string& id_cititor,
                        const std::string& data_imp, const std::string& termen,
                        const std::string& obs);
    bool returneazaCarte(const std::string& isbn, const std::string& id_cititor, const std::string& id_actor = "");
    const Imprumut* getImprumutDupaIndex(size_t index) const;

    // Returnare în 2 pași
    bool solicitaReturnare(const std::string& isbn, const std::string& id_cititor);
    bool confirmaReturnare(size_t index, const std::string& id_actor = "");
    bool refuzaReturnareDefecta(size_t index, const std::string& id_actor = "");
    void afiseazaReturnariInAsteptare(std::ostream& os) const;
    size_t getNumarReturnariInAsteptare() const;

    // ═══════════════════════════════════════════════
    //  FUNCȚIONALITĂȚI NOI
    // ═══════════════════════════════════════════════

    // Sistem de Rezervări (Holds)
    bool adaugaRezervare(const std::string& isbn, const std::string& id_cititor);
    bool anuleazaRezervare(const std::string& isbn, const std::string& id_cititor);
    bool confirmaRidicare(size_t index, const std::string& id_actor = "");
    void verificaRezervariExpirate();
    void alocaRezervare(const std::string& isbn);
    void afiseazaRezervariCititor(std::ostream& os, const std::string& id_cititor) const;
    void afiseazaRezervariAlocate(std::ostream& os) const;
    size_t getNumarRezervariAlocate() const;
    const std::vector<Rezervare>& getRezervari() const { return rezervari; }

    // Istoric Lectură
    void adaugaInIstoric(const std::string& isbn, const std::string& titlu,
                         const std::string& id_cititor, const std::string& data_imp,
                         const std::string& data_ret);
    void afiseazaIstoricCititor(std::ostream& os, const std::string& id_cititor) const;
    bool aCititCartea(const std::string& isbn, const std::string& id_cititor) const;

    // Recenzii și Rating
    bool adaugaRecenzie(const std::string& isbn, const std::string& id_cititor,
                        const std::string& nume_cititor, int nota, const std::string& text);
    bool areRecenzie(const std::string& isbn, const std::string& id_cititor) const;
    double calculeazaRatingMediu(const std::string& isbn) const;
    int getNumarRecenzii(const std::string& isbn) const;
    void afiseazaRecenziiCarte(std::ostream& os, const std::string& isbn) const;
    void afiseazaRecenziileCititorului(std::ostream& os, const std::string& id_cititor) const;

    // Gestiunea Sălilor de Lectură
    bool rezervaSala(const std::string& id_cititor, const std::string& nume_cititor,
                     const std::string& sala, const std::string& data,
                     const std::string& interval);
    bool esteSalaDisponibila(const std::string& sala, const std::string& data,
                             const std::string& interval) const;
    void afiseazaRezervariSaliCititor(std::ostream& os, const std::string& id_cititor) const;
    void afiseazaDispSali(std::ostream& os, const std::string& data) const;

    // Achiziții din buget
    bool achizitioneazaExemplare(const std::string& isbn, int cantitate, const std::string& id_actor = "");

    // Casări
    bool caseazaCarte(const std::string& isbn, const std::string& id_cititor_pierdere = "",
                      const std::string& id_actor = "");

    // Notificări
    void adaugaNotificare(const std::string& id_destinatar, const std::string& mesaj);
    void afiseazaNotificariNecitite(std::ostream& os, const std::string& id_destinatar);
    void genereazaNotificariExpirari();

    // Audit Log
    void scrieLaLog(const std::string& id_actor, const std::string& actiune) const;
    void afiseazaJurnalAudit(std::ostream& os, int ultimele_n = 30) const;

    // Afișare
    void afiseazaToateCartile(std::ostream& os) const;
    void afiseazaToateImprumuturile(std::ostream& os) const;
    void afiseazaImprumuturiCititor(std::ostream& os, const std::string& id_cititor) const;
    void afiseazaInventarScurt(std::ostream& os) const;
    void afiseazaTotiUtilizatorii(std::ostream& os) const;
    void afiseazaUtilizatoriScurt(std::ostream& os) const;

    // Statistici
    size_t getNumarCarti() const;
    size_t getNumarImprumuturi() const;
    size_t getNumarDisponibile() const;
    size_t getNumarUtilizatori() const;
    size_t getNumarCititori() const;
    size_t getNumarAngajati() const;
};

#endif
