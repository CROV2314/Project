#pragma once

#include <iostream>
#include <memory>
#include <string>

std::string formatNumber(double value, int decimals);

class IExaminer {
public:
    virtual ~IExaminer() = default;
    virtual double examHours(int studentsCount) const = 0;
};

class Person {
public:
    Person(const std::string& fullName, int birthYear);
    virtual ~Person() = default;

    int id() const;
    static int created();

    const std::string& fullName() const;
    int birthYear() const;
    void setFullName(const std::string& fullName);
    void setBirthYear(int birthYear);

    virtual double monthlyPayment() const = 0;
    virtual int weeklyHours() const = 0;
    virtual std::string roleName() const = 0;
    virtual std::string typeName() const = 0;
    virtual std::string toLine() const = 0;
    virtual std::unique_ptr<Person> clone() const = 0;
    virtual void print(std::ostream& os) const;

    bool operator<(const Person& other) const;
    bool operator==(const Person& other) const;

protected:
    Person(const Person& other);

private:
    int id_;
    std::string fullName_;
    int birthYear_;
    static int counter_;
};

std::ostream& operator<<(std::ostream& os, const Person& person);

class Student : public Person {
public:
    Student(const std::string& fullName, int birthYear, double avgGrade, int course);

    double avgGrade() const;
    int course() const;
    void setAvgGrade(double avgGrade);
    void setCourse(int course);

    double monthlyPayment() const override;
    int weeklyHours() const override;
    std::string roleName() const override;
    std::string typeName() const override;
    std::string toLine() const override;
    std::unique_ptr<Person> clone() const override;
    void print(std::ostream& os) const override;

private:
    double avgGrade_;
    int course_;
};

class Lecturer : public Person, public IExaminer {
public:
    Lecturer(const std::string& fullName, int birthYear, double hourlyRate, int hoursPerWeek);

    double hourlyRate() const;
    int hoursPerWeek() const;
    void setHourlyRate(double hourlyRate);
    void setHoursPerWeek(int hoursPerWeek);

    double monthlyPayment() const override;
    int weeklyHours() const override;
    std::string roleName() const override;
    std::string typeName() const override;
    std::string toLine() const override;
    std::unique_ptr<Person> clone() const override;
    void print(std::ostream& os) const override;
    double examHours(int studentsCount) const override;

private:
    double hourlyRate_;
    int hoursPerWeek_;
};

class Professor : public Lecturer {
public:
    Professor(const std::string& fullName, int birthYear, double hourlyRate,
              int hoursPerWeek, int supervisedStudents);

    int supervisedStudents() const;
    void setSupervisedStudents(int supervisedStudents);

    double monthlyPayment() const override;
    std::string roleName() const override;
    std::string typeName() const override;
    std::string toLine() const override;
    std::unique_ptr<Person> clone() const override;
    void print(std::ostream& os) const override;
    double examHours(int studentsCount) const override;

private:
    int supervisedStudents_;
};

class AdminStaff : public Person {
public:
    AdminStaff(const std::string& fullName, int birthYear, double monthlySalary,
               const std::string& department);

    double monthlySalary() const;
    const std::string& department() const;
    void setMonthlySalary(double monthlySalary);
    void setDepartment(const std::string& department);

    double monthlyPayment() const override;
    int weeklyHours() const override;
    std::string roleName() const override;
    std::string typeName() const override;
    std::string toLine() const override;
    std::unique_ptr<Person> clone() const override;
    void print(std::ostream& os) const override;

private:
    double monthlySalary_;
    std::string department_;
};