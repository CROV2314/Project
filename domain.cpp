#include "domain.h"
#include "errors.h"

#include <iomanip>
#include <sstream>

using namespace std;

int Person::counter_ = 0;

string formatNumber(double value, int decimals) {
    ostringstream out;
    out << fixed << setprecision(decimals) << value;
    return out.str();
}

static string plain(double value) {
    ostringstream out;
    out << setprecision(10) << value;
    return out.str();
}

static string noSemicolon(string text) {
    for (size_t i = 0; i < text.size(); i++) {
        if (text[i] == ';') {
            text[i] = ',';
        }
    }
    return text;
}

Person::Person(const string& fullName, int birthYear) : id_(0) {
    setFullName(fullName);
    setBirthYear(birthYear);
    id_ = ++counter_;
}

Person::Person(const Person& other)
    : id_(++counter_), fullName_(other.fullName_), birthYear_(other.birthYear_) {
}

int Person::id() const {
    return id_;
}

int Person::created() {
    return counter_;
}

const string& Person::fullName() const {
    return fullName_;
}

int Person::birthYear() const {
    return birthYear_;
}

void Person::setFullName(const string& fullName) {
    if (fullName.empty()) {
        throw PersonError("ПІБ не може бути порожнім");
    }
    fullName_ = fullName;
}

void Person::setBirthYear(int birthYear) {
    if (birthYear < 1930 || birthYear > 2010) {
        throw PersonError("рік народження має бути від 1930 до 2010");
    }
    birthYear_ = birthYear;
}

void Person::print(ostream& os) const {
    os << "#" << id_ << " " << roleName() << ": " << fullName_
       << ", " << birthYear_ << " р.н.";
}

bool Person::operator<(const Person& other) const {
    if (fullName_ != other.fullName_) {
        return fullName_ < other.fullName_;
    }
    return birthYear_ < other.birthYear_;
}

bool Person::operator==(const Person& other) const {
    return typeName() == other.typeName()
        && fullName_ == other.fullName_
        && birthYear_ == other.birthYear_;
}

ostream& operator<<(ostream& os, const Person& person) {
    person.print(os);
    return os;
}

Student::Student(const string& fullName, int birthYear, double avgGrade, int course)
    : Person(fullName, birthYear) {
    setAvgGrade(avgGrade);
    setCourse(course);
}

double Student::avgGrade() const {
    return avgGrade_;
}

int Student::course() const {
    return course_;
}

void Student::setAvgGrade(double avgGrade) {
    if (avgGrade < 0 || avgGrade > 100) {
        throw PersonError("середній бал має бути від 0 до 100");
    }
    avgGrade_ = avgGrade;
}

void Student::setCourse(int course) {
    if (course < 1 || course > 6) {
        throw PersonError("курс має бути від 1 до 6");
    }
    course_ = course;
}

double Student::monthlyPayment() const {
    if (avgGrade_ >= 90) return 2000;
    if (avgGrade_ >= 75) return 1200;
    return 0;
}

int Student::weeklyHours() const {
    return 24;
}

string Student::roleName() const {
    return "Студент";
}

string Student::typeName() const {
    return "Student";
}

string Student::toLine() const {
    return typeName() + ";" + noSemicolon(fullName()) + ";" + to_string(birthYear())
         + ";" + plain(avgGrade_) + ";" + to_string(course_);
}

unique_ptr<Person> Student::clone() const {
    return make_unique<Student>(*this);
}

void Student::print(ostream& os) const {
    Person::print(os);
    os << ", бал " << formatNumber(avgGrade_, 1) << ", курс " << course_
       << ", стипендія " << formatNumber(monthlyPayment(), 2) << " грн";
}

Lecturer::Lecturer(const string& fullName, int birthYear, double hourlyRate, int hoursPerWeek)
    : Person(fullName, birthYear) {
    setHourlyRate(hourlyRate);
    setHoursPerWeek(hoursPerWeek);
}

double Lecturer::hourlyRate() const {
    return hourlyRate_;
}

int Lecturer::hoursPerWeek() const {
    return hoursPerWeek_;
}

void Lecturer::setHourlyRate(double hourlyRate) {
    if (hourlyRate <= 0) {
        throw PersonError("ставка має бути більшою за 0");
    }
    hourlyRate_ = hourlyRate;
}

void Lecturer::setHoursPerWeek(int hoursPerWeek) {
    if (hoursPerWeek < 1 || hoursPerWeek > 40) {
        throw PersonError("годин на тиждень має бути від 1 до 40");
    }
    hoursPerWeek_ = hoursPerWeek;
}

double Lecturer::monthlyPayment() const {
    return hourlyRate_ * hoursPerWeek_ * 4;
}

int Lecturer::weeklyHours() const {
    return hoursPerWeek_;
}

string Lecturer::roleName() const {
    return "Викладач";
}

string Lecturer::typeName() const {
    return "Lecturer";
}

string Lecturer::toLine() const {
    return typeName() + ";" + noSemicolon(fullName()) + ";" + to_string(birthYear())
         + ";" + plain(hourlyRate_) + ";" + to_string(hoursPerWeek_);
}

unique_ptr<Person> Lecturer::clone() const {
    return make_unique<Lecturer>(*this);
}

void Lecturer::print(ostream& os) const {
    Person::print(os);
    os << ", ставка " << formatNumber(hourlyRate_, 2) << ", годин " << hoursPerWeek_
       << ", виплата " << formatNumber(monthlyPayment(), 2) << " грн";
}

double Lecturer::examHours(int studentsCount) const {
    return studentsCount * 0.5;
}

Professor::Professor(const string& fullName, int birthYear, double hourlyRate,
                     int hoursPerWeek, int supervisedStudents)
    : Lecturer(fullName, birthYear, hourlyRate, hoursPerWeek) {
    setSupervisedStudents(supervisedStudents);
}

int Professor::supervisedStudents() const {
    return supervisedStudents_;
}

void Professor::setSupervisedStudents(int supervisedStudents) {
    if (supervisedStudents < 0) {
        throw PersonError("кількість аспірантів не може бути від'ємною");
    }
    supervisedStudents_ = supervisedStudents;
}

double Professor::monthlyPayment() const {
    return Lecturer::monthlyPayment() * 1.3 + 200 * supervisedStudents_;
}

string Professor::roleName() const {
    return "Професор";
}

string Professor::typeName() const {
    return "Professor";
}

string Professor::toLine() const {
    return typeName() + ";" + noSemicolon(fullName()) + ";" + to_string(birthYear())
         + ";" + plain(hourlyRate()) + ";" + to_string(hoursPerWeek())
         + ";" + to_string(supervisedStudents_);
}

unique_ptr<Person> Professor::clone() const {
    return make_unique<Professor>(*this);
}

void Professor::print(ostream& os) const {
    Lecturer::print(os);
    os << ", аспірантів " << supervisedStudents_;
}

double Professor::examHours(int studentsCount) const {
    return studentsCount * 0.75;
}

AdminStaff::AdminStaff(const string& fullName, int birthYear, double monthlySalary,
                       const string& department)
    : Person(fullName, birthYear) {
    setMonthlySalary(monthlySalary);
    setDepartment(department);
}

double AdminStaff::monthlySalary() const {
    return monthlySalary_;
}

const string& AdminStaff::department() const {
    return department_;
}

void AdminStaff::setMonthlySalary(double monthlySalary) {
    if (monthlySalary <= 0) {
        throw PersonError("зарплата має бути більшою за 0");
    }
    monthlySalary_ = monthlySalary;
}

void AdminStaff::setDepartment(const string& department) {
    if (department.empty()) {
        throw PersonError("відділ не може бути порожнім");
    }
    department_ = department;
}

double AdminStaff::monthlyPayment() const {
    return monthlySalary_;
}

int AdminStaff::weeklyHours() const {
    return 40;
}

string AdminStaff::roleName() const {
    return "Адміністрація";
}

string AdminStaff::typeName() const {
    return "AdminStaff";
}

string AdminStaff::toLine() const {
    return typeName() + ";" + noSemicolon(fullName()) + ";" + to_string(birthYear())
         + ";" + plain(monthlySalary_) + ";" + noSemicolon(department_);
}

unique_ptr<Person> AdminStaff::clone() const {
    return make_unique<AdminStaff>(*this);
}

void AdminStaff::print(ostream& os) const {
    Person::print(os);
    os << ", відділ " << department_ << ", зарплата "
       << formatNumber(monthlyPayment(), 2) << " грн";
}