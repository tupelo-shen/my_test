#include "gmock_AccountService.h"

AccountService :: AccountService() : pAccountManager(NULL)
{

}

void AccountService::setAccountManager(AccountManager *pManager) 
{
    pAccountManager = pManager;
}

void AccountService :: transfer(const string& sender_id, const string& beneficiary_id, long amount)
{
    Account sender = pAccountManager->findAccountForUser(sender_id);
    Account beneficiary = pAccountManager->findAccountForUser(beneficiary_id);

    sender.debit(amount);
    beneficiary.credit(amount);

    pAccountManager->updateAccount(sender);
    pAccountManager->updateAccount(beneficiary);
}

bool AccountService :: withdraw(const string& id, long amount)
{
    Account owner = pAccountManager->findAccountForUser(id);
    if(owner.isNil())
    {
        return false;
    }

    owner.debit(amount);
    pAccountManager->updateAccount(owner);

    return true;
}

void AccountService :: topup(const string& id, long amount)
{
    Account owner = pAccountManager->findAccountForUser(id);
    if (owner.isNil())
    {
        owner.setAccountId(id);
    }

    owner.credit(amount); 
    pAccountManager->updateAccount(owner);    
}

