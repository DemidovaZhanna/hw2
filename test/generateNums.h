#ifndef GENERATENUMS_H
#define GENERATENUMS_H

#include <fstream>
#include <random>
#include <string>
#include <filesystem>

inline void generateNumbersFile(const std::string& filename, size_t digits)
{
    if (std::filesystem::exists(filename))
    {
        std::ifstream file(filename, std::ios::ate);
        if (file.tellg() > 0)
            return;
        file.close();
    }

    std::ofstream file(filename);
    if (!file)
        throw std::runtime_error("Unable to create file");

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 9);

    for (size_t i = 0; i < digits; ++i)
        file << dis(gen);

    file << '\n';

    for (size_t i = 0; i < digits; ++i)
        file << dis(gen);

    file.close();
}

inline bool readNumbersFromFile(std::string& num1, std::string& num2, std::string& expected_product, std::streampos& last_pos)
{
    std::string filename = "../test/numbers.txt";
    std::ifstream file(filename);
    if (!file)
    {
        std::cerr << "Unable to open file: " << filename << '\n';
        return false;
    }

    if (last_pos != std::streampos(0))
    {
        file.seekg(last_pos);
    }

    if (!std::getline(file, num1) || !std::getline(file, num2) || !std::getline(file, expected_product))
    {
        std::cerr << "Error reading data from file.\n";
        return false;
    }
    last_pos = file.tellg();
    file.close();

    return true;
}

#endif //GENERATENUMS_H
