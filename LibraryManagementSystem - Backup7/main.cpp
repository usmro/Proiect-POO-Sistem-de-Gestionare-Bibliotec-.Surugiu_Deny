#include <iostream>
#include <memory>
#include <limits>
#include "Biblioteca.h"
#include "ui_functii.h"
#include "ConsoleUI.h"
#include "Cititor.h"

// ═══════════════════════════════════════════════
//  MENIURI PE ROL (TUI)
// ═══════════════════════════════════════════════

static void afiseazaHeaderTUI(const std::shared_ptr<Utilizator>& u) {
    ConsoleUI::clearScreen();
    ConsoleUI::drawBox(2, 2, 70, 5, "SISTEM DE GESTIUNE A BIBLIOTECII v4.0", ConsoleUI::Cyan);
    ConsoleUI::printAt(4, 4, "Conectat ca: " + u->getNumeComplet() + " (" + u->getRol() + ")", ConsoleUI::Cyan, true);
    
    // Desenare Card pe dreapta
    ConsoleUI::drawBox(40, 8, 32, 8, "Card Bibliotecă", ConsoleUI::Yellow);
    ConsoleUI::printAt(42, 10, "ID: " + u->getId());
    ConsoleUI::printAt(42, 11, "Rol: " + u->getRol());
    ConsoleUI::printAt(42, 12, "Contact: " + u->getEmail());
}

static bool MeniuDirector(Biblioteca& bib, const std::shared_ptr<Utilizator>& u) {
    std::vector<std::string> mainOpt = {
        "📚 Gestiune Cărți",
        "👥 Gestiune Utilizatori",
        "📋 Împrumuturi & Retururi",
        "💰 Finanțe & Audit",
        "👤 Detalii Cont",
        "🚪 Delogare"
    };
    int mainIndex = 0;
    
    while(true) {
        afiseazaHeaderTUI(u);
        int sel = ConsoleUI::showInteractiveMenu(2, 8, "Meniu Director", mainOpt, mainIndex);
        
        if (sel == 5 || sel == -1) return false;
        
        if (sel == 4) {
            ConsoleUI::restore();
            std::cout << "\n";
            u->afisare(std::cout);
            pausare();
            ConsoleUI::init();
        }
        else if (sel == 0) {
            std::vector<std::string> subOpt = {"Adaugă carte fizică", "Adaugă carte digitală", "Adaugă audiobook", "Șterge carte", "Restaurează carte", "Inventar", "Caută carte", "Modifică carte", "Mută carte fizică", "Gestiune Rafturi", "Înapoi"};
            int subIndex = 0;
            while(true) {
                afiseazaHeaderTUI(u);
                int s2 = ConsoleUI::showInteractiveMenu(2, 8, "📚 Gestiune Cărți", subOpt, subIndex);
                if (s2 == 10 || s2 == -1) break;
                
                ConsoleUI::restore();
                if(s2 == 0) adaugaCarteFizicaUI(bib);
                if(s2 == 1) adaugaCarteDigitalaUI(bib);
                if(s2 == 2) adaugaCarteAudioUI(bib);
                if(s2 == 3) stergeCarteUI(bib, u->getId());
                if(s2 == 4) restaureazaCarteUI(bib, u->getId());
                if(s2 == 5) vizualizareCatalogUI(bib);
                if(s2 == 6) cautaCarteUI(bib);
                if(s2 == 7) modificaCarteUI(bib, u->getId());
                if(s2 == 8) mutaCarteFizicaUI(bib);
                if(s2 == 9) gestiuneRafturiUI(bib);
                pausare();
                ConsoleUI::init();
            }
        }
        else if (sel == 1) {
            std::vector<std::string> subOpt = {"Adaugă utilizator", "Șterge utilizator", "Restaurează utilizator", "Afișează utilizatori", "Modifică utilizator", "Înapoi"};
            int subIndex = 0;
            while(true) {
                afiseazaHeaderTUI(u);
                int s2 = ConsoleUI::showInteractiveMenu(2, 8, "👥 Gestiune Utilizatori", subOpt, subIndex);
                if (s2 == 5 || s2 == -1) break;
                
                ConsoleUI::restore();
                if(s2 == 0) adaugaUtilizatorUI(bib);
                if(s2 == 1) stergeUtilizatorUI(bib, u->getId());
                if(s2 == 2) restaureazaUtilizatorUI(bib, u->getId());
                if(s2 == 3) bib.afiseazaUtilizatoriScurt(std::cout);
                if(s2 == 4) modificaUtilizatorUI(bib, u->getId());
                pausare();
                ConsoleUI::init();
            }
        }
        else if (sel == 2) {
            std::vector<std::string> subOpt = {"Împrumută carte", "Returnează carte", "Listă împrumuturi", "Confirmă returnări", "Confirmă ridicare rezervare", "Casare carte", "Înapoi"};
            int subIndex = 0;
            while(true) {
                afiseazaHeaderTUI(u);
                int s2 = ConsoleUI::showInteractiveMenu(2, 8, "📋 Împrumuturi & Retururi", subOpt, subIndex);
                if (s2 == 6 || s2 == -1) break;
                
                ConsoleUI::restore();
                if(s2 == 0) imprumutaCarteUI(bib);
                if(s2 == 1) returneazaCarteUI(bib, u->getId());
                if(s2 == 2) toateImprumuturileUI(bib);
                if(s2 == 3) confirmaReturnariUI(bib, u->getId());
                if(s2 == 4) confirmaRidicarRezervareUI(bib, u->getId());
                if(s2 == 5) casareCarteUI(bib, u->getId());
                pausare();
                ConsoleUI::init();
            }
        }
        else if (sel == 3) {
            std::vector<std::string> subOpt = {"Stare financiară", "⏰ Simulează trecerea timpului", "💸 Plătește salariile", "Achiziționează cărți", "Confirmă Plăți Amenzi", "Jurnal audit", "Înapoi"};
            int subIndex = 0;
            while(true) {
                afiseazaHeaderTUI(u);
                int s2 = ConsoleUI::showInteractiveMenu(2, 8, "💰 Finanțe & Audit", subOpt, subIndex);
                if (s2 == 6 || s2 == -1) break;
                
                ConsoleUI::restore();
                if(s2 == 0) {
                    std::cout << "\n  [ Finanțe ] Buget curent: " << Color::Green << bib.getBuget() << " RON" << Color::Reset << "\n";
                    std::cout << "  Salarii totale (2 săpt): " << Color::Yellow << bib.calculeazaSalariiTotale() << " RON" << Color::Reset << "\n";
                }
                if(s2 == 1) {
                    std::string input;
                    std::cout << "\n  Introduceți numărul de zile de simulat: ";
                    std::getline(std::cin, input);
                    try {
                        int zile = std::stoi(input);
                        if (zile > 0) bib.simuleazaTrecereTimp(zile);
                        else std::cout << "  Număr invalid.\n";
                    } catch(...) { std::cout << "  Eroare.\n"; }
                }
                if(s2 == 2) {
                    bib.platesteSalarii();
                }
                if(s2 == 3) achizitiiUI(bib, u->getId());
                if(s2 == 4) confirmaPlatiUI(bib, u->getId());
                if(s2 == 5) bib.afiseazaJurnalAudit(std::cout);
                pausare();
                ConsoleUI::init();
            }
        }
    }
}

static bool MeniuBibliotecar(Biblioteca& bib, const std::shared_ptr<Utilizator>& u) {
    std::vector<std::string> mainOpt = {
        "📚 Gestiune Cărți",
        "📋 Împrumuturi & Retururi",
        "👥 Afișează utilizatori",
        "👤 Detalii Cont",
        "⏰ Simulează Timpul",
        "🚪 Delogare"
    };
    int mainIndex = 0;
    
    while(true) {
        afiseazaHeaderTUI(u);
        int sel = ConsoleUI::showInteractiveMenu(2, 8, "Meniu Bibliotecar", mainOpt, mainIndex);
        
        if (sel == 5 || sel == -1) return false;
        
        if (sel == 4) {
            ConsoleUI::restore();
            std::string input;
            std::cout << "\n  Introduceți numărul de zile de simulat: ";
            std::getline(std::cin, input);
            try {
                int zile = std::stoi(input);
                if (zile > 0) bib.simuleazaTrecereTimp(zile);
                else std::cout << "  Număr invalid.\n";
            } catch(...) { std::cout << "  Eroare.\n"; }
            pausare();
            ConsoleUI::init();
        }
        else if (sel == 3) {
            ConsoleUI::restore();
            std::cout << "\n";
            u->afisare(std::cout);
            pausare();
            ConsoleUI::init();
        }
        else if (sel == 0) {
            std::vector<std::string> subOpt = {"Adaugă carte fizică", "Adaugă carte digitală", "Adaugă audiobook", "Șterge carte", "Restaurează carte", "Inventar", "Caută carte", "Modifică carte", "Mută carte fizică", "Gestiune Rafturi", "Înapoi"};
            int subIndex = 0;
            while(true) {
                afiseazaHeaderTUI(u);
                int s2 = ConsoleUI::showInteractiveMenu(2, 8, "📚 Gestiune Cărți", subOpt, subIndex);
                if (s2 == 10 || s2 == -1) break;
                
                ConsoleUI::restore();
                if(s2 == 0) adaugaCarteFizicaUI(bib);
                if(s2 == 1) adaugaCarteDigitalaUI(bib);
                if(s2 == 2) adaugaCarteAudioUI(bib);
                if(s2 == 3) stergeCarteUI(bib, u->getId());
                if(s2 == 4) restaureazaCarteUI(bib, u->getId());
                if(s2 == 5) vizualizareCatalogUI(bib);
                if(s2 == 6) cautaCarteUI(bib);
                if(s2 == 7) modificaCarteUI(bib, u->getId());
                if(s2 == 8) mutaCarteFizicaUI(bib);
                if(s2 == 9) gestiuneRafturiUI(bib);
                pausare();
                ConsoleUI::init();
            }
        }
        else if (sel == 1) {
            std::vector<std::string> subOpt = {"Împrumută carte", "Returnează carte", "Listă împrumuturi", "Confirmă returnări", "Confirmă ridicare rezervare", "Confirmă Plăți Amenzi", "Casare carte", "Înapoi"};
            int subIndex = 0;
            while(true) {
                afiseazaHeaderTUI(u);
                int s2 = ConsoleUI::showInteractiveMenu(2, 8, "📋 Împrumuturi & Retururi", subOpt, subIndex);
                if (s2 == 7 || s2 == -1) break;
                
                ConsoleUI::restore();
                if(s2 == 0) imprumutaCarteUI(bib);
                if(s2 == 1) returneazaCarteUI(bib, u->getId());
                if(s2 == 2) toateImprumuturileUI(bib);
                if(s2 == 3) confirmaReturnariUI(bib, u->getId());
                if(s2 == 4) confirmaRidicarRezervareUI(bib, u->getId());
                if(s2 == 5) confirmaPlatiUI(bib, u->getId());
                if(s2 == 6) casareCarteUI(bib, u->getId());
                pausare();
                ConsoleUI::init();
            }
        }
        else if (sel == 2) {
            ConsoleUI::restore();
            bib.afiseazaUtilizatoriScurt(std::cout);
            pausare();
            ConsoleUI::init();
        }
    }
}

static bool MeniuIngrijitor(Biblioteca& bib, const std::shared_ptr<Utilizator>& u) {
    std::vector<std::string> mainOpt = {
        "📖 Inventar cărți",
        "👥 Listă simplificată colegi",
        "🔧 Repară cărți defecte",
        "👤 Detalii Cont",
        "🚪 Delogare"
    };
    int mainIndex = 0;
    
    while(true) {
        afiseazaHeaderTUI(u);
        int sel = ConsoleUI::showInteractiveMenu(2, 8, "Meniu Îngrijitor", mainOpt, mainIndex);
        
        if (sel == 4 || sel == -1) return false;
        
        ConsoleUI::restore();
        if(sel == 3) {
            std::cout << "\n";
            u->afisare(std::cout);
        }
        else {
            if(sel == 0) vizualizareCatalogUI(bib);
            if(sel == 1) bib.afiseazaUtilizatoriScurt(std::cout);
            if(sel == 2) reconditioneazaCartiUI(bib);
        }
        pausare();
        ConsoleUI::init();
    }
}

static bool MeniuCititor(Biblioteca& bib, const std::shared_ptr<Utilizator>& u) {
    std::vector<std::string> mainOpt = {
        "📚 Catalog",
        "📋 Împrumuturile mele",
        "📌 Rezervări (Holds)",
        "📖 Lectură & Recenzii",
        "🏛️ Săli de lectură",
        "⚙️ Profilul meu",
        "🚪 Delogare"
    };
    int mainIndex = 0;
    
    while(true) {
        afiseazaHeaderTUI(u);
        int sel = ConsoleUI::showInteractiveMenu(2, 8, "Meniu Cititor", mainOpt, mainIndex);
        
        if (sel == 6 || sel == -1) return false;
        
        if (sel == 5) {
            std::vector<std::string> subOpt = {"Vezi detalii cont", "Modifică Profil", "Înapoi"};
            int subIndex = 0;
            while(true) {
                afiseazaHeaderTUI(u);
                int s2 = ConsoleUI::showInteractiveMenu(2, 8, "⚙️ Profilul meu", subOpt, subIndex);
                if (s2 == 2 || s2 == -1) break;
                
                ConsoleUI::restore();
                if(s2 == 0) {
                    std::cout << "\n";
                    u->afisare(std::cout);
                }
                if(s2 == 1) {
                    modificaProfilUI(bib, u->getId());
                }
                pausare();
                ConsoleUI::init();
            }
        }
        else if (sel == 0) {
            std::vector<std::string> subOpt = {"Inventar cărți", "Caută carte", "Înapoi"};
            int subIndex = 0;
            while(true) {
                afiseazaHeaderTUI(u);
                int s2 = ConsoleUI::showInteractiveMenu(2, 8, "📚 Catalog", subOpt, subIndex);
                if (s2 == 2 || s2 == -1) break;
                
                ConsoleUI::restore();
                if(s2 == 0) vizualizareCatalogUI(bib);
                if(s2 == 1) cautaCarteUI(bib);
                pausare();
                ConsoleUI::init();
            }
        }
        else if (sel == 1) {
            auto cit = std::dynamic_pointer_cast<Cititor>(u);
            bool areAmenzi = (cit && cit->getPenalizari() > 0);
            std::vector<std::string> subOpt = {"Vezi împrumuturile mele", "Împrumută o carte", "Returnează o carte"};
            if(areAmenzi) subOpt.push_back("Meniu Amenzi (💰)");
            subOpt.push_back("Înapoi");
            int subIndex = 0;
            int inapoi_idx = (int)subOpt.size() - 1;
            while(true) {
                afiseazaHeaderTUI(u);
                int s2 = ConsoleUI::showInteractiveMenu(2, 8, "📋 Împrumuturi", subOpt, subIndex);
                if (s2 == inapoi_idx || s2 == -1) break;
                
                ConsoleUI::restore();
                if(s2 == 0) veziImprumuturileMeleUI(bib, u->getId());
                if(s2 == 1) imprumutaCarteCititorUI(bib, u->getId());
                if(s2 == 2) returneazaCarteCititorUI(bib, u->getId());
                if(areAmenzi && s2 == 3) solicitaPlataUI(bib, u);
                pausare();
                ConsoleUI::init();
            }
        }
        else if (sel == 2) {
            std::vector<std::string> subOpt = {"Rezervă o carte", "Vezi rezervările mele", "Înapoi"};
            int subIndex = 0;
            while(true) {
                afiseazaHeaderTUI(u);
                int s2 = ConsoleUI::showInteractiveMenu(2, 8, "📌 Rezervări", subOpt, subIndex);
                if (s2 == 2 || s2 == -1) break;
                
                ConsoleUI::restore();
                if(s2 == 0) rezervaCarteUI(bib, u->getId());
                if(s2 == 1) bib.afiseazaRezervariCititor(std::cout, u->getId());
                pausare();
                ConsoleUI::init();
            }
        }
        else if (sel == 3) {
            std::vector<std::string> subOpt = {"Istoric lectură", "Scrie o recenzie", "Recenziile mele", "Înapoi"};
            int subIndex = 0;
            while(true) {
                afiseazaHeaderTUI(u);
                int s2 = ConsoleUI::showInteractiveMenu(2, 8, "📖 Lectură", subOpt, subIndex);
                if (s2 == 3 || s2 == -1) break;
                
                ConsoleUI::restore();
                if(s2 == 0) {
                    ConsoleUI::clearScreen();
                    ConsoleUI::drawBox(2, 2, 70, 20, "Istoric Lectură");
                    ConsoleUI::moveTo(4, 4);
                    bib.afiseazaIstoricCititor(std::cout, u->getId());
                }
                else if(s2 == 1) scrieRecenzieUI(bib, u->getId(), u->getNumeComplet());
                else if(s2 == 2) {
                    ConsoleUI::clearScreen();
                    ConsoleUI::drawBox(2, 2, 70, 20, "Recenziile Mele");
                    ConsoleUI::moveTo(4, 4);
                    bib.afiseazaRecenziileCititorului(std::cout, u->getId());
                }
                pausare();
                ConsoleUI::init();
            }
        }
        else if (sel == 4) {
            std::vector<std::string> subOpt = {"Rezervă sală de lectură", "Rezervările mele săli", "Înapoi"};
            int subIndex = 0;
            while(true) {
                afiseazaHeaderTUI(u);
                int s2 = ConsoleUI::showInteractiveMenu(2, 8, "🏛️ Săli de lectură", subOpt, subIndex);
                if (s2 == 2 || s2 == -1) break;
                
                ConsoleUI::restore();
                if(s2 == 0) rezervaSalaUI(bib, u->getId(), u->getNumeComplet());
                if(s2 == 1) veziRezervariSaliMeleUI(bib, u->getId());
                pausare();
                ConsoleUI::init();
            }
        }
    }
}

// ═══════════════════════════════════════════════
//  MAIN (Login Loop TUI)
// ═══════════════════════════════════════════════

int main() {
    Biblioteca bib;

    while (true) {
        ConsoleUI::init();
        ConsoleUI::drawBox(10, 5, 60, 7, "SISTEM DE GESTIUNE A BIBLIOTECII v4.0", ConsoleUI::Cyan);
        ConsoleUI::printAt(12, 7, "Autentificare securizată", ConsoleUI::Yellow, true);
        ConsoleUI::printAt(12, 9, "ID Utilizator : ");
        ConsoleUI::printAt(12, 10, "Parola        : ");
        
        std::string id = ConsoleUI::citesteStringLa(28, 9, 20);
        if (id == "exit") break;
        
        std::string p = ConsoleUI::citesteStringLa(28, 10, 20);

        auto u = bib.autentificare(id, p);
        if (!u) {
            ConsoleUI::showToast("❌ Autentificare eșuată! ID sau parolă incorectă.", ConsoleUI::Red);
            continue;
        }

        ConsoleUI::showToast("✅ Bine ai venit, " + u->getPrenume() + "!", ConsoleUI::Green);
        
        std::string tip = u->getTip();
        bool conectat = true;

        while (conectat) {
            if (tip == "DIRECTOR") conectat = MeniuDirector(bib, u);
            else if (tip == "BIBLIOTECAR") conectat = MeniuBibliotecar(bib, u);
            else if (tip == "INGRIJITOR") conectat = MeniuIngrijitor(bib, u);
            else if (tip == "CITITOR") conectat = MeniuCititor(bib, u);
        }
        
        bib.salveazaDate();
    }

    ConsoleUI::restore();
    std::cout << "\n  💾 La revedere! 👋\n\n";
    return 0;
}
