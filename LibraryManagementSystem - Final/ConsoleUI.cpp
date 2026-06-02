#include "ConsoleUI.h"
#include <iostream>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#endif

namespace ConsoleUI {

    void init() {
#ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
#else
        // Nimic special de inițializat global pentru Linux (ANSI merge nativ)
#endif
        hideCursor();
        clearScreen();
    }

    void restore() {
        showCursor();
        resetColors();
        clearScreen();
        moveTo(1, 1);
    }

    void moveTo(int x, int y) {
        std::cout << "\033[" << y << ";" << x << "H";
    }

    void clearScreen() {
        std::cout << "\033[2J\033[H";
    }

    void clearLine() {
        std::cout << "\033[2K";
    }

    void clearArea(int x, int y, int width, int height) {
        std::string spaces(width, ' ');
        for (int i = 0; i < height; ++i) {
            moveTo(x, y + i);
            std::cout << spaces;
        }
    }

    void hideCursor() {
        std::cout << "\033[?25l";
    }

    void showCursor() {
        std::cout << "\033[?25h";
    }

    void setTextColor(Color c, bool bold) {
        if (bold)
            std::cout << "\033[1;" << (int)c << "m";
        else
            std::cout << "\033[0;" << (int)c << "m";
    }

    void setBgColor(Color c) {
        std::cout << "\033[" << (int)c + 10 << "m";
    }

    void resetColors() {
        std::cout << "\033[0m";
    }

    void drawBox(int x, int y, int width, int height, const std::string& title, Color borderColor) {
        setTextColor(borderColor, true);
        moveTo(x, y); std::cout << "┌";
        for (int i = 0; i < width - 2; ++i) std::cout << "─";
        std::cout << "┐";

        for (int i = 1; i < height - 1; ++i) {
            moveTo(x, y + i); std::cout << "│";
            moveTo(x + width - 1, y + i); std::cout << "│";
        }

        moveTo(x, y + height - 1); std::cout << "└";
        for (int i = 0; i < width - 2; ++i) std::cout << "─";
        std::cout << "┘";

        if (!title.empty()) {
            moveTo(x + 2, y);
            setTextColor(Yellow, true);
            std::cout << " " << title << " ";
        }
        resetColors();
    }

    void printAt(int x, int y, const std::string& text, Color c, bool bold) {
        moveTo(x, y);
        if (c != Reset) setTextColor(c, bold);
        std::cout << text;
        if (c != Reset) resetColors();
    }

    void drawProgressBar(int x, int y, int width, float percentage, Color barColor) {
        int filled = (int)(width * (percentage / 100.0f));
        moveTo(x, y);
        setTextColor(barColor, true);
        for (int i = 0; i < filled; ++i) std::cout << "█";
        setTextColor(White, false);
        for (int i = filled; i < width; ++i) std::cout << "░";
        resetColors();
    }

    void showToast(const std::string& message, Color typeColor) {
        // Vom desena toast-ul jos de tot pe ecran
        int toastY = 30; // Aproximare
        
        drawBox(2, toastY, message.length() + 6, 3, "", typeColor);
        printAt(5, toastY + 1, message, typeColor, true);
        std::cout << std::flush;
        
        // Wait 1.5 seconds
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        
        // Clear toast area
        clearArea(2, toastY, message.length() + 6, 3);
    }

    int getKeyPress() {
#ifdef _WIN32
        int ch = _getch();
        if (ch == 0 || ch == 224) { // arrow keys in Windows return 0 or 224 first
            int arrow = _getch();
            switch (arrow) {
                case 72: return 1; // Up
                case 80: return 2; // Down
                case 75: return 3; // Left
                case 77: return 4; // Right
            }
        }
        return ch; // ASCII (Enter=13, Esc=27)
#else
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        
        if (ch == 27) { // Escape sequence or just ESC
            struct termios temp;
            tcgetattr(STDIN_FILENO, &temp);
            temp.c_cc[VMIN] = 0;
            temp.c_cc[VTIME] = 1;
            tcsetattr(STDIN_FILENO, TCSANOW, &temp);
            
            int ch2 = getchar();
            if (ch2 == EOF || ch2 == -1) {
                tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
                return 27; // ESC key
            }
            if (ch2 == 91) { // '['
                int ch3 = getchar();
                tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
                switch(ch3) {
                    case 65: return 1; // Up
                    case 66: return 2; // Down
                    case 68: return 3; // Left
                    case 67: return 4; // Right
                }
                return 27;
            }
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            return 27;
        }
        
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return (ch == 10) ? 13 : ch; // Treat '\n' as Enter (13)
#endif
    }

    std::string citesteStringLa(int x, int y, int maxLen) {
        showCursor();
        std::string result = "";
        
        // Disable canonical mode for character by character reading so we can echo at specific spot
#ifndef _WIN32
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
#endif

        while (true) {
            moveTo(x, y);
            std::cout << result << " \033[1D" << std::flush; // Desenam si suprascriem caracterul sters (daca e cazul)

#ifdef _WIN32
            int ch = _getch();
            if (ch == 13) break; // Enter
            if (ch == 8) { // Backspace
                if (result.length() > 0) {
                    result.pop_back();
                    moveTo(x + result.length(), y);
                    std::cout << " ";
                }
            } else if (ch >= 32 && ch <= 126 && result.length() < (size_t)maxLen) {
                result += (char)ch;
            }
#else
            int ch = getchar();
            if (ch == 10 || ch == 13) break; // Enter
            if (ch == 127 || ch == 8) { // Backspace in Linux is usually 127
                if (result.length() > 0) {
                    result.pop_back();
                    moveTo(x + result.length(), y);
                    std::cout << " ";
                }
            } else if (ch >= 32 && ch <= 126 && result.length() < (size_t)maxLen) {
                result += (char)ch;
            }
#endif
        }

#ifndef _WIN32
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
        hideCursor();
        return result;
    }

    int showInteractiveMenu(int x, int y, const std::string& title, const std::vector<std::string>& options, int& currentIndex) {
        int height = options.size() + 4;
        int maxWidth = title.length() + 4;
        for (const auto& opt : options) {
            if (opt.length() + 8 > (size_t)maxWidth) maxWidth = opt.length() + 8;
        }

        bool inMenu = true;
        while (inMenu) {
            drawBox(x, y, maxWidth, height, title);
            for (size_t i = 0; i < options.size(); ++i) {
                moveTo(x + 2, y + 2 + i);
                if ((int)i == currentIndex) {
                    std::cout << "\033[7m" << " > " << options[i] << std::string(maxWidth - options[i].length() - 7, ' ') << " \033[27m";
                } else {
                    std::cout << "   " << options[i] << std::string(maxWidth - options[i].length() - 7, ' ') << " ";
                }
            }
            std::cout << std::flush;

            int key = getKeyPress();
            if (key == 1) { // Up
                currentIndex--;
                if (currentIndex < 0) currentIndex = options.size() - 1;
            } else if (key == 2) { // Down
                currentIndex++;
                if (currentIndex >= (int)options.size()) currentIndex = 0;
            } else if (key == 13 || key == 10) { // Enter
                return currentIndex;
            } else if (key == 27 || key == 8 || key == 127) { // Esc or Backspace
                return -1;
            }
        }
        return -1;
    }

    std::string citesteFormularLinie(int x, int y, const std::string& prompt, int maxLen) {
        printAt(x, y, prompt, Cyan);
        return citesteStringLa(x + prompt.length(), y, maxLen);
    }

    int citesteFormularInt(int x, int y, const std::string& prompt) {
        while (true) {
            printAt(x, y, prompt, Cyan);
            std::string val = citesteStringLa(x + prompt.length(), y, 15);
            try {
                if(!val.empty()) return std::stoi(val);
                return 0; // if empty, just return 0 to allow skipping
            } catch (...) {
                showToast("❌ Introduceti un numar valid!", Red);
                clearArea(x + prompt.length(), y, 15, 1);
            }
        }
    }

    double citesteFormularDouble(int x, int y, const std::string& prompt) {
        while (true) {
            printAt(x, y, prompt, Cyan);
            std::string val = citesteStringLa(x + prompt.length(), y, 15);
            try {
                if(!val.empty()) return std::stod(val);
                return 0.0;
            } catch (...) {
                showToast("❌ Introduceti un numar valid!", Red);
                clearArea(x + prompt.length(), y, 15, 1);
            }
        }
    }

    int showPaginatedTable(int x, int y, const std::string& title, const std::vector<std::string>& headers, const std::vector<std::vector<std::string>>& rows) {
        int itemsPerPage = 10;
        int totalPages = (rows.size() + itemsPerPage - 1) / itemsPerPage;
        if(totalPages == 0) totalPages = 1;
        int currentPage = 0;
        int selectedRow = 0; // global index

        std::vector<int> colWidths(headers.size(), 0);
        for(size_t i=0; i<headers.size(); ++i) colWidths[i] = headers[i].length() + 2;
        for(const auto& r : rows) {
            for(size_t i=0; i<r.size() && i<colWidths.size(); ++i) {
                if(r[i].length() + 2 > (size_t)colWidths[i]) colWidths[i] = r[i].length() + 2;
            }
        }
        int totalWidth = 2; // margins
        for(int w : colWidths) totalWidth += w + 1; // plus separators
        
        while(true) {
            clearArea(x, y, totalWidth + 4, itemsPerPage + 6);
            drawBox(x, y, totalWidth, itemsPerPage + 6, title + " (Pag " + std::to_string(currentPage+1) + "/" + std::to_string(totalPages) + ")");
            
            // Draw Headers
            int curX = x + 2;
            for(size_t i=0; i<headers.size(); ++i) {
                printAt(curX, y + 2, headers[i], Yellow, true);
                curX += colWidths[i] + 1;
            }
            
            int startIdx = currentPage * itemsPerPage;
            int endIdx = startIdx + itemsPerPage;
            if(endIdx > (int)rows.size()) endIdx = rows.size();
            
            for(int i = startIdx; i < endIdx; ++i) {
                int rowY = y + 4 + (i - startIdx);
                curX = x + 2;
                if(i == selectedRow) {
                    moveTo(curX, rowY);
                    std::cout << "\033[7m"; // Invert
                }
                for(size_t c=0; c<rows[i].size() && c<colWidths.size(); ++c) {
                    moveTo(curX, rowY);
                    std::string cell = rows[i][c];
                    if(cell.length() > (size_t)colWidths[c]-2) {
                        cell = cell.substr(0, colWidths[c]-5) + "...";
                    }
                    std::cout << cell << std::string(colWidths[c] - cell.length(), ' ');
                    curX += colWidths[c] + 1;
                }
                if(i == selectedRow) {
                    std::cout << "\033[27m";
                }
            }
            std::cout << std::flush;
            
            int key = getKeyPress();
            if(key == 1) { // Up
                selectedRow--;
                if(selectedRow < 0) selectedRow = rows.size() - 1;
                currentPage = selectedRow / itemsPerPage;
            } else if(key == 2) { // Down
                selectedRow++;
                if(selectedRow >= (int)rows.size()) selectedRow = 0;
                currentPage = selectedRow / itemsPerPage;
            } else if(key == 13 || key == 10) { // Enter
                if(rows.empty()) return -1;
                return selectedRow;
            } else if(key == 27 || key == 8 || key == 127) { // Esc or Backspace
                return -1;
            }
        }
        return -1;
    }
}
