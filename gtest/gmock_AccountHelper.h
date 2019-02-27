#include <map>
#include "gmock_Account.h"

using namespace std;

class AccountHelper {
private:
    map<string, Account> m_mapAccounts; // 内部用来存储所有的测试账户

public:
    AccountHelper(map<string, Account> &mAccount);
    AccountHelper() {;}

    void updateAccount(const Account &account);

    Account findAccountForUser(const string &user_id);
}; 
