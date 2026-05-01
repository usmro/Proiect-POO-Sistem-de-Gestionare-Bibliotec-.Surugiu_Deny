# 📚 Sistem de Gestiune a Bibliotecii

Un sistem complet de management al unei biblioteci, dezvoltat în **C++17** cu principii **OOP** (Programare Orientată pe Obiecte).

---

## 🏗️ Arhitectura Proiectului

```
LibraryManagementSystem/
├── Carte.h / Carte.cpp           # Clasă de bază abstractă
├── CarteFizica.h / CarteFizica.cpp   # Clasă derivată – cărți fizice
├── CarteDigitala.h / CarteDigitala.cpp # Clasă derivată – cărți digitale
├── Imprumut.h / Imprumut.cpp     # Clasă pentru gestionarea împrumuturilor
├── Biblioteca.h / Biblioteca.cpp # Manager principal (CRUD, căutare, persistență)
├── main.cpp                      # Interfața consolă (meniu interactiv)
├── Makefile                      # Build system
├── db_carti.txt                  # Baza de date – cărți (generat automat)
└── db_imprumuturi.txt            # Baza de date – împrumuturi (generat automat)
```

## ⚙️ Cerințe

- **OS**: WSL Ubuntu (sau orice distribuție Linux)
- **Compilator**: g++ cu suport C++17
- **Build**: make (opțional – se poate compila manual)

## 🚀 Compilare și Rulare

### Cu Makefile
```bash
cd /mnt/b/LibraryManagementSystem
make          # Compilează proiectul
./biblioteca  # Rulează aplicația
make clean    # Curăță fișierele obiect
make rebuild  # Recompilare completă
```

### Fără Makefile (compilare directă)
```bash
g++ -std=c++17 -Wall -O2 -o biblioteca \
    main.cpp Carte.cpp CarteFizica.cpp CarteDigitala.cpp \
    Imprumut.cpp Biblioteca.cpp
./biblioteca
```

## 📋 Funcționalități

### Gestiune Cărți
- ➕ Adaugă carte fizică (cu locație: clădire/cameră/culoar/raft)
- ➕ Adaugă carte digitală (cu format: PDF/audiobook/film + link acces)
- ➖ Șterge carte după ISSN
- 📖 Afișare detaliată a tuturor cărților (polimorfism)
- 📊 Inventar tabel cu statistici rapide

### 🔍 Motor de Căutare
- Caută după **autor** (substring, case-insensitive)
- Caută după **titlu** (substring, case-insensitive)
- Caută după **categorie**
- Caută după **an apariție** (exact)
- Caută după **nr. pagini** (interval min-max)

### 📋 Împrumuturi
- Înregistrare împrumut (verifică disponibilitatea)
- Returnare carte (marchează cartea ca disponibilă)
- Vizualizare împrumuturi active

### 💾 Persistență
- **Salvare automată** la închiderea programului (destructor)
- **Încărcare automată** la pornirea programului (constructor)
- Format: fișiere text cu delimitator `|`

## 🎓 Principii OOP Demonstrate

| Principiu | Implementare |
|-----------|-------------|
| **Încapsulare** | Atribute `private`/`protected`, acces prin getteri/setteri |
| **Moștenire** | `CarteFizica` ← `Carte`, `CarteDigitala` ← `Carte` |
| **Polimorfism** | `shared_ptr<Carte>`, metode virtuale pure `afisare()`, `formatFisier()`, `getTip()` |
| **Abstractizare** | `Carte` – clasă abstractă, nu poate fi instanțiată direct |
| **Supraîncărcare** | `operator<<` pentru afișare cu `std::cout` |

## 📝 Format Bază de Date

### db_carti.txt
```
FIZICA|titlu|autori|issn|pret|serie|poza|stare|disp|categorie|an|pagini|dimensiuni|greutate|coperta|cladire|camera|culoar|raft
DIGITALA|titlu|autori|issn|pret|serie|poza|stare|disp|categorie|an|pagini|format|dim_mb|link
```

### db_imprumuturi.txt
```
issn_carte|nume_cititor|data_imprumut|termen_limita|observatii
```

## 📜 Licență

Proiect educațional – uz liber.
