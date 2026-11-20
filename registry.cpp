#include "registry.h"

#include <algorithm>
#include <fstream>
#include <iterator>
#include <sstream>
#include <stdexcept>

using namespace std;

string toLowerText(string text);

static vector<string> splitLine(const string& line) {
    vector<string> parts;
    stringstream ss(line);
    string part;

    while (getline(ss, part, ';')) {
        parts.push_back(part);
    }
    return parts;
}

void University::registerType(const string& tag, Creator creator) {
    creators_[tag] = creator;
}

void University::add(unique_ptr<Person> person) {
    people_.push_back(move(person));
}

bool University::remove(int id) {
    for (size_t i = 0; i < people_.size(); i++) {
        if (people_[i]->id() == id) {
            people_.erase(people_.begin() + i);
            return true;
        }
    }
    return false;
}

int University::duplicate(int id) {
    const Person* found = findById(id);

    if (found == nullptr) {
        return -1;
    }

    unique_ptr<Person> copy = found->clone();
    int newId = copy->id();
    people_.push_back(move(copy));
    return newId;
}

bool University::empty() const {
    return people_.empty();
}

int University::size() const {
    return (int)people_.size();
}

void University::showAll(ostream& os) const {
    if (people_.empty()) {
        os << "Колекція порожня.\n";
        return;
    }

    for (size_t i = 0; i < people_.size(); i++) {
        os << *people_[i] << "\n";
    }
}

void University::showByRoles(ostream& os) const {
    if (people_.empty()) {
        os << "Колекція порожня.\n";
        return;
    }

    map<string, vector<const Person*>> groups;
    for (size_t i = 0; i < people_.size(); i++) {
        groups[people_[i]->roleName()].push_back(people_[i].get());
    }

    for (map<string, vector<const Person*>>::iterator it = groups.begin(); it != groups.end(); ++it) {
        os << "\n" << it->first << " (" << it->second.size() << "):\n";
        for (size_t i = 0; i < it->second.size(); i++) {
            os << "  " << *it->second[i] << "\n";
        }
    }
}

const Person* University::findById(int id) const {
    vector<unique_ptr<Person>>::const_iterator it = find_if(
        people_.begin(), people_.end(),
        [id](const unique_ptr<Person>& p) { return p->id() == id; });

    if (it == people_.end()) {
        return nullptr;
    }
    return it->get();
}

vector<const Person*> University::searchByName(const string& text) const {
    vector<const Person*> all;
    for (size_t i = 0; i < people_.size(); i++) {
        all.push_back(people_[i].get());
    }

    string query = toLowerText(text);
    vector<const Person*> result;
    copy_if(all.begin(), all.end(), back_inserter(result),
            [query](const Person* p) {
                return toLowerText(p->fullName()).find(query) != string::npos;
            });
    return result;
}

vector<const Person*> University::filterByMinHours(int hours) const {
    vector<const Person*> all;
    for (size_t i = 0; i < people_.size(); i++) {
        all.push_back(people_[i].get());
    }

    vector<const Person*> result;
    copy_if(all.begin(), all.end(), back_inserter(result),
            [hours](const Person* p) { return p->weeklyHours() >= hours; });
    return result;
}

void University::sortBy(int criterion) {
    if (criterion == 1) {
        sort(people_.begin(), people_.end(),
             [](const unique_ptr<Person>& a, const unique_ptr<Person>& b) {
                 return *a < *b;
             });
    }
    else if (criterion == 2) {
        sort(people_.begin(), people_.end(),
             [](const unique_ptr<Person>& a, const unique_ptr<Person>& b) {
                 return a->monthlyPayment() > b->monthlyPayment();
             });
    }
    else if (criterion == 3) {
        sort(people_.begin(), people_.end(),
             [](const unique_ptr<Person>& a, const unique_ptr<Person>& b) {
                 return a->weeklyHours() > b->weeklyHours();
             });
    }
}

map<string, double> University::payrollByRole() const {
    map<string, double> result;
    for (size_t i = 0; i < people_.size(); i++) {
        result[people_[i]->roleName()] += people_[i]->monthlyPayment();
    }
    return result;
}

double University::totalPayroll() const {
    return sumBy(people_, [](const unique_ptr<Person>& p) { return p->monthlyPayment(); });
}

double University::totalWeeklyHours() const {
    return sumBy(people_, [](const unique_ptr<Person>& p) { return (double)p->weeklyHours(); });
}

const Person* University::highestPaid() const {
    int index = indexOfMax(people_, [](const unique_ptr<Person>& p) { return p->monthlyPayment(); });

    if (index == -1) {
        return nullptr;
    }
    return people_[index].get();
}

double University::examSchedule(int studentsCount, ostream& os) const {
    double total = 0;

    for (size_t i = 0; i < people_.size(); i++) {
        const IExaminer* examiner = dynamic_cast<const IExaminer*>(people_[i].get());

        if (examiner != nullptr) {
            double hours = examiner->examHours(studentsCount);
            os << people_[i]->fullName() << " (" << people_[i]->roleName() << "): "
               << formatNumber(hours, 2) << " год\n";
            total += hours;
        }
    }
    return total;
}

bool University::save(const string& path) const {
    ofstream out(path);

    if (!out.is_open()) {
        return false;
    }

    for (size_t i = 0; i < people_.size(); i++) {
        out << people_[i]->toLine() << "\n";
    }

    out.close();
    return true;
}

int University::load(const string& path, int& skipped) {
    skipped = 0;
    ifstream in(path);

    if (!in.is_open()) {
        cout << "Не вдалося відкрити файл для читання: " << path << endl;
        return 0;
    }

    vector<unique_ptr<Person>> loaded;
    string line;
    int lineNumber = 0;

    while (getline(in, line)) {
        lineNumber++;

        if (line.empty()) {
            continue;
        }

        vector<string> fields = splitLine(line);
        map<string, Creator>::iterator it = creators_.find(fields[0]);

        if (it == creators_.end()) {
            cout << "Рядок " << lineNumber << " пропущено: невідомий тип " << fields[0] << endl;
            skipped++;
            continue;
        }

        try {
            loaded.push_back(it->second(fields));
        }
        catch (const out_of_range&) {
            cout << "Рядок " << lineNumber << " пропущено: не вистачає полів\n";
            skipped++;
        }
        catch (const exception& e) {
            cout << "Рядок " << lineNumber << " пропущено: " << e.what() << endl;
            skipped++;
        }
    }

    in.close();

    if (loaded.empty()) {
        return 0;
    }

    people_ = move(loaded);
    return (int)people_.size();
}