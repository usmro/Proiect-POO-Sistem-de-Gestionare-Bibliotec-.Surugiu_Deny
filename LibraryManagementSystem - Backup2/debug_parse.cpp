#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

std::vector<std::string> splitLinie(const std::string& linie, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(linie);
    std::string token;
    while (std::getline(ss, token, delim)) {
        result.push_back(token);
    }
    return result;
}

int main() {
    std::ifstream fin_carti("db_carti.txt");
    std::string linie;
    int lineNum = 0;
    while (std::getline(fin_carti, linie)) {
        lineNum++;
        if (linie.empty()) continue;
        auto campuri = splitLinie(linie, '|');
        std::cout << "Line " << lineNum << " has " << campuri.size() << " fields.\n";
        for (size_t i = 0; i < campuri.size(); i++) {
            std::cout << "  " << i << ": [" << campuri[i] << "]\n";
        }
    }
    return 0;
}
