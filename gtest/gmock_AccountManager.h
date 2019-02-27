#pragma once
#include "gmock_Account.h"

class AccountManager {
public:
    virtual Account     findAccountForUser(const string &userId) = 0;
    virtual void        updateAccount(const Account &account) = 0;
}; 
