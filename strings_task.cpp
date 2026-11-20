
#include "strings_task.h"
#include "menu_utils.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>
#include <cctype>

using namespace std;

// Розділення тексту на окремі символи UTF-8
vector<string> splitChars(string text) {
    vector<string> chars;

    for (int i = 0; i < text.length();) {
        int size = 1;

        unsigned char c = text[i];

        if ((c & 0xE0) == 0xC0) size = 2;
        else if ((c & 0xF0) == 0xE0) size = 3;
        else if ((c & 0xF8) == 0xF0) size = 4;

        chars.push_back(text.substr(i, size));
        i += size;
    }

    return chars;
}

// Переведення символу у нижній регістр
string toLowerChar(string c) {
    string upper[] = {
        "А","Б","В","Г","Ґ","Д","Е","Є","Ж","З",
        "И","І","Ї","Й","К","Л","М","Н","О","П",
        "Р","С","Т","У","Ф","Х","Ц","Ч","Ш","Щ",
        "Ь","Ю","Я"
    };

    string lower[] = {
        "а","б","в","г","ґ","д","е","є","ж","з",
        "и","і","ї","й","к","л","м","н","о","п",
        "р","с","т","у","ф","х","ц","ч","ш","щ",
        "ь","ю","я"
    };

    for (int i = 0; i < 33; i++) {
        if (c == upper[i]) return lower[i];
    }

    if (c.length() == 1) {
        c[0] = tolower((unsigned char)c[0]);
    }

    return c;
}

// Перевірка, чи є символ літерою
bool isLetter(string c) {
    string letters =
        "abcdefghijklmnopqrstuvwxyz";

    if (c.length() == 1) {
        return isalpha((unsigned char)c[0]);
    }

    string ukrainian[] = {
        "а","б","в","г","ґ","д","е","є","ж","з",
        "и","і","ї","й","к","л","м","н","о","п",
        "р","с","т","у","ф","х","ц","ч","ш","щ",
        "ь","ю","я"
    };

    c = toLowerChar(c);

    for (int i = 0; i < 33; i++) {
        if (c == ukrainian[i]) return true;
    }

    return false;
}

// Отримання тексту від користувача
void enterText(string& text) {
    text = readLine("Введіть текст: ");
}

// 1. Статистика тексту
void showStatistics(string text) {
    vector<string> chars = splitChars(text);

    int vowels = 0;
    int consonants = 0;
    int digits = 0;
    int punctuation = 0;
    int words = 0;

    string vowelList = "аеиіоуяюєї";

    for (int i = 0; i < chars.size(); i++) {
        string c = toLowerChar(chars[i]);

        if (isLetter(c)) {
            if (c.length() == 1 &&
                string("aeiouy").find(c) != string::npos) {
                vowels++;
            }
            else if (vowelList.find(c) != string::npos) {
                vowels++;
            }
            else {
                consonants++;
            }
        }
        else if (chars[i].length() == 1 &&
                 isdigit((unsigned char)chars[i][0])) {
            digits++;
        }
        else if (chars[i].length() == 1 &&
                 ispunct((unsigned char)chars[i][0])) {
            punctuation++;
        }
    }

    stringstream ss(text);
    string word;

    while (ss >> word) {
        words++;
    }

    cout << "\nКількість символів: " << chars.size();
    cout << "\nКількість слів: " << words;
    cout << "\nГолосних літер: " << vowels;
    cout << "\nПриголосних літер: " << consonants;
    cout << "\nЦифр: " << digits;
    cout << "\nРозділових знаків: " << punctuation << endl;
}

// 2. Найдовше та найкоротше слово
void findWords(string text) {
    stringstream ss(text);
    string word, longest, shortest;

    if (!(ss >> word)) {
        cout << "Текст порожній або не містить слів.\n";
        return;
    }

    longest = shortest = word;

    while (ss >> word) {
        if (splitChars(word).size() > splitChars(longest).size()) {
            longest = word;
        }

        if (splitChars(word).size() < splitChars(shortest).size()) {
            shortest = word;
        }
    }

    cout << "Найдовше слово: " << longest;
    cout << "\nНайкоротше слово: " << shortest << endl;
}

// 3. Перевертання тексту та слів
void reverseText(string text) {
    vector<string> chars = splitChars(text);

    reverse(chars.begin(), chars.end());

    cout << "Текст навпаки: ";

    for (int i = 0; i < chars.size(); i++) {
        cout << chars[i];
    }

    cout << endl;

    stringstream ss(text);
    string word;
    vector<string> words;

    while (ss >> word) {
        words.push_back(word);
    }

    reverse(words.begin(), words.end());

    cout << "Зворотний порядок слів: ";

    for (int i = 0; i < words.size(); i++) {
        cout << words[i] << " ";
    }

    cout << endl;
}

// 4. Перевірка паліндрому
void checkPalindrome(string text) {
    vector<string> chars = splitChars(text);
    vector<string> result;

    for (int i = 0; i < chars.size(); i++) {
        if (chars[i] != " ") {
            result.push_back(toLowerChar(chars[i]));
        }
    }

    vector<string> reversed = result;
    reverse(reversed.begin(), reversed.end());

    if (result == reversed) {
        cout << "Текст є паліндромом.\n";
    }
    else {
        cout << "Текст не є паліндромом.\n";
    }
}

// 5. Пошук підрядка
void searchSubstring(string text) {
    string findText = readLine("Введіть підрядок для пошуку: ");

    if (findText.empty()) {
        cout << "Підрядок не може бути порожнім.\n";
        return;
    }

    size_t pos = 0;
    int count = 0;

    while ((pos = text.find(findText, pos)) != string::npos) {
        vector<string> before = splitChars(text.substr(0, pos));

        cout << "Позиція: " << before.size() << endl;

        count++;
        pos++;
    }

    cout << "Кількість входжень: " << count << endl;
}

// 6. Заміна підрядка
void replaceSubstring(string& text) {
    string oldText = readLine("Що замінити: ");

    if (oldText.empty()) {
        cout << "Підрядок не може бути порожнім.\n";
        return;
    }

    string newText = readLine("На що замінити: ");

    size_t pos = 0;
    int count = 0;

    while ((pos = text.find(oldText, pos)) != string::npos) {
        text.replace(pos, oldText.length(), newText);

        pos += newText.length();
        count++;
    }

    cout << "Кількість замін: " << count << endl;
    cout << "Новий текст: " << text << endl;
}

// 7. Видалення зайвих пробілів
void removeExtraSpaces(string& text) {
    stringstream ss(text);
    string word;

    text = "";

    while (ss >> word) {
        if (!text.empty()) {
            text += " ";
        }

        text += word;
    }

    cout << "Результат: " << text << endl;
}

// 8. Шифр Цезаря
void caesarCipher(string& text) {
    int key = readInt("Введіть ключ (число): ", -1000, 1000);

    vector<string> alphabet = {
        "а","б","в","г","ґ","д","е","є","ж","з",
        "и","і","ї","й","к","л","м","н","о","п",
        "р","с","т","у","ф","х","ц","ч","ш","щ",
        "ь","ю","я"
    };

    vector<string> upper = {
        "А","Б","В","Г","Ґ","Д","Е","Є","Ж","З",
        "И","І","Ї","Й","К","Л","М","Н","О","П",
        "Р","С","Т","У","Ф","Х","Ц","Ч","Ш","Щ",
        "Ь","Ю","Я"
    };

    vector<string> chars = splitChars(text);

    for (int i = 0; i < chars.size(); i++) {
        bool isUpper = false;

        string c = chars[i];

        for (int j = 0; j < 33; j++) {
            if (c == upper[j]) {
                isUpper = true;
                c = alphabet[j];
                break;
            }
        }

        for (int j = 0; j < 33; j++) {
            if (c == alphabet[j]) {
                int newPos = (j + key) % 33;

                if (newPos < 0) {
                    newPos += 33;
                }

                chars[i] = isUpper ? upper[newPos] : alphabet[newPos];
                break;
            }
        }
    }

    text = "";

    for (int i = 0; i < chars.size(); i++) {
        text += chars[i];
    }

    cout << "Результат: " << text << endl;
}

// 9. Частота символів
void characterFrequency(string text) {
    vector<string> chars = splitChars(text);
    map<string, int> frequency;

    for (int i = 0; i < chars.size(); i++) {
        frequency[chars[i]]++;
    }

    cout << "Частота символів:\n";

    for (map<string, int>::iterator it = frequency.begin();
         it != frequency.end(); ++it) {
        cout << it->first << " : " << it->second << endl;
    }
}

// Меню першого дня
void stringsMenu() {
    string text = "";

    while (true) {
        cout << "\n========== ТЕКСТОВИЙ ПРОЦЕСОР ==========\n";
        cout << "1. Введення тексту\n";
        cout << "2. Статистика тексту\n";
        cout << "3. Найдовше та найкоротше слово\n";
        cout << "4. Перевертання тексту та слів\n";
        cout << "5. Перевірка паліндрому\n";
        cout << "6. Пошук підрядка\n";
        cout << "7. Заміна підрядка\n";
        cout << "8. Видалення зайвих пробілів\n";
        cout << "9. Шифр Цезаря\n";
        cout << "10. Частота символів\n";
        cout << "0. Назад\n";

        int choice = readInt("Ваш вибір: ", 0, 10);

        if (choice == 0) {
            break;
        }

        if (choice != 1 && text.empty()) {
            cout << "Спочатку введіть текст!\n";
            continue;
        }

        switch (choice) {
            case 1:
                enterText(text);
                break;

            case 2:
                showStatistics(text);
                break;

            case 3:
                findWords(text);
                break;

            case 4:
                reverseText(text);
                break;

            case 5:
                checkPalindrome(text);
                break;

            case 6:
                searchSubstring(text);
                break;

            case 7:
                replaceSubstring(text);
                break;

            case 8:
                removeExtraSpaces(text);
                break;

            case 9:
                caesarCipher(text);
                break;

            case 10:
                characterFrequency(text);
                break;
        }
    }
}