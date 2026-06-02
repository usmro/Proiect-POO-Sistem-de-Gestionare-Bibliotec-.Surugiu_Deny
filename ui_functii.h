#pragma once
#include "Biblioteca.h"
#include "ConsoleUI.h"
#include <memory>
#include <string>

void pausare();

// Cărți
void adaugaCarteFizicaUI(Biblioteca& bib);
void adaugaCarteDigitalaUI(Biblioteca& bib);
void adaugaCarteAudioUI(Biblioteca& bib);
void stergeCarteUI(Biblioteca& bib, const std::string& id_actor);
void restaureazaCarteUI(Biblioteca& bib, const std::string& id_actor);
void modificaCarteUI(Biblioteca& bib, const std::string& id_actor);
void cautaCarteUI(Biblioteca& bib);
void vizualizareCatalogUI(Biblioteca& bib);

// Utilizatori
void adaugaUtilizatorUI(Biblioteca& bib);
void stergeUtilizatorUI(Biblioteca& bib, const std::string& id_actor);
void restaureazaUtilizatorUI(Biblioteca& bib, const std::string& id_actor);
void modificaUtilizatorUI(Biblioteca& bib, const std::string& id_actor);
void modificaProfilUI(Biblioteca& bib, const std::string& id_actor);

// Împrumuturi și Retururi
void imprumutaCarteUI(Biblioteca& bib);
void returneazaCarteUI(Biblioteca& bib, const std::string& id_actor);
void toateImprumuturileUI(Biblioteca& bib);
void imprumutaCarteCititorUI(Biblioteca& bib, const std::string& idCititor);
void returneazaCarteCititorUI(Biblioteca& bib, const std::string& idCititor);
void confirmaReturnariUI(Biblioteca& bib, const std::string& id_actor);

// Funcționalități Noi
void solicitaPlataUI(Biblioteca& bib, const std::shared_ptr<Utilizator>& u);
void confirmaPlatiUI(Biblioteca& bib, const std::string& id_actor);
void reconditioneazaCartiUI(Biblioteca& bib);
void rezervaCarteUI(Biblioteca& bib, const std::string& idCititor);
void confirmaRidicarRezervareUI(Biblioteca& bib, const std::string& id_actor);
void scrieRecenzieUI(Biblioteca& bib, const std::string& idCititor, const std::string& numeCititor);
void rezervaSalaUI(Biblioteca& bib, const std::string& idCititor, const std::string& numeCititor);
void achizitiiUI(Biblioteca& bib, const std::string& id_actor);
void casareCarteUI(Biblioteca& bib, const std::string& id_actor);

void gestiuneRafturiUI(Biblioteca& bib);
void mutaCarteFizicaUI(Biblioteca& bib);

// Cititor - vizualizari TUI
void veziImprumuturileMeleUI(Biblioteca& bib, const std::string& idCititor);
void veziRezervariSaliMeleUI(Biblioteca& bib, const std::string& idCititor);
