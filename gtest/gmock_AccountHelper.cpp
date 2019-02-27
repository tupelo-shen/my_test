#include "gmock_AccountHelper.h"

AccountHelper::AccountHelper(map<string, Account> &mAccount) 
{
    m_mapAccounts = mAccount;
}

void AccountHelper::updateAccount(const Account &account) 
{
    m_mapAccounts[account.getAccountId()] = account;
}

Account AccountHelper::findAccountForUser(const std::string &user_id) 
{
    if (m_mapAccounts.find(user_id) != this->m_mapAccounts.end()) 
        return this->m_mapAccounts[user_id];
    return Account();
} 
