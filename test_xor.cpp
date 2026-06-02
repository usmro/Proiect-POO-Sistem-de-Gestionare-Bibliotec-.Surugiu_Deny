#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

std::string toHex(const std::string& data) {
    std::ostringstream oss;
    for (unsigned char c : data) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)c;
    }
    return oss.str();
}

std::string xorEncode(const std::string& text) {
    const std::string cheie = "BibliotecaSecreta2025";
    std::string result = text;
    for (size_t i = 0; i < text.size(); i++)
        result[i] = text[i] ^ cheie[i % cheie.size()];
    return toHex(result);
}

int main() {
    std::cout << "admin: " << xorEncode("admin") << std::endl;
    std::cout << "1234: " << xorEncode("1234") << std::endl;
    return 0;
}
