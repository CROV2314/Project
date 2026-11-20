#include "types_task.h"
#include "menu_utils.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace std;

StudentList students;

string toString(Faculty f) {
    switch (f) {
        case Faculty::IT:        return "IT";
        case Faculty::Economics: return "Економіка";
        case Faculty::Law:       return "Право";
        default:                 return "Інше";
    }
}

int textLength(string text) {
    int count = 0;
    for (size_t i = 0; i < text.size(); i++) {
        unsigned char c = text[i];
        if ((c & 0xC0) != 0x80) {
            count++;
        }
    }
    return count;
}

void printPadded(string text, int width) {
    cout << text;
    for (int i = textLength(text); i < width; i++) {
        cout << ' ';
    }
}

string toLowerText(string text) {
    string result = "";
    for (size_t i = 0; i < text.size(); i++) {
        unsigned char c = text[i];

        if (c < 128) {
            result += (char)tolower(c);
        }
        else if (c == 0xD0 && i + 1 < text.size()) {
            unsigned char d = text[i + 1];
            if (d >= 0x90 && d <= 0x9F) {
                result += (char)0xD0;
                result += (char)(d + 0x20);
            }
            else if (d >= 0xA0 && d <= 0xAF) {
                result += (char)0xD1;
                result += (char)(d - 0x20);
            }
            else if (d == 0x84) { result += (char)0xD1; result += (char)0x94; }
            else if (d == 0x86) { result += (char)0xD1; result += (char)0x96; }
            else if (d == 0x87) { result += (char)0xD1; result += (char)0x97; }
            else {
                result += (char)c;
                result += (char)d;
            }
            i++;
        }
        else if (c == 0xD2 && i + 1 < text.size() && (unsigned char)text[i + 1] == 0x90) {
            result += (char)0xD2;
            result += (char)0x91;
            i++;
        }
        else {
            result += (char)c;
        }
    }
    return result;
}

bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int daysInMonth(int month, int year) {
    if (month == 2) {
        return isLeapYear(year) ? 29 : 28;
    }
    if (month == 4 || month == 6 || month == 9 || month == 11) {
        return 30;
    }
    return 31;
}

bool isValidDate(Date d) {
    if (d.year < 1900 || d.year > 2025) return false;
    if (d.month < 1 || d.month > 12) return false;
    return d.day >= 1 && d.day <= daysInMonth(d.month, d.year);
}

string dateToString(Date d) {
    ostringstream out;
    out << setfill('0') << setw(2) << d.day << "."
        << setw(2) << d.month << "." << d.year;
    return out.str();
}

int findIndexById(int id) {
    for (size_t i = 0; i < students.size(); i++) {
        if (students[i].id == id) {
            return (int)i;
        }
    }
    return -1;
}

string inputName() {
    while (true) {
        string name = readLine("Прізвище та ім'я: ");
        if (textLength(name) <= 25) {
            return name;
        }
        cout << "Занадто довго (максимум 25 символів).\n";
    }
}

Date inputDate() {
    Date d;
    while (true) {
        d.day = readInt("День: ", 1, 31);
        d.month = readInt("Місяць: ", 1, 12);
        d.year = readInt("Рік (1900-2025): ", 1900, 2025);

        if (isValidDate(d)) {
            return d;
        }
        cout << "Такої дати не існує. Спробуйте ще раз.\n";
    }
}

Faculty inputFaculty() {
    cout << "Факультет:\n";
    cout << "1. IT\n";
    cout << "2. Економіка\n";
    cout << "3. Право\n";
    cout << "4. Інше\n";

    int choice = readInt("Ваш вибір: ", 1, 4);

    switch (choice) {
        case 1:  return Faculty::IT;
        case 2:  return Faculty::Economics;
        case 3:  return Faculty::Law;
        default: return Faculty::Other;
    }
}

void printTable(const StudentList& list) {
    cout << left;
    cout << string(76, '-') << endl;
    cout << setw(8) << "ID";
    printPadded("Прізвище та ім'я", 28);
    printPadded("Дата народж.", 18);
    printPadded("Факультет", 14);
    cout << "Сер. бал" << endl;
    cout << string(76, '-') << endl;

    for (size_t i = 0; i < list.size(); i++) {
        cout << left << setw(8) << list[i].id;
        printPadded(list[i].name, 28);
        cout << setw(18) << dateToString(list[i].birthDate);
        printPadded(toString(list[i].faculty), 14);
        cout << fixed << setprecision(2) << list[i].avgGrade << endl;
    }

    cout << string(76, '-') << endl;
}

void addStudent() {
    Student s;

    while (true) {
        s.id = readInt("Введіть id (1-999999): ", 1, 999999);
        if (findIndexById(s.id) == -1) {
            break;
        }
        cout << "Студент з таким id уже є.\n";
    }

    s.name = inputName();
    cout << "Дата народження:\n";
    s.birthDate = inputDate();
    s.faculty = inputFaculty();
    s.avgGrade = readDouble("Середній бал (0-100): ", 0, 100);

    students.push_back(s);
    cout << "Студента додано.\n";
}

void showStudents() {
    if (students.empty()) {
        cout << "Список порожній.\n";
        return;
    }
    printTable(students);
}

void searchStudents() {
    if (students.empty()) {
        cout << "Список порожній.\n";
        return;
    }

    string query = toLowerText(readLine("Введіть прізвище або його частину: "));
    StudentList found;

    for (size_t i = 0; i < students.size(); i++) {
        if (toLowerText(students[i].name).find(query) != string::npos) {
            found.push_back(students[i]);
        }
    }

    if (found.empty()) {
        cout << "Нічого не знайдено.\n";
    } else {
        cout << "Знайдено: " << found.size() << endl;
        printTable(found);
    }
}

void sortStudents() {
    if (students.empty()) {
        cout << "Список порожній.\n";
        return;
    }

    cout << "Сортувати за:\n";
    cout << "1. Прізвищем\n";
    cout << "2. Середнім балом (від більшого до меншого)\n";
    cout << "3. Датою народження (від старших до молодших)\n";
    cout << "0. Назад\n";

    int choice = readInt("Ваш вибір: ", 0, 3);

    switch (choice) {
        case 1:
            sort(students.begin(), students.end(),
                 [](const Student& a, const Student& b) {
                     return a.name < b.name;
                 });
            break;

        case 2:
            sort(students.begin(), students.end(),
                 [](const Student& a, const Student& b) {
                     return a.avgGrade > b.avgGrade;
                 });
            break;

        case 3:
            sort(students.begin(), students.end(),
                 [](const Student& a, const Student& b) {
                     if (a.birthDate.year != b.birthDate.year)
                         return a.birthDate.year < b.birthDate.year;
                     if (a.birthDate.month != b.birthDate.month)
                         return a.birthDate.month < b.birthDate.month;
                     return a.birthDate.day < b.birthDate.day;
                 });
            break;

        case 0:
            return;
    }

    cout << "Список відсортовано.\n";
    printTable(students);
}

void deleteStudent() {
    if (students.empty()) {
        cout << "Список порожній.\n";
        return;
    }

    int id = readInt("Введіть id студента для видалення: ", 1, 999999);
    int index = findIndexById(id);

    if (index == -1) {
        cout << "Студента з таким id не знайдено.\n";
        return;
    }

    students.erase(students.begin() + index);
    cout << "Студента видалено.\n";
}

void showFacultyStatistics() {
    if (students.empty()) {
        cout << "Список порожній.\n";
        return;
    }

    Faculty all[4] = { Faculty::IT, Faculty::Economics, Faculty::Law, Faculty::Other };

    cout << "\nСтатистика по факультетах:\n";
    for (int f = 0; f < 4; f++) {
        int count = 0;
        double sum = 0;

        for (size_t i = 0; i < students.size(); i++) {
            if (students[i].faculty == all[f]) {
                count++;
                sum += students[i].avgGrade;
            }
        }

        printPadded(toString(all[f]), 12);
        cout << "студентів: " << count;
        if (count > 0) {
            cout << ", середній бал: " << fixed << setprecision(2) << sum / count;
        } else {
            cout << ", середній бал: -";
        }
        cout << endl;
    }
}

void editStudent() {
    if (students.empty()) {
        cout << "Список порожній.\n";
        return;
    }

    int id = readInt("Введіть id студента для редагування: ", 1, 999999);
    int index = findIndexById(id);

    if (index == -1) {
        cout << "Студента з таким id не знайдено.\n";
        return;
    }

    Student& s = students[index];
    int choice;

    do {
        cout << "\nЩо змінити?\n";
        cout << "1. Прізвище та ім'я\n";
        cout << "2. Дату народження\n";
        cout << "3. Факультет\n";
        cout << "4. Середній бал\n";
        cout << "0. Готово\n";

        choice = readInt("Ваш вибір: ", 0, 4);

        switch (choice) {
            case 1: s.name = inputName(); break;
            case 2: s.birthDate = inputDate(); break;
            case 3: s.faculty = inputFaculty(); break;
            case 4: s.avgGrade = readDouble("Середній бал (0-100): ", 0, 100); break;
            case 0: break;
        }
    } while (choice != 0);

    cout << "Запис оновлено.\n";
}

void typesMenu() {
    int choice;
    do {
        cout << "\n--- Облік студентів ---\n";
        cout << "1. Додати студента\n";
        cout << "2. Показати всіх студентів\n";
        cout << "3. Пошук за прізвищем\n";
        cout << "4. Сортування\n";
        cout << "5. Видалити студента за id\n";
        cout << "6. Статистика по факультетах\n";
        cout << "7. Редагувати запис\n";
        cout << "0. Назад\n";

        choice = readInt("Ваш вибір: ", 0, 7);

        switch (choice) {
            case 1: addStudent(); break;
            case 2: showStudents(); break;
            case 3: searchStudents(); break;
            case 4: sortStudents(); break;
            case 5: deleteStudent(); break;
            case 6: showFacultyStatistics(); break;
            case 7: editStudent(); break;
            case 0: break;
            default: cout << "Невірний пункт\n";
        }
    } while (choice != 0);
}