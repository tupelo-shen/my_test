#include "gmock_AccountHelper.h"
#include "gmock_MockAccountManager.h"
#include "gmock_AccountService.h"

TEST(AccountServiceTest, transferTest) 
{
    map<std::string, Account> mapAccounts;
    mapAccounts["A"] = Account("A", 3000);
    mapAccounts["B"] = Account("B", 2000);
    AccountHelper helper(mapAccounts);

    MockAccountManager *pManager = new MockAccountManager();

    EXPECT_CALL(*pManager, findAccountForUser(testing::_))
        .Times(2)
        .WillRepeatedly(testing::Invoke(&helper, &AccountHelper::findAccountForUser));

    EXPECT_CALL(*pManager, updateAccount(testing::_)).WillRepeatedly(
        testing::Invoke(&helper, &AccountHelper::updateAccount));

    AccountService as;
    as.setAccountManager(pManager);
    as.transfer("A", "B", 1005);

    EXPECT_EQ(1995, helper.findAccountForUser("A").getBalance());
    EXPECT_EQ(3005, helper.findAccountForUser("B").getBalance());

    delete pManager;
}

TEST(AccountServiceTest, topupTest) 
{
    AccountHelper helper;

    MockAccountManager *pManager = new MockAccountManager();

    EXPECT_CALL(*pManager, findAccountForUser(testing::_))
        .WillRepeatedly(testing::Invoke(&helper, &AccountHelper::findAccountForUser)); 

    EXPECT_CALL(*pManager, findAccountForUser("B"))
        .WillRepeatedly(testing::Invoke(&helper, &AccountHelper::findAccountForUser));

    EXPECT_CALL(*pManager, findAccountForUser("A"))
        .WillRepeatedly(testing::Return(Account("A", 5))); 

    EXPECT_CALL(*pManager, updateAccount(testing::_)).WillRepeatedly(
        testing::Invoke(&helper, &AccountHelper::updateAccount));

    AccountService as;
    as.setAccountManager(pManager);
    as.topup("B", 1005);
    as.topup("A", 5); 

    EXPECT_EQ(10, helper.findAccountForUser("A").getBalance());
    EXPECT_EQ(1005, helper.findAccountForUser("B").getBalance());

    delete pManager;
}
