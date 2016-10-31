#ifndef __TESTCASE_H__
#define __TESTCASE_H__

#include <string>
#include <vector>
#include "StdOutTestReporter.h"

using namespace std;

class TestCase;

typedef vector<TestCase*> TestSuite;

//*******************************************************************
// Exception used within asserts to identify a test as failing
class TestFail
{
public:
    TestFail(string str) : m_msg(str) {};
    TestFail() {};
    ~TestFail() {};

    string m_msg;
};

//*******************************************************************
// Base class for all test cases
class TestCase
{
public:
    TestCase(string name);
    virtual ~TestCase(void);

    virtual void run() = 0;

    virtual void setUp();
    virtual void tearDown();

    string getTestName();

    void registerTestReporter(TestReporter* reporter);

protected:
    void assertTrue(bool val) throw (TestFail);
    void assertTrue(string msg, bool val) throw (TestFail);

    TestReporter* m_reporter;
    StdOutTestReporter m_defaultReporter;
    string m_name;
};


#endif

