#  Proiect POO – Sistem de Gestionare Bibliotecă

Programul permite:

*  Adăugarea de cărți
*  Ștergerea cărților
*  Afișarea tuturor cărților
*  Adăugarea cititorilor
* Gestionarea împrumuturilor
*  Căutarea după autor sau titlu
*  Sortarea cărților
*  Salvarea datelor în fișier
*  Încărcarea datelor din fișier
*  Afișarea statisticilor

---

## 🔹 Structura proiectului

Proiectul este împărțit în mai multe clase:

###  Clasa `Carte`

Reprezintă o carte din bibliotecă.

#### Variabile:

* `id` – identificator unic
* `titlu` – titlul cărții
* `autor` – autorul
* `editura` – editura
* `isbn` – cod unic internațional
* `an` – anul apariției
* `pagini` – număr de pagini
* `disponibila` – indică dacă poate fi împrumutată

#### Funcții:

* `getId()` – returnează ID-ul
* `getTitlu()` – returnează titlul
* `getAutor()` – returnează autorul
* `esteDisponibila()` – verifică disponibilitatea
* `imprumuta()` – marchează cartea ca împrumutată
* `returneaza()` – marchează cartea ca disponibilă
* `toCSV()` – conversie pentru salvare în fișier
* `toString()` – afișare formatată

---

###  Clasa `Cititor`

Reprezintă un utilizator al bibliotecii.

#### Variabile:

* `id` – identificator unic
* `nume` – nume
* `prenume` – prenume

#### Funcții:

* `getId()` – returnează ID-ul
* `getNume()` – returnează numele
* `getPrenume()` – returnează prenumele
* `toCSV()` – salvare în fișier
* `toString()` – afișare

---

###  Clasa `Imprumut`

Reprezintă un împrumut de carte.

#### Variabile:

* `idCarte` – ID-ul cărții
* `idCititor` – ID-ul cititorului
* `data` – data împrumutului
* `dataReturnare` – data limită

#### Funcții:

* `getIdCarte()` – returnează ID-ul cărții
* `getDataReturnare()` – returnează data limită
* `toCSV()` – salvare
* `toString()` – afișare

---

###  Clasa `Biblioteca`

Este clasa principală care gestionează toate operațiile.

#### Variabile:

* `vector<Carte> carti` – lista de cărți
* `vector<Cititor> cititori` – lista de cititori
* `vector<Imprumut> imprumuturi` – lista de împrumuturi
* `nextIdCarte` – generator ID carte
* `nextIdCititor` – generator ID cititor

#### Funcții:

###  Gestionare cărți

* `adaugaCarte(...)` – adaugă o carte
* `afiseazaCarti()` – afișează toate cărțile
* `stergeCarte(id)` – șterge o carte

###  Gestionare cititori

* `adaugaCititor(...)`
* `afiseazaCititori()`
* `stergeCititor(id)`

###  Împrumuturi

* `imprumutaCarte(idCarte, idCititor)`
* `returneazaCarte(idCarte)`
* `afiseazaImprumuturi()`

###  Căutare

* `cautaDupaAutor(autor)`
* `cautaDupaTitlu(titlu)`

###  Sortare

* `sorteazaCartiTitlu()`
* `sorteazaCartiAutor()`

###  Statistică

* `statistici()` – afișează număr total de:

  * cărți
  * cititori
  * împrumuturi

###  Fișiere

* `salveazaDate()` – salvează în fișier
* `incarcaDate()` – citește din fișier

---

##  Funcția `main`

Aceasta este punctul de intrare în program.

### Ce face:

1. Realizează autentificarea utilizatorului
2. Creează un obiect `Biblioteca`
3. Afișează un meniu interactiv
4. Permite utilizatorului să aleagă opțiuni

