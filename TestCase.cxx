#include "TestCase.h"
#include <iostream>

//-------------------------------------------------------------------
// Constructor
TestCase::TestCase(string name) : m_name(name)
{
    // set our default reportor as stdout (we have this as a member variable
    // and are just pointing our pointer to this member variable)
    m_reporter = &m_defaultReporter;
}

//-------------------------------------------------------------------
// Destructor - clean-up memory from this test
TestCase::~TestCase(void)
{
}

//-------------------------------------------------------------------
// Register a reporter which is used in logging messages and reporting
// on the test execution
void TestCase::registerTestReporter(TestReporter* reporter)
{
    // set our report to the one provided
    m_reporter = reporter;
}

//-------------------------------------------------------------------
// Return the name of the test being executed
string TestCase::getTestName()
{
    return m_name;
}

/*
bool TestCase::run()
{
    bool passed = true;
    long passedTests = 0;
    long failedTests = 0;

    for (int i = 0; i < m_testCount; i++)
    {
        try {
            *this << "Running test: " << m_testNames[i] << "\n";
            *this << "==============";
            for (size_t l = 0; l < m_testNames[i].size(); l++)
            {
                *this << "=";
            }
            *this << "\n";

            this->setUp();
            (m_tests[i])(this);
            this->tearDown();
            passedTests++;

            *this << "Test PASSED" << "\n\n";
        }
        catch (TestFail failure)
        {
            failedTests++;
            passed = false;
            *this << "[" << failure.m_msg << "]" << "\n";
            *this << "Test FAILED" << "\n\n";
        }
        catch (...)
        {
            failedTests++;
            passed = false;
            *this << "[Unexpected exception caused test to fail]" << "\n";
            *this << "Test FAILED" << "\n\n";
        }
    }

    *this << passedTests << " tests passed" << "\n";    
    *this << failedTests << " tests failed" << "\n";    
    return passed;
}
*/


void TestCase::setUp()
{
}

void TestCase::tearDown()
{
}

void TestCase::assertTrue(bool val) throw (TestFail)
{
    if (!val)
    {
        throw TestFail();
    }
}

void TestCase::assertTrue(string msg, bool val) throw (TestFail)
{
    if (!val)
    {
        throw TestFail(msg);
    }
}
