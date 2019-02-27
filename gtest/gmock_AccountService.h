#pragma once
#include <string.h>
#include "gmock_Account.h"
#include "gmock_AccountManager.h"

using namespace std;

class AccountService 
{ 
private: 
    AccountManager* pAccountManager; 

public: 
    AccountService(); 

    void        setAccountManager(AccountManager* pManager); 

    bool        withdraw(const string &id, long amount); 

    void        topup(const string &id, long amount); 

    void        transfer(const string& senderId, const string& beneficiaryId, long amount); 
};


