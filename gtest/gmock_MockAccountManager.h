#include "gmock_AccountManager.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

class MockAccountManager : public AccountManager 
{
public:
    MOCK_METHOD1(findAccountForUser, Account(const string&));
    MOCK_METHOD1(updateAccount, void(const Account&));
};

