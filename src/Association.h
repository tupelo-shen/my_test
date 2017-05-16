#ifndef _ASSOCIATION_H_
#define _ASSOCIATION_H_

#include <iostream>
#include <string>
#include <vector>

// 因为这两个类有循环依赖， 必须前置声明
class Doctor;

class Patient
{
private:
    std::string m_name;
    std::vector<Doctor *> m_doctor;

    void addDoctor(Doctor *doc);

public:
    Patient(std::string name)
        : m_name(name)
    {

    }

    friend std::ostream & operator<<(std::ostream & out, const Patient & patient);
    std::string getName() const { return m_name; }

    friend Doctor;
};

class Doctor
{
private:
    std::string m_name;
    std::vector<Patient *> m_patient;

public:
    Doctor(std::string name) :
        m_name(name)
    {

    }

    void addPatient(Patient *pat)
    {
        // Our doctor will add this patient
        m_patient.push_back(pat);

        pat->addDoctor(this);
    }

    friend std::ostream & operator<<(std::ostream & out, const Doctor & doc)
    {
        unsigned int length = doc.m_patient.size();

        if(length == 0)
        {
            out << doc.m_name << " has no patients right now ";

            return out;
        }

        out << doc.m_name << " is seeing patients: ";
        for (unsigned int count = 0; count < length; ++count)
            out << doc.m_patient[count]->getName() << ' ';

        return out;
    }

    std::string getName() const { return m_name; }
};

void Patient::addDoctor(Doctor *doc)
{
    m_doctor.push_back(doc);
}

std::ostream & operator<<(std::ostream & out, const Patient & pat)
{
    unsigned int length = pat.m_doctor.size();
    if (length == 0)
    {
        std::cout << pat.getName() << " has no doctors right now";
        return out;
    }
    out << pat.m_name << " is seeing doctors: ";
    for (unsigned int count = 0; count < length; ++count)
        out << pat.m_doctor[count]->getName() << ' ';
 
    return out;  
}
#endif /* _ASSOCIATION_H_ */