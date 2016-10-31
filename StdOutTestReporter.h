#ifndef __STDOUTTESTREPORTER_H__
#define __STDOUTTESTREPORTER_H__

#include "TestReporter.h"
#include <iostream>

using namespace std;

//*******************************************************************
// An interface used to capture output from the runs of a test case
class StdOutTestReporter : public TestReporter
{
public:
    StdOutTestReporter() {};
    ~StdOutTestReporter() {};

    virtual TestReporter& operator<<(string str) 
    {
        cout << str;
        return *this;
    };

    virtual TestReporter& operator<<(long l)
    {
        cout << l;
        return *this;
    };

    virtual TestReporter& operator<<(int i)
    {
        cout << i;
        return *this;
    };

    virtual TestReporter& operator<<(size_t s)
    {
        cout << (unsigned long)s;
        return *this;
    };
};

#endif