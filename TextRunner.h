#ifndef __TEXTRUNNER_H__
#define __TEXTRUNNER_H__

#include "TestCase.h"

template<class T> class TextRunner : public StdOutTestReporter
{
public:
    TextRunner() {};
    virtual ~TextRunner() {};

    virtual void run()
    {
        long passedTests = 0;
        long failedTests = 0;

        // get our tests to run
        TestSuite tests = T::suite();
        for (TestSuite::iterator it = tests.begin(); it != tests.end(); it++)
        {
            *this << "Running test: " << (*it)->getTestName() << "\n";
            *this << "==============";
            for (size_t l = 0; l < (*it)->getTestName().size(); l++)
            {
                *this << "=";
            }
            *this << "\n";

            try 
            {
                (*it)->run();
                passedTests++;
                *this << "Test PASSED" << "\n\n";

            }
            catch (TestFail failure)
            {
                failedTests++;
                *this << "[" << failure.m_msg << "]" << "\n";
                *this << "Test FAILED" << "\n\n";
            }
            catch (...)
            {
                failedTests++;
                *this << "[Unexpected exception caused test to fail]" << "\n";
                *this << "Test FAILED" << "\n\n";
            }
        }
        *this << "\n" << "Test execution summary:" << "\n";
        *this << passedTests << " tests passed" << "\n";    
        *this << failedTests << " tests failed" << "\n";    

        if (failedTests > 0)
        {
            *this << "\n" << "TEST FAILED!!!" << "\n";
        }
        else
        {
            *this << "\n" << "TEST PASSED!!!" << "\n";
        }
    }

private:
};


#endif

