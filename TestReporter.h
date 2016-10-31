#ifndef __TESTREPORTER_H__
#define __TESTREPORTER_H__

#include <string>

using namespace std;

//*******************************************************************
// An interface used to capture output from the runs of a test case
// This allows us to report from within the test to various output
// mechanisms such as stdout or some GUI
class TestReporter
{
public:
    virtual TestReporter& operator<<(string str) = 0;
    virtual TestReporter& operator<<(long l) = 0;
    virtual TestReporter& operator<<(int i) = 0;
    virtual TestReporter& operator<<(size_t s) = 0;
};

#endif