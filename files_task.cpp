#include "files_task.h"
#include "menu_utils.h"
#include "types_task.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

string dateToString(Date d);
bool isValidDate(Date d);
string inputName();
Date inputDate();
Faculty inputFaculty();
string toLowerText(string text);
int textLength(string text);
void printTable(const StudentList& list);

struct StudentRec {
    int id;
    char name[100];
    int day, month, year;
    int faculty;
    double avgGrade;
};

string textPath = "data/students.txt";
string binPath = "data/students.bin";
bool pathsChosen = false;

string choosePath(string title, string current) {
    cout << title << ", за замовчуванням: " << current << endl;
    string input = readLine("Введіть шлях або натисніть Enter: ");

    if (input.size() >= 2 && input.front() == '"' && input.back() == '"') {
        input = input.substr(1, input.size() - 2);
    }

    if (input.empty()) {
        return current;
    }
    return input;
}

bool prepareForWrite(string path) {
    fs::path folder = fs::path(path).parent_path();

    if (folder.empty()) {
        return true;
    }

    try {
        fs::create_directories(folder);
    }
    catch (...) {
        cout << "Не вдалося створити каталог: " << folder.string() << endl;
        return false;
    }
    return true;
}

bool checkExists(string path) {
    if (!fs::exists(path)) {
        cout << "Файл не знайдено: " << path << endl;
        return false;
    }
    return true;
}

long long getFileSize(string path) {
    ifstream in(path, ios::binary);

    if (!in.is_open()) {
        return -1;
    }

    in.seekg(0, ios::end);
    long long size = in.tellg();
    in.close();

    return size;
}

bool parseFaculty(string text, Faculty& faculty) {
    Faculty all[4] = { Faculty::IT, Faculty::Economics, Faculty::Law, Faculty::Other };

    for (int i = 0; i < 4; i++) {
        if (toString(all[i]) == text) {
            faculty = all[i];
            return true;
        }
    }
    return false;
}

string studentToLine(const Student& s) {
    string name = s.name;

    for (size_t i = 0; i < name.size(); i++) {
        if (name[i] == ';') {
            name[i] = ',';
        }
    }

    ostringstream out;
    out << s.id << ";" << name << ";" << dateToString(s.birthDate) << ";"
        << toString(s.faculty) << ";" << fixed << setprecision(2) << s.avgGrade;
    return out.str();
}

bool parseLine(string line, Student& s, string& error) {
    if (count(line.begin(), line.end(), ';') != 4) {
        error = "має бути 5 полів, розділених символом ;";
        return false;
    }

    stringstream ss(line);
    string idText, name, dateText, facultyText, gradeText;

    getline(ss, idText, ';');
    getline(ss, name, ';');
    getline(ss, dateText, ';');
    getline(ss, facultyText, ';');
    getline(ss, gradeText, ';');

    try {
        s.id = stoi(idText);
        s.avgGrade = stod(gradeText);
    }
    catch (...) {
        error = "id або середній бал не є числом";
        return false;
    }

    if (s.id < 1 || s.id > 999999) {
        error = "некоректний id";
        return false;
    }

    if (name.empty() || name.size() >= 100) {
        error = "некоректне ім'я";
        return false;
    }
    s.name = name;

    int day, month, year;
    if (sscanf(dateText.c_str(), "%d.%d.%d", &day, &month, &year) != 3) {
        error = "некоректна дата";
        return false;
    }
    s.birthDate.day = day;
    s.birthDate.month = month;
    s.birthDate.year = year;

    if (!isValidDate(s.birthDate)) {
        error = "такої дати не існує";
        return false;
    }

    if (!parseFaculty(facultyText, s.faculty)) {
        error = "невідомий факультет";
        return false;
    }

    if (s.avgGrade < 0 || s.avgGrade > 100) {
        error = "середній бал поза діапазоном 0-100";
        return false;
    }

    return true;
}

bool saveStudentsText(string path) {
    if (students.empty()) {
        cout << "Список студентів порожній, зберігати нічого.\n";
        return false;
    }

    if (!prepareForWrite(path)) {
        return false;
    }

    ofstream out(path);
    if (!out.is_open()) {
        cout << "Не вдалося відкрити файл для запису: " << path << endl;
        return false;
    }

    for (size_t i = 0; i < students.size(); i++) {
        out << studentToLine(students[i]) << "\n";
    }

    out.close();
    cout << "Збережено студентів: " << students.size() << " (файл " << path << ")\n";
    return true;
}

bool loadStudentsText(string path) {
    if (!checkExists(path)) {
        return false;
    }

    ifstream in(path);
    if (!in.is_open()) {
        cout << "Не вдалося відкрити файл для читання: " << path << endl;
        return false;
    }

    StudentList loaded;
    string line;
    int lineNumber = 0;
    int skipped = 0;

    while (getline(in, line)) {
        lineNumber++;

        if (line.empty()) {
            continue;
        }

        Student s;
        string error;

        if (!parseLine(line, s, error)) {
            cout << "Рядок " << lineNumber << " пропущено: " << error << endl;
            skipped++;
            continue;
        }

        bool duplicate = false;
        for (size_t i = 0; i < loaded.size(); i++) {
            if (loaded[i].id == s.id) {
                duplicate = true;
            }
        }
        if (duplicate) {
            cout << "Рядок " << lineNumber << " пропущено: id " << s.id << " уже є\n";
            skipped++;
            continue;
        }

        loaded.push_back(s);
    }

    in.close();

    if (loaded.empty()) {
        cout << "У файлі немає коректних записів. Поточний список не змінено.\n";
        return false;
    }

    students = loaded;
    cout << "Завантажено: " << loaded.size() << ", пропущено: " << skipped << endl;
    return true;
}

bool idInTextFile(string path, int id) {
    ifstream in(path);

    if (!in.is_open()) {
        return false;
    }

    string line;
    while (getline(in, line)) {
        if (line.substr(0, line.find(';')) == to_string(id)) {
            return true;
        }
    }
    return false;
}

void appendStudentText(string path) {
    Student s;

    while (true) {
        s.id = readInt("Введіть id (1-999999): ", 1, 999999);
        if (idInTextFile(path, s.id)) {
            cout << "Такий id уже є у файлі.\n";
        } else {
            break;
        }
    }

    s.name = inputName();
    cout << "Дата народження:\n";
    s.birthDate = inputDate();
    s.faculty = inputFaculty();
    s.avgGrade = readDouble("Середній бал (0-100): ", 0, 100);

    if (!prepareForWrite(path)) {
        return;
    }

    ofstream out(path, ios::app);
    if (!out.is_open()) {
        cout << "Не вдалося відкрити файл для запису: " << path << endl;
        return;
    }

    out << studentToLine(s) << "\n";
    out.close();

    cout << "Запис додано в кінець файлу.\n";
    cout << "Щоб побачити його в списку студентів, завантажте файл (пункт 2).\n";
}

void textStatistics(string path) {
    if (!checkExists(path)) {
        return;
    }

    ifstream in(path);
    if (!in.is_open()) {
        cout << "Не вдалося відкрити файл для читання: " << path << endl;
        return;
    }

    int lines = 0, words = 0, chars = 0;
    string line;

    while (getline(in, line)) {
        lines++;
        chars += textLength(line);

        stringstream lineStream(line);
        string word;
        while (lineStream >> word) {
            words++;
        }
    }

    in.close();

    cout << "Кількість рядків: " << lines << endl;
    cout << "Кількість слів: " << words << endl;
    cout << "Кількість символів (без переводів рядка): " << chars << endl;
}

void searchWordInFile(string path) {
    if (!checkExists(path)) {
        return;
    }

    string word = readLine("Введіть слово для пошуку: ");
    if (word.empty()) {
        cout << "Слово не може бути порожнім.\n";
        return;
    }
    word = toLowerText(word);

    ifstream in(path);
    if (!in.is_open()) {
        cout << "Не вдалося відкрити файл для читання: " << path << endl;
        return;
    }

    string line;
    int lineNumber = 0;
    int found = 0;

    while (getline(in, line)) {
        lineNumber++;

        if (toLowerText(line).find(word) != string::npos) {
            cout << "Рядок " << lineNumber << ": " << line << endl;
            found++;
        }
    }

    in.close();

    if (found == 0) {
        cout << "Слово не знайдено.\n";
    } else {
        cout << "Знайдено рядків: " << found << endl;
    }
}

StudentRec toRec(const Student& s) {
    StudentRec rec = {};

    rec.id = s.id;
    strncpy(rec.name, s.name.c_str(), sizeof(rec.name) - 1);
    rec.day = s.birthDate.day;
    rec.month = s.birthDate.month;
    rec.year = s.birthDate.year;
    rec.faculty = (int)s.faculty;
    rec.avgGrade = s.avgGrade;

    return rec;
}

Student toStudent(StudentRec rec) {
    rec.name[sizeof(rec.name) - 1] = '\0';

    Student s;
    s.id = rec.id;
    s.name = rec.name;
    s.birthDate.day = rec.day;
    s.birthDate.month = rec.month;
    s.birthDate.year = rec.year;
    s.faculty = (Faculty)rec.faculty;
    s.avgGrade = rec.avgGrade;

    return s;
}

int countRecords(string path) {
    long long size = getFileSize(path);

    if (size < 0) {
        cout << "Не вдалося відкрити файл: " << path << endl;
        return -1;
    }

    if (size % sizeof(StudentRec) != 0) {
        cout << "Увага: розмір файлу не кратний розміру запису, файл може бути пошкоджений.\n";
    }

    return size / sizeof(StudentRec);
}

bool saveStudentsBinary(string path) {
    if (students.empty()) {
        cout << "Список студентів порожній, зберігати нічого.\n";
        return false;
    }

    if (!prepareForWrite(path)) {
        return false;
    }

    ofstream out(path, ios::binary);
    if (!out.is_open()) {
        cout << "Не вдалося відкрити файл для запису: " << path << endl;
        return false;
    }

    for (size_t i = 0; i < students.size(); i++) {
        StudentRec rec = toRec(students[i]);
        out.write(reinterpret_cast<const char*>(&rec), sizeof(rec));
    }

    out.close();
    cout << "Збережено записів: " << students.size() << " (файл " << path << ")\n";
    return true;
}

void showBinary(string path) {
    if (!checkExists(path)) {
        return;
    }

    ifstream in(path, ios::binary);
    if (!in.is_open()) {
        cout << "Не вдалося відкрити файл для читання: " << path << endl;
        return;
    }

    StudentList list;
    StudentRec rec;

    while (in.read(reinterpret_cast<char*>(&rec), sizeof(rec))) {
        list.push_back(toStudent(rec));
    }

    in.close();

    if (list.empty()) {
        cout << "У файлі немає записів.\n";
        return;
    }

    cout << "Записів у файлі: " << list.size() << endl;
    printTable(list);
}

void readRecordByNumber(string path) {
    if (!checkExists(path)) {
        return;
    }

    int total = countRecords(path);
    if (total <= 0) {
        cout << "У файлі немає записів.\n";
        return;
    }

    int number = readInt("Номер запису (1-" + to_string(total) + "): ", 1, total);

    ifstream in(path, ios::binary);
    if (!in.is_open()) {
        cout << "Не вдалося відкрити файл для читання: " << path << endl;
        return;
    }

    StudentRec rec;
    long long position = (number - 1) * sizeof(StudentRec);

    in.seekg(position);
    in.read(reinterpret_cast<char*>(&rec), sizeof(rec));

    if (!in) {
        cout << "Не вдалося прочитати запис.\n";
        return;
    }

    in.close();

    StudentList one;
    one.push_back(toStudent(rec));
    printTable(one);
}

void editRecordInPlace(string path) {
    if (!checkExists(path)) {
        return;
    }

    int total = countRecords(path);
    if (total <= 0) {
        cout << "У файлі немає записів.\n";
        return;
    }

    int number = readInt("Номер запису (1-" + to_string(total) + "): ", 1, total);

    fstream file(path, ios::in | ios::out | ios::binary);
    if (!file.is_open()) {
        cout << "Не вдалося відкрити файл: " << path << endl;
        return;
    }

    StudentRec rec;
    long long position = (number - 1) * sizeof(StudentRec);

    file.seekg(position);
    file.read(reinterpret_cast<char*>(&rec), sizeof(rec));

    if (!file) {
        cout << "Не вдалося прочитати запис.\n";
        return;
    }

    Student s = toStudent(rec);

    StudentList one;
    one.push_back(s);
    printTable(one);

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

    rec = toRec(s);

    file.seekp(position);
    file.write(reinterpret_cast<const char*>(&rec), sizeof(rec));
    file.close();

    cout << "Запис " << number << " змінено в файлі.\n";
}

void compareSizes(string textFile, string binFile) {
    long long textSize = getFileSize(textFile);
    long long binSize = getFileSize(binFile);

    if (textSize < 0 || binSize < 0) {
        cout << "Спочатку збережіть список в обидва файли (пункти 1 і 6).\n";
        return;
    }

    cout << "Текстовий файл: " << textSize << " байт\n";
    cout << "Бінарний файл:  " << binSize << " байт";
    cout << " (записів: " << binSize / sizeof(StudentRec)
         << ", розмір запису: " << sizeof(StudentRec) << " байт)\n";

    if (textSize > 0) {
        cout << fixed << setprecision(2);
        cout << "Бінарний файл відносно текстового: "
             << (double)binSize / textSize << " раза\n";
    }
}

void filesMenu() {
    if (!pathsChosen) {
        cout << "\nВибір робочих файлів\n";
        textPath = choosePath("Текстовий файл", textPath);
        binPath = choosePath("Бінарний файл", binPath);
        pathsChosen = true;
    }

    int choice;
    do {
        cout << "\n--- Файли ---\n";
        cout << "Текстовий файл: " << textPath << endl;
        cout << "Бінарний файл:  " << binPath << endl;
        cout << "\nА. Текстові файли\n";
        cout << "1. Зберегти список студентів у текстовий файл\n";
        cout << "2. Завантажити список студентів з текстового файлу\n";
        cout << "3. Додати запис у кінець файлу\n";
        cout << "4. Статистика текстового файлу\n";
        cout << "5. Пошук слова у файлі\n";
        cout << "\nБ. Бінарні файли\n";
        cout << "6. Зберегти список студентів у бінарний файл\n";
        cout << "7. Прочитати бінарний файл і вивести вміст\n";
        cout << "8. Прочитати запис за номером\n";
        cout << "9. Змінити запис за номером\n";
        cout << "10. Порівняти розміри текстового та бінарного файлів\n";
        cout << "\n11. Змінити текстовий файл\n";
        cout << "12. Змінити бінарний файл\n";
        cout << "0. Назад\n";

        choice = readInt("Ваш вибір: ", 0, 12);

        switch (choice) {
            case 1: saveStudentsText(textPath); break;
            case 2: loadStudentsText(textPath); break;
            case 3: appendStudentText(textPath); break;
            case 4: textStatistics(textPath); break;
            case 5: searchWordInFile(textPath); break;
            case 6: saveStudentsBinary(binPath); break;
            case 7: showBinary(binPath); break;
            case 8: readRecordByNumber(binPath); break;
            case 9: editRecordInPlace(binPath); break;
            case 10: compareSizes(textPath, binPath); break;
            case 11: textPath = choosePath("Текстовий файл", textPath); break;
            case 12: binPath = choosePath("Бінарний файл", binPath); break;
            case 0: break;
            default: cout << "Невірний пункт\n";
        }
    } while (choice != 0);
}