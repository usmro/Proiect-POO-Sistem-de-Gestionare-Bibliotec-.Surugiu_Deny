#include "Biblioteca.h"
#include <iostream>

int main() {
    Biblioteca bib("biblioteca.db", "db_utilizatori.txt", "db_carti.txt", "db_carti_sterse.txt", "db_utilizatori_stersi.txt", "db_rezervari.txt", "db_imprumuturi.txt", "db_returnari.txt", "db_plati.txt", "db_istoric.txt", "db_recenzii.txt", "db_rezervari_sali.txt", "db_rafturi.txt", "db_timp.txt", "db_notificari.txt", "export_rapoarte.txt", "audit.log", "config.ini");
    std::cout << "Books loaded: " << bib.getCarti().size() << std::endl;
    bib.salveazaDate();
    std::cout << "Books saved." << std::endl;
    return 0;
}
