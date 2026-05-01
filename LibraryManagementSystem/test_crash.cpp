#include "Biblioteca.h"
#include <iostream>

int main() {
    std::cout << "Loading..." << std::endl;
    Biblioteca bib("db_carti.txt", "db_imprumuturi.txt", "db_utilizatori.txt", "db_returnari.txt");
    std::cout << "Loaded! Printing inventory..." << std::endl;
    bib.afiseazaInventarScurt(std::cout);
    std::cout << "Printed inventory! Now saving..." << std::endl;
    bib.salveazaDate();
    std::cout << "Saved! Done." << std::endl;
    return 0;
}
