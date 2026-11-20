#include "menu_utils.h"
#include <iostream>
#include <string>
#include <limits>

int readInt(const std::string& prompt, int min, int max) {
    int value;

    while (true) {
        std::cout << prompt;
        std::cin >> value;

        if (std::cin.fail()) {
            std::cout << "Помилка! Введіть число.\n";

            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );
            continue;
        }

        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n'
        );

        if (value < min || value > max) {
            std::cout << "Помилка! Введіть число від "
                      << min << " до " << max << ".\n";
            continue;
        }

        return value;
    }
}

double readDouble(const std::string& prompt, double min, double max) {
    double value;

    while (true) {
        std::cout << prompt;
        std::cin >> value;

        if (std::cin.fail()) {
            std::cout << "Помилка! Введіть число.\n";

            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );
            continue;
        }

        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(), '\n'
        );

        if (value < min || value > max) {
            std::cout << "Помилка! Введіть число від "
                      << min << " до " << max << ".\n";
            continue;
        }

        return value;
    }
}

std::string readLine(const std::string& prompt) {
    std::string value;

    std::cout << prompt;
    std::getline(std::cin, value);

    return value;
}