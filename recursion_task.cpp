#include "recursion_task.h"
#include "menu_utils.h"

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

vector<string> splitChars(string text);
string toLowerChar(string c);

long long fibCalls = 0;
int hanoiMoves = 0;

// Базовий випадок: n <= 1, результат 1
// Рекурсивний крок: n! = n * (n - 1)!
long long factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

long long factorialIter(int n) {
    long long result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

void factorialTask() {
    int n = readInt("Введіть n (0-20): ", 0, 20);

    long long r1 = factorial(n);
    long long r2 = factorialIter(n);

    cout << "Рекурсія: " << n << "! = " << r1 << endl;
    cout << "Ітерація: " << n << "! = " << r2 << endl;

    if (r1 == r2) {
        cout << "Результати збігаються.\n";
    }
}

// Базовий випадок: n <= 1, результат дорівнює n
// Рекурсивний крок: F(n) = F(n - 1) + F(n - 2)
long long fibonacci(int n) {
    fibCalls++;
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

long long fibonacciIter(int n) {
    long long a = 0, b = 1;
    for (int i = 0; i < n; i++) {
        long long next = a + b;
        a = b;
        b = next;
    }
    return a;
}

void fibonacciNumber() {
    int n = readInt("Введіть номер числа (0-40): ", 0, 40);

    fibCalls = 0;
    long long result = fibonacci(n);

    cout << "F(" << n << ") = " << result << endl;
    cout << "Кількість рекурсивних викликів: " << fibCalls << endl;
}

void fibonacciList() {
    int n = readInt("Скільки чисел вивести (1-30): ", 1, 30);

    for (int i = 0; i < n; i++) {
        cout << fibonacci(i) << " ";
    }
    cout << endl;
}

void fibonacciCompare() {
    cout << "\nПорівняння рекурсії та ітерації:\n";

    for (int n = 35; n <= 40; n++) {
        fibCalls = 0;

        auto start = chrono::high_resolution_clock::now();
        long long r1 = fibonacci(n);
        auto end = chrono::high_resolution_clock::now();
        double timeRec = chrono::duration<double, milli>(end - start).count();

        start = chrono::high_resolution_clock::now();
        long long r2 = fibonacciIter(n);
        end = chrono::high_resolution_clock::now();
        double timeIter = chrono::duration<double, milli>(end - start).count();

        cout << "n = " << n << ", F(n) = " << r1
             << ", викликів: " << fibCalls << endl;
        cout << fixed << setprecision(6);
        cout << "   рекурсія: " << timeRec << " мс, ітерація: "
             << timeIter << " мс";
        if (r1 != r2) {
            cout << "  (результати не збігаються!)";
        }
        cout << endl;
    }
}

void fibonacciMenu() {
    int choice;
    do {
        cout << "\n--- Числа Фібоначчі ---\n";
        cout << "1. Число Фібоначчі за номером\n";
        cout << "2. Перші n чисел\n";
        cout << "3. Порівняння часу рекурсії та ітерації (n = 35-40)\n";
        cout << "0. Назад\n";

        choice = readInt("Ваш вибір: ", 0, 3);

        switch (choice) {
            case 1: fibonacciNumber(); break;
            case 2: fibonacciList(); break;
            case 3: fibonacciCompare(); break;
            case 0: break;
        }
    } while (choice != 0);
}

// Базовий випадок: n < 10, сума цифр дорівнює самому n
// Рекурсивний крок: остання цифра (n % 10) + сума цифр числа n / 10
int digitSum(long long n) {
    if (n < 10) return (int)n;
    return (int)(n % 10) + digitSum(n / 10);
}

void digitSumTask() {
    long long n = readInt("Введіть ціле число: ", -2000000000, 2000000000);

    if (n < 0) {
        n = -n;
    }

    cout << "Сума цифр: " << digitSum(n) << endl;
}

// Базовий випадок: n == 0, будь-яке число в степені 0 дорівнює 1
// Рекурсивний крок: x^n = x * x^(n - 1); для від'ємного n рахуємо 1 / x^(-n)
double power(double x, int n) {
    if (n == 0) return 1;
    if (n < 0) return 1 / power(x, -n);
    return x * power(x, n - 1);
}

void powerTask() {
    double x = readDouble("Введіть основу x (від -1000 до 1000): ", -1000, 1000);
    int n = readInt("Введіть степінь n (від -100 до 100): ", -100, 100);

    if (x == 0 && n < 0) {
        cout << "Помилка: ділення на нуль.\n";
        return;
    }

    cout << x << "^" << n << " = " << power(x, n) << endl;
}

// Базовий випадок: b == 0, НСД дорівнює a
// Рекурсивний крок: НСД(a, b) = НСД(b, a % b)
int findGcd(int a, int b) {
    if (b == 0) return a;
    return findGcd(b, a % b);
}

void gcdTask() {
    int a = readInt("Введіть перше число (1-2000000000): ", 1, 2000000000);
    int b = readInt("Введіть друге число (1-2000000000): ", 1, 2000000000);

    cout << "НСД(" << a << ", " << b << ") = " << findGcd(a, b) << endl;
}

// Базовий випадок: index == chars.size(), повертаємо порожній рядок
// Рекурсивний крок: розвертаємо решту символів і дописуємо в кінець поточний
string reverseChars(const vector<string>& chars, int index) {
    if (index == (int)chars.size()) return "";
    return reverseChars(chars, index + 1) + chars[index];
}

// Базовий випадок: left >= right (дійшли до середини) - це паліндром;
//                  якщо крайні символи різні - не паліндром
// Рекурсивний крок: порівняти крайні символи і перевірити частину між ними
bool isPalindromeRec(const vector<string>& chars, int left, int right) {
    if (left >= right) return true;
    if (chars[left] != chars[right]) return false;
    return isPalindromeRec(chars, left + 1, right - 1);
}

void reverseAndPalindrome() {
    string text = readLine("Введіть текст: ");

    if (text.empty()) {
        cout << "Текст не може бути порожнім.\n";
        return;
    }

    vector<string> chars = splitChars(text);

    if (chars.size() > 1000) {
        cout << "Занадто довгий текст (максимум 1000 символів).\n";
        return;
    }

    cout << "Текст навпаки: " << reverseChars(chars, 0) << endl;

    vector<string> clean;
    for (int i = 0; i < (int)chars.size(); i++) {
        if (chars[i] != " ") {
            clean.push_back(toLowerChar(chars[i]));
        }
    }

    if (clean.empty()) {
        cout << "У тексті немає символів для перевірки.\n";
    }
    else if (isPalindromeRec(clean, 0, (int)clean.size() - 1)) {
        cout << "Текст є паліндромом.\n";
    }
    else {
        cout << "Текст не є паліндромом.\n";
    }
}

// Базовий випадок: n < 2, двійкове число дорівнює n
// Рекурсивний крок: двійкове представлення n / 2, і в кінець дописуємо n % 2
string toBinary(int n) {
    if (n < 2) return to_string(n);
    return toBinary(n / 2) + to_string(n % 2);
}

void binaryTask() {
    int n = readInt("Введіть невід'ємне число: ", 0, 2000000000);

    cout << n << " у двійковій системі: " << toBinary(n) << endl;
}

// Базовий випадок: n == 1, переносимо один диск одразу
// Рекурсивний крок: переносимо n - 1 дисків на допоміжний стрижень,
//                   найбільший диск на цільовий, потім n - 1 дисків на цільовий
void hanoi(int n, char from, char to, char via) {
    if (n == 1) {
        hanoiMoves++;
        cout << hanoiMoves << ". Диск 1: " << from << " -> " << to << endl;
        return;
    }

    hanoi(n - 1, from, via, to);

    hanoiMoves++;
    cout << hanoiMoves << ". Диск " << n << ": " << from << " -> " << to << endl;

    hanoi(n - 1, via, to, from);
}

void hanoiTask() {
    int n = readInt("Кількість дисків (1-10): ", 1, 10);

    hanoiMoves = 0;
    hanoi(n, 'A', 'C', 'B');

    cout << "Загальна кількість ходів: " << hanoiMoves << endl;
    cout << "Перевірка за формулою 2^n - 1: " << (1 << n) - 1 << endl;
}

// Базовий випадок: у каталозі більше немає елементів (або перевищено глибину 10)
// Рекурсивний крок: для кожного підкаталогу викликаємо цю саму функцію
void printTree(const fs::path& folder, int level) {
    if (level > 10) {
        cout << string(level * 4, ' ') << "...\n";
        return;
    }

    error_code ec;
    for (fs::directory_entry entry : fs::directory_iterator(folder, ec)) {
        cout << string(level * 4, ' ') << entry.path().filename().u8string();

        if (entry.is_directory(ec) && !entry.is_symlink(ec)) {
            cout << "/" << endl;
            printTree(entry.path(), level + 1);
        } else {
            cout << endl;
        }
    }
}

void directoryTask() {
    string input = readLine("Введіть шлях до каталогу (Enter - поточний): ");

    if (input.empty()) {
        input = ".";
    }

    fs::path folder = input;
    error_code ec;

    if (!fs::exists(folder, ec) || !fs::is_directory(folder, ec)) {
        cout << "Каталог не знайдено.\n";
        return;
    }

    cout << folder.u8string() << "/" << endl;

    try {
        printTree(folder, 1);
    }
    catch (const fs::filesystem_error&) {
        cout << "Помилка доступу до каталогу.\n";
    }
}

void recursionMenu() {
    int choice;
    do {
        cout << "\n--- Рекурсивні функції ---\n";
        cout << "1. Факторіал\n";
        cout << "2. Числа Фібоначчі\n";
        cout << "3. Сума цифр числа\n";
        cout << "4. Піднесення до степеня\n";
        cout << "5. НСД (алгоритм Евкліда)\n";
        cout << "6. Розворот рядка та паліндром\n";
        cout << "7. Десяткове число у двійкове\n";
        cout << "8. Ханойські вежі\n";
        cout << "9. Дерево файлів каталогу\n";
        cout << "0. Назад\n";

        choice = readInt("Ваш вибір: ", 0, 9);

        switch (choice) {
            case 1: factorialTask(); break;
            case 2: fibonacciMenu(); break;
            case 3: digitSumTask(); break;
            case 4: powerTask(); break;
            case 5: gcdTask(); break;
            case 6: reverseAndPalindrome(); break;
            case 7: binaryTask(); break;
            case 8: hanoiTask(); break;
            case 9: directoryTask(); break;
            case 0: break;
            default: cout << "Невірний пункт\n";
        }
    } while (choice != 0);
}