#include <iostream>
#include <windows.h>

#include "menu_utils.h"
#include "strings_task.h"
#include "types_task.h"
#include "recursion_task.h"
#include "files_task.h"
#include "oop_task.h"

using namespace std;

// Головне меню програми
void mainMenu() {
    int choice;

    do {
        cout << "\n================================\n";
        cout << "       НАВЧАЛЬНИЙ КОМПЛЕКС\n";
        cout << "================================\n";
        cout << "1. Робота з рядками\n";
        cout << "2. Користувацькі типи даних\n";
        cout << "3. Рекурсивні функції\n";
        cout << "4. Робота з файлами\n";
        cout << "5. ООП\n";
        cout << "0. Вихід\n";
        cout << "================================\n";

        choice = readInt("Ваш вибір: ", 0, 5);

        switch (choice) {
            case 1:
                stringsMenu();
                break;

            case 2:
                typesMenu();
                break;

            case 3:
                recursionMenu();
                break;

            case 4:
                filesMenu();
                break;

            case 5:
                oopMenu();
                break;

            case 0:
                cout << "\nЗавершення програми...\n";
                break;
        }

    } while (choice != 0);
}

// Головна функція
int main() {
    // Налаштування кодування консолі Windows
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    mainMenu();

    return 0;
}