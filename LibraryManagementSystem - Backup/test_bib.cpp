#include "Biblioteca.h"
#include <iostream>

int main() {
    try {
        Biblioteca bib("db_carti.txt", "db_imprumuturi.txt", "db_utilizatori.txt");
        std::cout << "Loaded successfully.\n";
        bib.afiseazaInventarScurt(std::cout);
        std::cout << "Displayed successfully.\n";
    } catch(const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    } catch(...) {
        std::cerr << "Unknown exception\n";
    }
    return 0;
}
