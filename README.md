# 📚 Library Management System (LMS)

![C++](https://img.shields.io/badge/C++-17-blue.svg?style=for-the-badge&logo=c%2B%2B)
![Qt](https://img.shields.io/badge/Qt-5.15%2B-41CD52.svg?style=for-the-badge&logo=qt)
![SQLite](https://img.shields.io/badge/SQLite-3-003B57.svg?style=for-the-badge&logo=sqlite)
![CMake](https://img.shields.io/badge/CMake-3.10%2B-064F8C.svg?style=for-the-badge&logo=cmake)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20Mac-lightgrey.svg?style=for-the-badge)

**Library Management System** este o platformă software completă și robustă, dezvoltată în **C++17**, proiectată pentru a digitaliza și automatiza integral fluxul operațional al unei biblioteci moderne. 

Proiectul se distinge printr-o arhitectură duală hibridă, oferind atât o **Interfață Grafică Premium (GUI)** bazată pe Qt, cât și o **Interfață pe Bază de Text (TUI/CLI)** integrată direct în terminal pentru medii server sau depanare.

---

## 📑 Cuprins
1. [Funcționalități Principale](#-1-funcționalități-principale)
2. [Arhitectura și Design Patterns](#-2-arhitectura-și-design-patterns)
3. [Arhitectura Duală (GUI vs CLI)](#-3-arhitectura-duală-gui-vs-cli)
4. [Sistemul de Securitate](#-4-sistemul-de-securitate-și-criptare)
5. [Structura Bazei de Date SQLite](#-5-structura-bazei-de-date-sqlite)
6. [Gestiunea Timpului și Financiară](#-6-gestiunea-timpului-și-financiară)
7. [Instrucțiuni de Compilare și Rulare](#-7-instrucțiuni-de-compilare-și-rulare)
8. [Structura Proiectului (Fișiere)](#-8-structura-proiectului)

---

## ✨ 1. Funcționalități Principale

- **Gestiunea Utilizatorilor (RBAC):** Sistem de acces bazat pe roluri (Director, Bibliotecar, Îngrijitor, Cititor), fiecare cu ecrane, dashboard-uri și permisiuni unice.
- **Catalog Multi-Media:** Suport nativ pentru Cărți Fizice (cu locație exactă pe rafturi), Cărți Digitale (eBooks PDF/ePub cu stoc nelimitat) și Audiobook-uri.
- **Sistem de Tonomat pentru Retururi:** Cărțile returnate intră într-o „Coadă de Așteptare” pentru a fi inspectate fizic de un bibliotecar înainte de a fi reintroduse pe raft.
- **Casare și Mentenanță:** Gestionarea cărților degradate și mutarea logică a volumelor între rafturi sau săli de lectură.
- **Simulare Temporală:** Un „ceas intern” independent de sistemul de operare care permite testarea fluxurilor, generarea automată a amenzilor și plata salariilor la trecerea timpului.
- **Jurnal de Audit Incoruptibil:** Toate acțiunile din sistem sunt înregistrate tăcut într-o bază de date securizată accesibilă doar Directorului.

---

## 🏗️ 2. Arhitectura și Design Patterns

Sistemul a fost construit folosind principii solide de **Software Engineering** (SOLID) și **Programare Orientată pe Obiect (OOP)**.

### Design Patterns Utilizate:
1. **Repository / Manager Pattern:** Clasa centrală `Biblioteca` acționează ca un *Single Source of Truth*, gestionând toate instanțele în memorie și sincronizând stările obiectelor direct cu baza de date SQLite.
2. **Polimorfism Dinamic:**
   - **Ierarhia `Utilizator`**: Metodele virtuale adaptează accesul la meniuri (`Director`, `Bibliotecar`, etc.).
   - **Ierarhia `Carte`**: Metode specifice pentru tipul resurselor (ex: doar `CarteFizica` are `LocatieRaft`, în timp ce `CarteAudio` are `Durata`). `std::dynamic_pointer_cast` este folosit extensiv pentru a extrage proprietățile unice la runtime.
3. **Delegation / Strategy (în UI):** Funcțiile care desenează paginile Qt sunt delegate prin funcții anonime (lambdas) către componente specializate (`QtFunc`).

---

## 🖥️ 3. Arhitectura Duală (GUI vs CLI)

Proiectul oferă o decuplare totală a logicii de business față de interfața vizuală. Aceasta se reflectă în existența a două build-uri diferite din același cod sursă de bază:

### A. Varianta Desktop (Qt6)
Construită pe arhitectura Semnal-Slot din Qt. Dispune de o tematică modernă (stilizată prin QSS - Qt Style Sheets), ferestre modale, layout-uri adaptive (Grid, VBox), panouri de notificări și tabele sortabile dinamic. (*Compilat prin: `biblioteca_qt`*)

### B. Varianta Terminal (TUI)
O interfață text complet customizată implementată în `ConsoleUI.cpp`. Evită comenzile banale de tip `std::cin` și folosește manipulări de buffere ANSI / Termios pentru a crea "ferestre" pop-up, liste paginate și butoane selectabile folosind doar text (ASCII Art). (*Compilat prin: `app`*)

---

## 🔒 4. Sistemul de Securitate și Criptare

Toate parolele din sistem sunt criptate folosind un algoritm didactic **XOR Hashing**, urmat de o encodare **Hexazecimală**, prevenind stocarea în clar a datelor vulnerabile.

**Snippet - Verificarea Hibridă a Autentificării:**
```cpp
std::shared_ptr<Utilizator> Biblioteca::autentificare(const std::string& id_or_email, const std::string& parola) const {
    auto u = gasesteUtilizator(id_or_email);
    if (!u) { 
        // Dacă login-ul nu este de tip "CIT001", căutăm iterativ adresa de email
        for (const auto& util : utilizatori) {
            if (util->getEmail() == id_or_email) {
                u = util; break;
            }
        }
    }
    // Validare folosind trecerea parolei scrise prin filtrul XOR și compararea cu Hash-ul DB
    if (u && u->verificaParola(parola) && u->getActiv()) {
        scrieLaLog(u->getId(), "AUTENTIFICARE SUCCES: " + u->getRol());
        return u;
    }
    return nullptr;
}
```

---

## 🗄️ 5. Structura Bazei de Date SQLite

Sistemul elimină dependența de servere SQL mari utilizând librăria portabilă `sqlite3.h`. Toate modificările din memorie sunt trimise tranzacțional către fișierul local `biblioteca.db`.

| Tabel / Entitate | Descriere și Rol |
| :--- | :--- |
| **`Utilizatori`** | Conține ID, Rol, Nume, Date Contact, Parola(Hash), Abonament. |
| **`Carti`** | Parametrii universali: ISBN, Titlu, Tip, Stoc. Parametri specifici se stochează sub format string/JSON sau câmpuri flexibile. |
| **`Rafturi`** | Structura ierarhică fizică (Clădire -> Sală -> Culoar -> Raft). |
| **`Imprumuturi`** | Tranzacții active. Leagă un `ID_Utilizator` de un `ISBN` cu un *Termen Limita*. |
| **`ReturnariInAsteptare`**| Tabela temporară pentru Tonomat. Reține cărțile depuse fizic dar nevalidate încă de staff. |
| **`Loguri`** | Jurnalul "Write-Only" pentru auditul sistemului. |

---

## ⏳ 6. Gestiunea Timpului și Financiară

Biblioteca nu depinde de data calculatorului tău pentru a sancționa utilizatorii. Modulul de "Simulare Timp" permite administratorilor să "sară" în viitor. 
1. **Sistemul de Amenzi:** Dacă se sare o lună, motorul iterează peste împrumuturi și aplică **1 RON pentru fiecare zi de întârziere**. Penalizările sunt lipite de contul utilizatorului.
2. **Plata Salariilor:** O dată pe lună (pe data de 15 a datei simulate), Directorul trebuie să efectueze plata salariilor personalului. Banii sunt sustrași automat din *Bugetul Global*.
3. **Venituri:** Bugetul crește strict din colectarea amenzilor de la utilizatorii neglijenți.

---

## 🛠️ 7. Instrucțiuni de Compilare și Rulare

Acest proiect necesită un compilator C++ compatibil **C++17** și sistemul de build **CMake**. 

### Condiții prealabile:
- [Qt 5.15 sau Qt 6.x](https://www.qt.io/) (Module Necesare: `Core`, `Gui`, `Widgets`)
- MinGW / GCC / Clang
- CMake 3.16+

### Rulare pe Windows (Folosind WSL / Linux Toolkit)
```bash
# 1. Creează folderul de compilare
mkdir build && cd build

# 2. Generează fișierele Ninja/Make
cmake ..

# 3. Construiește executabilul UI Qt
cmake --build .
# ./biblioteca_qt va fi generat

# (Opțional) Pentru a construi versiunea veche de Terminal:
cd ..
make TARGET=app
# ./app va fi generat
```

***Rulare cu wsl: instalare cmake si qt6:
1.sudo apt update
   sudo apt install build-essential cmake
2.sudo apt install qt6-base-dev qt6-tools-dev
> **Notă rapidă pentru Windows:** Poți folosi scripturile batch incluse direct în root: `run.bat` (pentru consola TUI) sau poți integra CMakeLists în Visual Studio / CLion.

---

## 📂 8. Structura Proiectului

```text
LibraryManagementSystem/
├── CMakeLists.txt         # Configurarea pentru compilarea Qt
├── Makefile               # Configurarea pentru compilarea aplicației de Consolă
├── biblioteca.db          # Baza de date SQLite (Locala)
├── README.md              # Documentația prezentă
├── run.bat                # Script de execuție Windows
├── Core Backend/          # Fișiere de logică de business
│   ├── Biblioteca.cpp/.h
│   ├── Carte*.cpp/.h      # Fiecare tip de carte (Fizică, Digitală, Audio)
│   ├── Imprumut.cpp/.h
│   └── Utilizator*.cpp/.h # Fiecare tip de rol (Director, Cititor, etc.)
├── GUI Frontend/          # Interfața Qt
│   ├── main_qt.cpp
│   ├── QtMainWindow.cpp/.h
│   └── QtDialogs.cpp/.h
└── TUI Frontend/          # Interfața de Consolă/Terminal
    ├── main.cpp
    ├── ConsoleUI.cpp/.h
    └── ui_functii.cpp/.h
```

---
> Proiect realizat pentru a demonstra competențe avansate de C++, Software Architecture, lucrul cu baze de date (SQL) și GUI Development (Qt).
