#include <iostream>
#include "Biblioteca.h"

using namespace std;

bool login() {
}

int main() {
    if (!login()) {
        cout << "Acces respins!\n";
        return 0;
    }

    Biblioteca b;
    int opt;

    do {
        cout << "\n===== BIBLIOTECA =====\n";
        cout << "1.Adauga carte\n2.Afiseaza\n3.Sterge carte\n";
        cout << "4.Adauga cititor\n5.Afiseaza cititori\n6.Sterge cititor\n";
        cout << "7.Imprumuta\n8.Returneaza\n9.Imprumuturi\n";
        cout << "10.Cauta autor\n11.Cauta titlu\n";
        cout << "12.Sort titlu\n13.Sort autor\n";
        cout << "14.Salveaza\n15.Incarca\n16.Statistici\n0.Exit\n";

        cin >> opt;

if (opt == 1) 
{
    string t, a, e, isbn;
    int an, p;

    cin.ignore();
    getline(cin, t);
    getline(cin, a);
    getline(cin, e);
    cin >> an >> p >> isbn;

    b.adaugaCarte(t, a, e, an, p, isbn);
}
else if (opt == 2) 
{
    b.afiseazaCarti();
}
else if (opt == 3) 
{
    int id;
    cin >> id;
    b.stergeCarte(id);
}
else if (opt == 4) 
{
    string n, p;
    cin >> n >> p;
    b.adaugaCititor(n, p);
}
else if (opt == 5) 
{
    b.afiseazaCititori();
}
else if (opt == 6) 
{
    int id;
    cin >> id;
    b.stergeCititor(id);
}
else if (opt == 7) 
{
    int idCarte, idCititor;
    cin >> idCarte >> idCititor;
    b.imprumutaCarte(idCarte, idCititor);
}
else if (opt == 8) 
{
    int id;
    cin >> id;
    b.returneazaCarte(id);
}
else if (opt == 9) 
{
    b.afiseazaImprumuturi();
}
else if (opt == 10) 
{
    string autor;
    cin >> autor;
    b.cautaDupaAutor(autor);
}
else if (opt == 11) 
{
    string titlu;
    cin >> titlu;
    b.cautaDupaTitlu(titlu);
}
else if (opt == 12) 
{
    b.sorteazaCartiTitlu();
}
else if (opt == 13) 
{
    b.sorteazaCartiAutor();
}
else if (opt == 14) 
{
    b.salveazaDate();
}
else if (opt == 15) 
{
    b.incarcaDate();
}
else if (opt == 16) 
{
    b.statistici();
}

    } while(opt!=0);

    return 0;
}
