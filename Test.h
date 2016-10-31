#ifndef __TESTTEMPLATE_H__
#define __TESTTEMPLATE_H__

#include "TestCase.h"
#include <string>

using namespace std;

template<class T> class Test : public T
{
typedef void (Test::*TESTFUNC)();

public:
    Test(TESTFUNC func, const string& name) 
            : T(name), m_function(func)
    {
    };

    virtual ~Test()
    {
    };

    void run()
    {
        this->setUp();
        (this->*m_function)();
        this->tearDown();        
    }

private:
    TESTFUNC m_function;
};

#endif

