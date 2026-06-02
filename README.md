# Library Management System

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![Qt](https://img.shields.io/badge/Qt-5.15%2B-green.svg)
![SQLite](https://img.shields.io/badge/SQLite-3-lightgrey.svg)
![CMake](https://img.shields.io/badge/CMake-3.10%2B-orange.svg)

**Library Management System** este o platformă software desktop robustă, dezvoltată în C++17 și Qt, proiectată pentru a gestiona integral fluxul operațional al unei biblioteci moderne. Acoperă administrarea membrilor, sistemul de împrumuturi și retururi (inclusiv tonomate automate), penalizările de întârziere (bază de date dependentă de un ceas simulat dinamic), un catalog vast de medii (fizice, e-book, audiobook) și un backend financiar complet.

---

## 🏗️ 1. Arhitectura Aplicației

Sistemul a fost construit folosind principiile **Programării Orientate pe Obiect (OOP)** — Abstractizare, Încapsulare, Moștenire și Polimorfism — asigurând un cod curat, scalabil și flexibil.

### Structura Claselor Principale (UML Simplificat)

1. **`Biblioteca`** (Clasa Singleton/Manager)
   Gestionează toate listele de obiecte în memorie și sincronizarea lor cu baza de date SQLite (`biblioteca.db`). Deține controlul central al logicii de business.
   
2. **Ierarhia `Utilizator`** (Polimorfism pe roluri)
   ```mermaid
   graph TD;
       Utilizator-->Director;
       Utilizator-->Bibliotecar;
       Utilizator-->Cititor;
       Utilizator-->Ingrijitor;
   ```
   *Rolurile definesc acțiunile permise pe interfața UI: de ex. Directorul poate executa plăți, Bibliotecarul gestionează catalogul și retururile.*

3. **Ierarhia `Carte`** (Polimorfism pe resurse)
   ```mermaid
   graph TD;
       Carte-->CarteFizica;
       Carte-->CarteDigitala;
       Carte-->CarteAudio;
   ```

---

## 🔒 2. Securitate: Sistemul de Autentificare și Criptare (XOR)

Toate parolele din sistem sunt criptate folosind o cheie statică prin algoritmul **XOR**, stocate sub formă de șiruri hexazecimale în fișierul `.db`. Autentificarea permite utilizarea fie a **ID**-ului unic, fie a **Adresei de Email**.

### Fragment de Cod: Algoritmul de Criptare (`Utilizator.cpp`)
```cpp
std::string Utilizator::xorEncode(const std::string& text) {
    const std::string cheie = "BibliotecaSecreta2025";
    std::string result = text;
    // Criptare pe baza cheii
    for (size_t i = 0; i < text.size(); i++) {
        result[i] = text[i] ^ cheie[i % cheie.size()];
    }
    return toHex(result); // Transformare in format Hexazecimal
}

bool Utilizator::verificaParola(const std::string& p) const {
    // Verificam parola in clar trecand-o prin acelasi algoritm si comparand hash-urile
    return parola == xorEncode(p); 
}
```

### Fragment de Cod: Autentificarea Hibridă (`Biblioteca.cpp`)
```cpp
std::shared_ptr<Utilizator> Biblioteca::autentificare(const std::string& id_or_email, const std::string& parola) const {
    auto u = gasesteUtilizator(id_or_email);
    if (!u) { // Daca nu s-a gasit dupa ID (ex: CIT001), cautam dupa adresa de email
        for (const auto& util : utilizatori) {
            if (util->getEmail() == id_or_email) {
                u = util; break;
            }
        }
    }
    // Daca s-a gasit contul, validam criptografic parola:
    if (u && u->verificaParola(parola) && u->getActiv()) {
        scrieLaLog(u->getId(), "AUTENTIFICARE: " + u->getRol());
        return u;
    }
    return nullptr;
}
```

---

## ⏳ 3. Gestiunea Timpului și Sistemul de Amenzi

Sistemul implementează propriul său ceas intern independent de cel al PC-ului (Simulare Timp). Atunci când zilele trec, aplicația iterază automat peste împrumuturile active și aplică penalizări (1 RON / Zi).

### Fragment de Cod: Aplicarea Automată a Penalizărilor
```cpp
void Biblioteca::avanseazaTimp(int zile) {
    auto t = QDateTime::fromString(QString::fromStdString(data_curenta), "dd/MM/yyyy");
    t = t.addDays(zile);
    data_curenta = t.toString("dd/MM/yyyy").toStdString();
    salveazaDataCurenta();

    // Actualizare dinamica a intarzierilor:
    for (auto& imp : imprumuturi) {
        int zile_intarziere = calculeazaZileIntarziere(imp.getTermenLimita());
        if (zile_intarziere > 0) {
            double penalizare = zile_intarziere * 1.0; // Amenda 1 RON pe zi
            auto cit = gasesteCititor(imp.getIdCititor());
            if (cit) {
                cit->setPenalizari(cit->getPenalizari() + penalizare);
                adaugaNotificare(cit->getId(), "Penalizare aplicată: " + std::to_string(penalizare) + " RON.");
            }
            // Resetam limita la ziua curenta pentru ca amenda se va percepe din nou maine
            imp.setTermenLimita(getDataCurentaStr());
        }
    }
}
```

---

## 🔄 4. Fluxul de Împrumut și Returnări (Sistem Tonomat)

Returnarea unei cărți nu o bagă direct pe raft. Ea trece printr-o coadă temporară `ReturnariInAsteptare`. Bibliotecarul este responsabil să valideze starea materială a cărții returnate.

### Fragment de Cod: Procesarea Returnării în Coadă
```cpp
bool Biblioteca::solicitaReturnare(const std::string& isbn, const std::string& id_cititor) {
    auto it = std::find_if(imprumuturi.begin(), imprumuturi.end(), [&](const Imprumut& i) {
        return i.getIdCarte() == isbn && i.getIdCititor() == id_cititor;
    });
    
    if (it != imprumuturi.end()) {
        std::string titlu = gasesteCarte(isbn)->getTitlu();
        std::string nume = gasesteUtilizator(id_cititor)->getNumeComplet();
        
        // Verificam pe loc daca e intarziata in momentul in care s-a pus la tonomat:
        double amenda = 0.0;
        int zile = calculeazaZileIntarziere(it->getTermenLimita());
        if (zile > 0) amenda = zile * 1.0; 
        
        // Cream intrarea in tabelul temporar
        ReturnareInAsteptare r = { isbn, id_cititor, nume, titlu, getDataCurentaStr(), amenda };
        returnari_in_asteptare.push_back(r);
        return true;
    }
    return false;
}
```

### Acceptare sau Refuz Carte (Bussines Logic Bibliotecar)
- **Confirmare:** Se eliberează stocul, se procesează amenda (dacă există) și dispare cartea din portofoliul utilizatorului.
- **Refuz:** Se marchează starea fizică drept `Defectă`. Cartea trebuie recondiționată sau casată din gestiune, iar cititorul primește imediat amenda pe valoarea cărții (înlocuire).

---

## 📊 5. Modulul de Bază de Date (SQLite Schema)

Aplicația se bazează pe o singură conexiune unificată. Există scheme relaționale pentru:
- `Utilizatori` (PK: id, rol, date_personale, parola_hash, abonament)
- `Carti` (PK: isbn, tip, autor, stoc, stare, url/durata)
- `Rafturi` & `CartiRafturi` (One-to-Many mapare între locația fizică și volum)
- `Imprumuturi`, `ReturnariInAsteptare` & `Rezervari` (Tranzacții)
- `Sistem` (Constante, ex: Data simulată și Buget Total)
- `Loguri` (Jurnal invizibil pentru Auditul Directorului)

---

## 🎨 6. Interfața Grafică (Qt Widgets)

UI-ul utilizează clase personalizate pentru un aspect imersiv, cu elemente tip Card (`QFrame`), Grid Layouts și Styling avansat via `QSS`. Butoanele de meniu stau într-un `QStackedWidget` cu side-navigation. Layout-ul se generează **dinamic** apelând rutine de UI separate (`QtMainWindow::createDashboardPage()`, `QtMainWindow::createProfilCititorPage()`).

---

## 🛠️ 7. Instrucțiuni de Compilare

Acest proiect folosește **CMake**. Ai nevoie de un compilator C++ (ex: g++, MSVC, Clang) și librăriile Qt5 / Qt6.

```bash
# 1. Clonează repository-ul / navighează în directorul de bază
cd LibraryManagementSystem

# 2. Creează și intră în folderul de build
mkdir build
cd build

# 3. Rulează CMake
cmake ..

# 4. Compilează Proiectul (pe Linux/WSL/MacOS)
make
# (pe Windows cu MSBuild / Visual Studio)
cmake --build .

# 5. Rulează fișierul generat (ex: biblioteca_qt / app.exe)
```
Dacă rulezi pe Windows, folosește scriptul rapid de rulare inclus: `run.bat`.
