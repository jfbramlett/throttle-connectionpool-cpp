#ifndef __DBPOOLTESTCASE_H__
#define __DBPOOLTESTCASE_H__

#include "TestCase.h"
#include "DBConnectionPtr.h"
#include <string>

using namespace std;

class DBPoolTestCase : public TestCase
{
public:
    DBPoolTestCase(string name);
    virtual ~DBPoolTestCase(void);

    // define our test method
    void testInitializePool() throw (TestFail);
    void testGettingConnectionFromPool() throw (TestFail);
    void testPoolSizing() throw (TestFail);
    void testMaxingOutPool() throw (TestFail);
    void testMultiplePool() throw (TestFail);

    void setUp();

    static TestSuite suite();


protected:
    string m_poolName;
};

bool validatePoolConnection(DBConnection* connection);

#endif

