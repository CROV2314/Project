#include "oop_task.h"
#include "menu_utils.h"
#include "domain.h"
#include "registry.h"
#include "errors.h"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

string choosePath(string title, string current);
bool prepareForWrite(string path);
bool checkExists(string path);

int parseInt(const string& text) {
    try {
        size_t pos = 0;
        int value = stoi(text, &pos);
        if (pos != text.size()) {
            throw invalid_argument(text);
        }
        return value;
    }
    catch (...) {
        throw DomainError("некоректне ціле число: " + text);
    }
}

double parseDouble(const string& text) {
    try {
        size_t pos = 0;
        double value = stod(text, &pos);
        if (pos != text.size()) {
            throw invalid_argument(text);
        }
        return value;
    }
    catch (...) {
        throw DomainError("некоректне число: " + text);
    }
}

void registerTypes(University& university) {
    university.registerType("Student", [](const vector<string>& f) {
        return make_unique<Student>(f.at(1), parseInt(f.at(2)), parseDouble(f.at(3)), parseInt(f.at(4)));
    });

    university.registerType("Lecturer", [](const vector<string>& f) {
        return make_unique<Lecturer>(f.at(1), parseInt(f.at(2)), parseDouble(f.at(3)), parseInt(f.at(4)));
    });

    university.registerType("Professor", [](const vector<string>& f) {
        return make_unique<Professor>(f.at(1), parseInt(f.at(2)), parseDouble(f.at(3)),
                                      parseInt(f.at(4)), parseInt(f.at(5)));
    });

    university.registerType("AdminStaff", [](const vector<string>& f) {
        return make_unique<AdminStaff>(f.at(1), parseInt(f.at(2)), parseDouble(f.at(3)), f.at(4));
    });
}

void printPeople(const vector<const Person*>& people) {
    if (people.empty()) {
        cout << "Нічого не знайдено.\n";
        return;
    }

    cout << "Знайдено: " << people.size() << endl;
    for (size_t i = 0; i < people.size(); i++) {
        cout << *people[i] << endl;
    }
}

void addPerson(University& university) {
    cout << "\nТип об'єкта:\n";
    cout << "1. Студент\n";
    cout << "2. Викладач\n";
    cout << "3. Професор\n";
    cout << "4. Адміністрація\n";
    cout << "0. Скасувати\n";

    int type = readInt("Ваш вибір: ", 0, 4);
    if (type == 0) {
        return;
    }

    string name = readLine("Прізвище та ім'я: ");
    int year = readInt("Рік народження: ", 0, 9999);

    try {
        if (type == 1) {
            double grade = readDouble("Середній бал: ", -1000, 1000);
            int course = readInt("Курс: ", -1000, 1000);
            university.add(make_unique<Student>(name, year, grade, course));
        }
        else if (type == 2) {
            double rate = readDouble("Ставка за годину: ", -1000000, 1000000);
            int hours = readInt("Годин на тиждень: ", -1000, 1000);
            university.add(make_unique<Lecturer>(name, year, rate, hours));
        }
        else if (type == 3) {
            double rate = readDouble("Ставка за годину: ", -1000000, 1000000);
            int hours = readInt("Годин на тиждень: ", -1000, 1000);
            int supervised = readInt("Кількість аспірантів: ", -1000, 1000);
            university.add(make_unique<Professor>(name, year, rate, hours, supervised));
        }
        else {
            double salary = readDouble("Місячна зарплата: ", -100000000, 100000000);
            string department = readLine("Відділ: ");
            university.add(make_unique<AdminStaff>(name, year, salary, department));
        }

        cout << "Об'єкт додано.\n";
    }
    catch (const DomainError& e) {
        cout << "Помилка: " << e.what() << endl;
    }
}

void removePerson(University& university) {
    int id = readInt("Введіть ID для видалення: ", 1, 999999);

    if (university.remove(id)) {
        cout << "Об'єкт видалено.\n";
    } else {
        cout << "Об'єкта з таким ID немає.\n";
    }
}

void duplicatePerson(University& university) {
    int id = readInt("Введіть ID для копіювання: ", 1, 999999);
    int newId = university.duplicate(id);

    if (newId == -1) {
        cout << "Об'єкта з таким ID немає.\n";
    } else {
        cout << "Створено копію з новим ID " << newId << ".\n";
    }
}

void searchMenu(University& university) {
    int choice;
    do {
        cout << "\n--- Пошук / фільтр ---\n";
        cout << "1. Пошук за прізвищем (часткове співпадіння)\n";
        cout << "2. Пошук за ID\n";
        cout << "3. Фільтр за мінімальним навантаженням\n";
        cout << "0. Назад\n";

        choice = readInt("Ваш вибір: ", 0, 3);

        if (choice == 1) {
            string text = readLine("Введіть прізвище або його частину: ");
            if (text.empty()) {
                cout << "Запит не може бути порожнім.\n";
            } else {
                printPeople(university.searchByName(text));
            }
        }
        else if (choice == 2) {
            int id = readInt("Введіть ID: ", 1, 999999);
            const Person* found = university.findById(id);
            if (found == nullptr) {
                cout << "Об'єкта з таким ID немає.\n";
            } else {
                cout << *found << endl;
            }
        }
        else if (choice == 3) {
            int hours = readInt("Мінімум годин на тиждень (1-100): ", 1, 100);
            printPeople(university.filterByMinHours(hours));
        }
    } while (choice != 0);
}

void sortMenu(University& university) {
    if (university.empty()) {
        cout << "Колекція порожня.\n";
        return;
    }

    cout << "\nСортувати за:\n";
    cout << "1. ПІБ (за алфавітом)\n";
    cout << "2. Виплатою (від більшої до меншої)\n";
    cout << "3. Навантаженням (від більшого до меншого)\n";
    cout << "0. Назад\n";

    int choice = readInt("Ваш вибір: ", 0, 3);
    if (choice == 0) {
        return;
    }

    university.sortBy(choice);
    cout << "Колекцію відсортовано.\n";
    university.showAll(cout);
}

void operationsMenu(University& university) {
    int choice;
    do {
        cout << "\n--- Операції університету ---\n";
        cout << "1. Показати за ролями\n";
        cout << "2. Фонд виплат за ролями\n";
        cout << "3. Найбільш оплачувана особа\n";
        cout << "4. Години іспитів\n";
        cout << "0. Назад\n";

        choice = readInt("Ваш вибір: ", 0, 4);

        if (choice == 1) {
            university.showByRoles(cout);
        }
        else if (choice == 2) {
            if (university.empty()) {
                cout << "Колекція порожня.\n";
            } else {
                map<string, double> payroll = university.payrollByRole();
                for (map<string, double>::iterator it = payroll.begin(); it != payroll.end(); ++it) {
                    cout << it->first << ": " << formatNumber(it->second, 2) << " грн\n";
                }
                cout << "Загальний фонд виплат: " << formatNumber(university.totalPayroll(), 2) << " грн\n";
                cout << "Загальне навантаження: " << formatNumber(university.totalWeeklyHours(), 0) << " год/тиждень\n";
            }
        }
        else if (choice == 3) {
            const Person* best = university.highestPaid();
            if (best == nullptr) {
                cout << "Колекція порожня.\n";
            } else {
                cout << "Найбільша виплата: " << *best << endl;
            }
        }
        else if (choice == 4) {
            int students = readInt("Кількість студентів на іспит (1-1000): ", 1, 1000);
            double total = university.examSchedule(students, cout);
            cout << "Усього годин іспитів: " << formatNumber(total, 2) << endl;
        }
    } while (choice != 0);
}

void saveToFile(University& university, string& path) {
    if (university.empty()) {
        cout << "Колекція порожня, зберігати нічого.\n";
        return;
    }

    path = choosePath("Файл даних", path);

    if (!prepareForWrite(path)) {
        return;
    }

    if (university.save(path)) {
        cout << "Збережено об'єктів: " << university.size() << " (файл " << path << ")\n";
    } else {
        cout << "Не вдалося відкрити файл для запису: " << path << endl;
    }
}

void loadFromFile(University& university, string& path) {
    path = choosePath("Файл даних", path);

    if (!checkExists(path)) {
        return;
    }

    int skipped = 0;
    int loaded = university.load(path, skipped);

    if (loaded == 0) {
        cout << "У файлі немає коректних об'єктів. Колекцію не змінено.\n";
    } else {
        cout << "Завантажено: " << loaded << ", пропущено: " << skipped << endl;
    }
}

void addSampleData(University& university) {
    university.add(make_unique<Student>("Андрієнко Іван", 2004, 92, 2));
    university.add(make_unique<Student>("Петренко Олена", 2003, 80, 3));
    university.add(make_unique<Student>("Сидоренко Марія", 2005, 60, 1));
    university.add(make_unique<Lecturer>("Коваленко Андрій", 1980, 250, 12));
    university.add(make_unique<Professor>("Мельник Ольга", 1970, 250, 12, 2));
    university.add(make_unique<AdminStaff>("Бондаренко Максим", 1985, 18000, "Бухгалтерія"));
    cout << "Додано 6 тестових об'єктів.\n";
}

void checkNumber(const string& name, double actual, double expected) {
    bool ok = fabs(actual - expected) < 0.001;

    cout << (ok ? "PASS: " : "FAIL: ") << name << " = " << formatNumber(actual, 2)
         << " (очікувалось " << formatNumber(expected, 2) << ")\n";
}

void checkTrue(const string& name, bool ok) {
    cout << (ok ? "PASS: " : "FAIL: ") << name << endl;
}

void selfTest() {
    cout << "\nСамотестування (контрольний приклад):\n";

    Student s1("Тест Один", 2000, 92, 1);
    Student s2("Тест Два", 2000, 80, 1);
    Student s3("Тест Три", 2000, 60, 1);
    Lecturer lecturer("Тест Викладач", 1980, 250, 12);
    Professor professor("Тест Професор", 1970, 250, 12, 2);

    checkNumber("Студент, середній бал 92: виплата", s1.monthlyPayment(), 2000);
    checkNumber("Студент, середній бал 80: виплата", s2.monthlyPayment(), 1200);
    checkNumber("Студент, середній бал 60: виплата", s3.monthlyPayment(), 0);
    checkNumber("Викладач (250, 12 год): виплата", lecturer.monthlyPayment(), 12000);
    checkNumber("Професор (250, 12 год, 2 аспіранти): виплата", professor.monthlyPayment(), 16000);
    checkNumber("examHours(20) викладача", lecturer.examHours(20), 10);
    checkNumber("examHours(20) професора", professor.examHours(20), 15);

    bool thrown = false;
    try {
        Student bad("Тест", 2000, 150, 1);
    }
    catch (const PersonError&) {
        thrown = true;
    }
    checkTrue("Середній бал 150 викликає виняток PersonError", thrown);

    unique_ptr<Person> copy = lecturer.clone();
    checkTrue("Копія дорівнює оригіналу, але має інший ID", *copy == lecturer && copy->id() != lecturer.id());

    University small;
    small.add(s1.clone());
    small.add(lecturer.clone());
    small.add(professor.clone());
    checkNumber("Загальний фонд виплат (2000 + 12000 + 16000)", small.totalPayroll(), 30000);
    checkTrue("Найбільш оплачувана особа - професор", small.highestPaid()->roleName() == "Професор");
}

void oopMenu() {
    static University university;
    static bool ready = false;
    static string dataPath = "data/oop_university.txt";

    if (!ready) {
        registerTypes(university);
        ready = true;
    }

    int choice;
    do {
        cout << "\n=== ДЕНЬ 5. ООП: Університет ===\n";
        cout << "1. Додати об'єкт (вибір типу)\n";
        cout << "2. Показати всі\n";
        cout << "3. Видалити за ID\n";
        cout << "4. Дублювати об'єкт (clone)\n";
        cout << "5. Пошук / фільтр\n";
        cout << "6. Сортування (3 критерії)\n";
        cout << "7. Операції університету\n";
        cout << "8. Зберегти у файл\n";
        cout << "9. Завантажити з файлу\n";
        cout << "10. Самотестування (контрольний приклад)\n";
        cout << "11. Додати тестові дані\n";
        cout << "0. Назад\n";

        choice = readInt("Ваш вибір: ", 0, 11);

        switch (choice) {
            case 1: addPerson(university); break;
            case 2: university.showAll(cout); break;
            case 3: removePerson(university); break;
            case 4: duplicatePerson(university); break;
            case 5: searchMenu(university); break;
            case 6: sortMenu(university); break;
            case 7: operationsMenu(university); break;
            case 8: saveToFile(university, dataPath); break;
            case 9: loadFromFile(university, dataPath); break;
            case 10: selfTest(); break;
            case 11: addSampleData(university); break;
            case 0: break;
            default: cout << "Невірний пункт\n";
        }
    } while (choice != 0);
}