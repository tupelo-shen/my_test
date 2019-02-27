#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

// Gmock最大只支持10个参数的函数mock

class IFileApi{
public:
    virtual int open(const char* path, int o_flag)=0;
    virtual int read(const char* path, char* buffer, int size, int &size_ret)=0;
    virtual int close(int fd)=0;
};

class FileDataLoad {
public:
    FileDataLoad(IFileApi *api) : 
            m_fileApi(api),
            m_szFileData(NULL),         
            m_nDataSize(0){ }

    ~FileDataLoad() {
        if (m_szFileData != NULL) {
            delete [] m_szFileData;
        }
    }

    int LoadData(const char* path, int size)
    {  
        int fd = m_fileApi->open(path, 0777);   // 打开文件
        if (fd <= 0) { return -1; } // 我们假设 fd > 0. (真实环境中可能=0) 

        m_szFileData = new char[size+1];
        m_szFileData[size] = '\0';
        int size_ret = 0;
        // 读文件，仅仅作为示例，所以忽略参数size
        int ret = m_fileApi->read(path, m_szFileData, size, size_ret);
        if (ret != 0) {
            delete [] m_szFileData;
            return ret;
        }

        if (size != size_ret) { // 暂时不考虑           
            return -1;          
        }
        m_nDataSize = ret;

        // 关闭文件
        ret = m_fileApi->close(fd);     

        return ret;
    }

    char *GetFileData() {
        return m_szFileData;
    }

    int GetDataSize() {
        return m_nDataSize;
    }

private:
    IFileApi*   m_fileApi;
    char*       m_szFileData;
    int         m_nDataSize;
};

class MockFileApi : public IFileApi{
public:
    MOCK_METHOD2(open, int(const char*, int));
    MOCK_METHOD4(read, int(const char*, char*, int, int &));
    MOCK_METHOD1(close, int(int));
};

TEST(FileDataLoadTest, LoadData) 
{
    MockFileApi *api = new MockFileApi;
    FileDataLoad fileLoad((IFileApi*)api);
    int file_load_size = 100;

    // 我们期望当调用api->open()函数的时候，总是返回11，且这个函数仅被调用1次
    EXPECT_CALL(*api, open(::testing::_, ::testing::_))
        .Times(1)   // 期望被调用1次
        .WillRepeatedly(::testing::Return(11)); // 总是返回11

    // 我们期望调用 api->read 时, 设置第3个参数为 file_load_size 且返回 0。这个函数只被调用1次。 
    EXPECT_CALL(*api, read(::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .Times(1)
        .WillRepeatedly(
            ::testing::DoAll(
                ::testing::SetArgReferee<3>(file_load_size),
                ::testing::Return(0)
            )
        );

    // 我们期望调用 api->close 时, 返回0。 这个函数只被调用1次。
    EXPECT_CALL(*api, close(::testing::_))
        .Times(1)
        .WillRepeatedly(::testing::Return(0));

    int ret = fileLoad.LoadData("/home/test_file", file_load_size); 
    // 我们期望返回 0
    EXPECT_EQ(ret, 0);

    delete api;
}


int main(int argc, char **argv) {
    
    ::testing::InitGoogleMock(&argc, argv);

    return RUN_ALL_TESTS();
}

/*--------------------------------<代码分析>----------------------------------------------*/
(1):
在这儿，我们只写了一个TEST，即只有一个测试用例（test case），测试的目标函数是 FileDataLoad:: LoadData()

(2):
FileDataLoad::LoadData函数使用IFileApi*类型的成员变量m_fileApi 来完成加载文件内容的功能。
首先打开文件(open)，然后读取文件(read)，最后关闭文件句柄(close)。使用IFileApi纯虚接口的目的在于，
可以扩展不同应用场景的文件内容加载。比如可以实现一个本地文件内容加载的子类来实现文件内容加载：
class LocalFileApi : public IFileApi，使用POSIX标准文件系统api来实现本地文件open，read和close。
还可以实现一个云端文件内容加载：class CloudFileApi : public IFileApi，使用约定接口规范来实现
云端文件open，read和close。

(3):
我们在测试FileDataLoad:: LoadData时，无需关心m_fileApi是本地的还是云端的还是其他什么的，
我们只关心LoadData这个函数的逻辑是否正确，错误处理是否完整，是否有内存泄漏等等。

(4):
所以，我们只需模拟m_fileApi的open，read和close，并指定这三个函数一些操作（返回一些特定值，
设置一些特定返回参数等：在TEST宏后面大括号中，使用EXPECT_CALL来设定），以此来验证
FileDataLoad::LoadData的逻辑是否正确。

(5):
以模拟open函数为例：

首先，定义Mock类，并继承自IFileApi纯虚基类，使用MOCK_METHOD宏声明open：
     MOCK_METHOD2(open, int(const char*, int)); 
因为open有两个参数，所以使用MOCK_METHOD2。宏第一个参数为函数名，第二个参数为函数类型
然后，使用EXPECT_CALL来设置调用到这个函数时的期望，代码如下（gmock_test.cpp第86行）：

        EXPECT_CALL(*api, open(::testing::, ::testing::)) 
            .Times(1) 
            .WillRepeatedly(::testing::Return(11));
具体含义为，在这个期望设置完以后，只会发生一次api->open调用（.Times(1)的作用），并且
api->open被调用时的返回值为11（.WillRepeatedly(::testing::Return(11)的作用）

另外”::testing::_”是一种参数匹配语法，它表示匹配任意类型的参数，这里这样写是
我们这个测试用例不关心参数类型。
