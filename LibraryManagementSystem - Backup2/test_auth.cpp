#include "Biblioteca.h"
#include <iostream>

int main() {
    Biblioteca bib("db_carti.txt", "db_imprumuturi.txt", "db_utilizatori.txt");
    std::cout << "Utilizatori: " << bib.getNumarUtilizatori() << "\n";
    auto dir = bib.gasesteUtilizator("DIR001");
    if (dir) {
        std::cout << "Director gasit!\n";
        std::cout << "ID: " << dir->getId() << "\n";
        std::cout << "Parola cautata: 'admin' vs '" << dir->getParola() << "'\n";
        std::cout << "Activ: " << dir->getActiv() << "\n";
        
        if (bib.autentificare("DIR001", "admin")) {
            std::cout << "Auth SUCCESS\n";
        } else {
            std::cout << "Auth FAILED\n";
        }
    } else {
        std::cout << "Director NU A FOST GASIT!\n";
    }
    return 0;
}
