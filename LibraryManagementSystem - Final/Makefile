# ═══════════════════════════════════════════════
#  Makefile – Sistem de Gestiune a Bibliotecii v2
#  Compilare: make
#  Rulare:    ./biblioteca
#  Curățare:  make clean
# ═══════════════════════════════════════════════

CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -O2
TARGET   = biblioteca

# Fișiere sursă
SRCS = main.cpp Carte.cpp CarteFizica.cpp CarteDigitala.cpp CarteAudio.cpp \
       Imprumut.cpp Biblioteca.cpp \
       Utilizator.cpp Director.cpp Bibliotecar.cpp Ingrijitor.cpp Cititor.cpp \
       ConsoleUI.cpp ui_functii.cpp

# Fișiere obiect (generate automat din surse)
OBJS = $(SRCS:.cpp=.o)

# ── Regula principală ──
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) -lsqlite3

# ── Regulă generică pentru .cpp → .o ──
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ── Dependențe explicite ──
main.o:          main.cpp Biblioteca.h Carte.h CarteFizica.h CarteDigitala.h Imprumut.h \
                 Utilizator.h Director.h Bibliotecar.h Ingrijitor.h Cititor.h
Carte.o:         Carte.cpp Carte.h
CarteFizica.o:   CarteFizica.cpp CarteFizica.h Carte.h
CarteDigitala.o: CarteDigitala.cpp CarteDigitala.h Carte.h
CarteAudio.o:    CarteAudio.cpp CarteAudio.h Carte.h
Imprumut.o:      Imprumut.cpp Imprumut.h
Biblioteca.o:    Biblioteca.cpp Biblioteca.h Carte.h CarteFizica.h CarteDigitala.h Imprumut.h \
                 Utilizator.h Director.h Bibliotecar.h Ingrijitor.h Cititor.h
Utilizator.o:   Utilizator.cpp Utilizator.h
Director.o:     Director.cpp Director.h Utilizator.h
Bibliotecar.o:  Bibliotecar.cpp Bibliotecar.h Utilizator.h
Ingrijitor.o:   Ingrijitor.cpp Ingrijitor.h Utilizator.h
Cititor.o:      Cititor.cpp Cititor.h Utilizator.h

# ── Curățare ──
clean:
	rm -f $(OBJS) $(TARGET)

# ── Rebuild complet ──
rebuild: clean all

.PHONY: all clean rebuild
