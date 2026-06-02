#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#include "Carte.h"
#include "CarteFizica.h"
#include "CarteDigitala.h"
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

// ═══════════════════════════════════════════════
// CULORI ANSI PENTRU UI
// ═══════════════════════════════════════════════
namespace Color {
    const std::string Reset   = "\033[0m";
    const std::string Red     = "\033[31m";
    const std::string Green   = "\033[32m";
    const std::string Yellow  = "\033[33m";
    const std::string Cyan    = "\033[36m";
    const std::string Magenta = "\033[35m";
    const std::string Bold    = "\033[1m";
}

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

class Biblioteca {
private:
    std::vector<std::shared_ptr<Carte>> carti;
    std::vector<Imprumut> imprumuturi;
    std::vector<std::shared_ptr<Utilizator>> utilizatori;
    std::vector<ReturnareInAsteptare> returnari_in_asteptare;
    std::vector<PlataInAsteptare> plati_in_asteptare;

    std::string fisier_carti;
    std::string fisier_imprumuturi;
    std::string fisier_utilizatori;
    std::string fisier_returnari;
    std::string fisier_plati;

    std::vector<std::string> splitLinie(const std::string& linie, char delim) const;
    std::shared_ptr<Carte> parseazaLinieCarte(const std::string& linie) const;
    std::shared_ptr<Utilizator> parseazaLinieUtilizator(const std::string& linie) const;

    // Utilități dată
    static time_t stringToTime(const std::string& data_str);
    static int calculeazaZileIntarziere(const std::string& data_limita_str);

public:
    Biblioteca(const std::string& fisier_carti = "db_carti.txt",
               const std::string& fisier_imprumuturi = "db_imprumuturi.txt",
               const std::string& fisier_utilizatori = "db_utilizatori.txt",
               const std::string& fisier_returnari = "db_returnari.txt",
               const std::string& fisier_plati = "db_plati.txt");
    ~Biblioteca();

    void incarcaDate();
    void salveazaDate() const;

    // Autentificare
    std::shared_ptr<Utilizator> autentificare(const std::string& id, const std::string& parola) const;

    // Cărți
    void adaugaCarteFizica(const std::string& titlu, const std::vector<std::string>& autori,
                           const std::string& isbn, double pret, const std::string& serie,
                           const std::string& poza, StareCarte stare, int stoc_tot, int stoc_disp,
                           const std::string& categorie, int an, int pagini,
                           const std::string& dimensiuni, double greutate,
                           const std::string& coperta, const Locatie& loc);

    void adaugaCarteDigitala(const std::string& titlu, const std::vector<std::string>& autori,
                             const std::string& isbn, double pret, const std::string& serie,
                             const std::string& poza, StareCarte stare, int stoc_tot, int stoc_disp,
                             const std::string& categorie, int an, int pagini,
                             const std::string& format_digital, double dimensiune_mb,
                             const std::string& link_acces);

    bool stergeCarte(const std::string& isbn);
    std::shared_ptr<Carte> gasesteCarte(const std::string& isbn) const;
    bool reparaCarte(const std::string& isbn);
    int obtineNumarCartiFiziceRaft(const Locatie& loc) const;

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

    bool stergeUtilizator(const std::string& id);
    std::shared_ptr<Utilizator> gasesteUtilizator(const std::string& id) const;
    std::shared_ptr<Cititor> gasesteCititor(const std::string& id) const;
    bool solicitaPlata(const std::string& id_cititor);
    bool confirmaPlata(size_t index);
    bool refuzaPlata(size_t index);
    void afiseazaPlatiInAsteptare(std::ostream& os) const;
    size_t getNumarPlatiInAsteptare() const;
    bool platestePenalizari(const std::string& id_cititor);
    std::string getIsbnDupaIndex(size_t index) const;

    // Căutare Cărți
    std::vector<std::shared_ptr<Carte>> cautaDupaAutor(const std::string& autor) const;
    std::vector<std::shared_ptr<Carte>> cautaDupaNume(const std::string& nume) const;
    std::vector<std::shared_ptr<Carte>> cautaDupaCategorie(const std::string& categorie) const;
    std::vector<std::shared_ptr<Carte>> cautaDupaAn(int an) const;
    std::vector<std::shared_ptr<Carte>> cautaDupaPagini(int min_pag, int max_pag) const;

    // Căutare Utilizatori
    std::vector<std::shared_ptr<Utilizator>> cautaUtilizatorDupaNume(const std::string& nume) const;
    std::vector<std::shared_ptr<Utilizator>> cautaUtilizatorDupaRol(const std::string& rol) const;

    // Împrumuturi
    bool adaugaImprumut(const std::string& isbn, const std::string& id_cititor,
                        const std::string& data_imp, const std::string& termen,
                        const std::string& obs);
    bool returneazaCarte(const std::string& isbn, const std::string& id_cititor);
    const Imprumut* getImprumutDupaIndex(size_t index) const;

    // Returnare în 2 pași
    bool solicitaReturnare(const std::string& isbn, const std::string& id_cititor);
    bool confirmaReturnare(size_t index);
    bool refuzaReturnareDefecta(size_t index);
    void afiseazaReturnariInAsteptare(std::ostream& os) const;
    size_t getNumarReturnariInAsteptare() const;

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
