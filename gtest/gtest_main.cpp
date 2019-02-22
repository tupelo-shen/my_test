#include "gtest/gtest.h"

// 也可以不写main函数，直接在编译的时候链接libgtest_main.a库
int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}