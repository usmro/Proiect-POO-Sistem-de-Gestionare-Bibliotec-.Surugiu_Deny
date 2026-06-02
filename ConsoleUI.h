#pragma once
#include <string>
#include <vector>

namespace ConsoleUI {
    enum Color { Reset = 0, Red = 31, Green = 32, Yellow = 33, Blue = 34, Magenta = 35, Cyan = 36, White = 37 };
    
    // Inițializare și restaurare
    void init();
    void restore();

    // Control Cursor & Ecran
    void moveTo(int x, int y);
    void clearScreen();
    void clearLine();
    void clearArea(int x, int y, int width, int height);
    void hideCursor();
    void showCursor();

    // Control Culori
    void setTextColor(Color c, bool bold = false);
    void setBgColor(Color c);
    void resetColors();

    // Elemente Grafice
    void drawBox(int x, int y, int width, int height, const std::string& title = "", Color borderColor = Cyan);
    void printAt(int x, int y, const std::string& text, Color c = Reset, bool bold = false);
    
    // Componente Avansate
    void drawProgressBar(int x, int y, int width, float percentage, Color barColor = Green);
    void showToast(const std::string& message, Color typeColor = Green);

    // Input Control
    int getKeyPress();
    std::string citesteStringLa(int x, int y, int maxLen = 50);

    // Formulare
    std::string citesteFormularLinie(int x, int y, const std::string& prompt, int maxLen = 50);
    int citesteFormularInt(int x, int y, const std::string& prompt);
    double citesteFormularDouble(int x, int y, const std::string& prompt);

    // Meniu & Tabele
    int showInteractiveMenu(int x, int y, const std::string& title, const std::vector<std::string>& options, int& currentIndex);
    
    // Afișează un tabel interactiv paginat. Returnează indexul rândului selectat sau -1 pt anulare.
    int showPaginatedTable(int x, int y, const std::string& title, const std::vector<std::string>& headers, const std::vector<std::vector<std::string>>& rows);
}
