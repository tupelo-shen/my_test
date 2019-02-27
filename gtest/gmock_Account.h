#pragma once
#include <string>

using namespace std;

class Account
{
private:
    string  m_account_id;
    long    m_balance;

public:
    Account() : m_account_id(""), m_balance(0) {;}

    Account(const string &init_acc_id, long init_balance) : 
        m_account_id(init_acc_id), 
        m_balance(init_balance) {;}

    bool            isNil() { return m_account_id == "";}
    void            debit(long amount) { m_balance -= amount;}
    void            credit(long amount) { m_balance += amount;}
    long            getBalance() const { return m_balance;}
    string          getAccountId() const { return m_account_id;}
    void            setAccountId(const string& id) { m_account_id = id;}
};
