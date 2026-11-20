#pragma once

#include "domain.h"

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

using Creator = std::function<std::unique_ptr<Person>(const std::vector<std::string>&)>;

template <typename Collection, typename Func>
double sumBy(const Collection& items, Func value) {
    double sum = 0;
    for (size_t i = 0; i < items.size(); i++) {
        sum += value(items[i]);
    }
    return sum;
}

template <typename Collection, typename Func>
int indexOfMax(const Collection& items, Func value) {
    int best = -1;
    for (size_t i = 0; i < items.size(); i++) {
        if (best == -1 || value(items[i]) > value(items[best])) {
            best = (int)i;
        }
    }
    return best;
}

class University {
public:
    void registerType(const std::string& tag, Creator creator);

    void add(std::unique_ptr<Person> person);
    bool remove(int id);
    int duplicate(int id);
    bool empty() const;
    int size() const;

    void showAll(std::ostream& os) const;
    void showByRoles(std::ostream& os) const;

    const Person* findById(int id) const;
    std::vector<const Person*> searchByName(const std::string& text) const;
    std::vector<const Person*> filterByMinHours(int hours) const;
    void sortBy(int criterion);

    std::map<std::string, double> payrollByRole() const;
    double totalPayroll() const;
    double totalWeeklyHours() const;
    const Person* highestPaid() const;
    double examSchedule(int studentsCount, std::ostream& os) const;

    bool save(const std::string& path) const;
    int load(const std::string& path, int& skipped);

private:
    std::vector<std::unique_ptr<Person>> people_;
    std::map<std::string, Creator> creators_;
};