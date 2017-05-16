#include <string>
 
class Person
{
public:
    std::string     m_name;
    int             m_age;
 
    Person(std::string name = "", int age = 0)
        : m_name(name), m_age(age)
    {
    }
 
    std::string             getName() const { return m_name; }
    int                     getAge() const { return m_age; }
};

class BaseballPlayer : public Person
{
public:
    double          m_battingAverage;
    int             m_homeRuns;
 
    BaseballPlayer(double battingAverage = 0.0, int homeRuns = 0)
       : m_battingAverage(battingAverage), m_homeRuns(homeRuns)
    {
    }
};