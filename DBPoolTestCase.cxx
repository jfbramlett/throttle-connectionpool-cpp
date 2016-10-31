#include "DBPoolTestCase.h"
#include "DBPool.h"
#include "DBPoolConfiguration.h"
#include "Test.h"
#include <vector>

using namespace std;

//-------------------------------------------------------------------
// Constructor - initialize what tests should be run
DBPoolTestCase::DBPoolTestCase(string testName)
    : TestCase(testName)
{
}

//-------------------------------------------------------------------
// Destructor
DBPoolTestCase::~DBPoolTestCase(void)
{
}

//-------------------------------------------------------------------
// Defines the set of tests to run
TestSuite DBPoolTestCase::suite()
{
    TestSuite tests;

    // create a vector of tests to run - each test is created by wrapping it in our
    // test template and defining a function pointer to the test to run. The second
    // parameter is a "friendly" name that will be displayed when this test is run
    tests.push_back(new Test<DBPoolTestCase>(&DBPoolTestCase::testInitializePool, "TestInitializePool"));
    tests.push_back(new Test<DBPoolTestCase>(&DBPoolTestCase::testGettingConnectionFromPool, "TestGettingConnectionFromPool"));
    tests.push_back(new Test<DBPoolTestCase>(&DBPoolTestCase::testPoolSizing, "TestPoolSizing"));
    tests.push_back(new Test<DBPoolTestCase>(&DBPoolTestCase::testMaxingOutPool, "TestMaxingOutPool"));
    tests.push_back(new Test<DBPoolTestCase>(&DBPoolTestCase::testMultiplePool, "TestMultiplePool"));

    return tests;
}

//-------------------------------------------------------------------
// Set-up routine, perform any initialization needed by the test
void DBPoolTestCase::setUp()
{
    m_poolName = "oracle";
    DBPool::getInstance(m_poolName).registerTestFunction(&validatePoolConnection);
}

//-------------------------------------------------------------------
// Method that validates a database connection
bool validatePoolConnection(DBConnection* connection)
{
    cout << "Validating connection" << "\n";
    return true;
}

//-------------------------------------------------------------------
// Tests initialize the pool of connections
void DBPoolTestCase::testInitializePool() throw (TestFail)
{
    // initialize our pool
    DBPool::getInstance();
    assertTrue(DBPool::getInstance().getAvailableConnections() > 0);
}

//-------------------------------------------------------------------
// Test getting a connection from the pool
void DBPoolTestCase::testGettingConnectionFromPool() throw (TestFail)
{
    // see how many connections are in the pool
    size_t originalPoolCount = DBPool::getInstance(m_poolName).getAvailableConnections();

    // now get a connection from the pool - we wrap this in {} so we can test the
    // releasing back to the pool
    {
        DBConnectionPtr connection = DBPool::getInstance(m_poolName).getConnection();

        // now see that our pool count is less one
        size_t newPoolCount = DBPool::getInstance(m_poolName).getAvailableConnections();

        // our new pool count should be 1 less that our original pool count
        assertTrue(newPoolCount == (originalPoolCount - 1));

        // make sure we have 1 connection checked out
        assertTrue(DBPool::getInstance(m_poolName).getInUseConnections() == 1);
    }

    // now that we are out of scope our connection should be back in the pool
    assertTrue("Pool connections don't match!", DBPool::getInstance(m_poolName).getAvailableConnections() == originalPoolCount);

    // make sure we have no active connections
    assertTrue("Still have connections checked out", DBPool::getInstance(m_poolName).getInUseConnections() == 0);
}

//-------------------------------------------------------------------
// Test getting the pool to resize itself when there are no connections
// available in the pool test the timing out of the request
void DBPoolTestCase::testPoolSizing() throw (TestFail)
{
    // get 1 more connection than our pool size offers

    // see how many connections are in the pool - then we will get 1 more connection
    size_t originalPoolCount = DBPool::getInstance(m_poolName).getAvailableConnections();

    // wrap this in {} so we can verify our pool after clean-up to make sure everything
    // gets put back
    {
        DBConnectionPtr connections[500];

        // now create our connection - go to our pool count which will be 1 more than
        // is currently in the pool
        for (size_t i = 0; i <= originalPoolCount; i++)
        {
            connections[i] = DBPool::getInstance(m_poolName).getConnection();
        }

        // verify we have more connections than in our original pool
        assertTrue(DBPool::getInstance(m_poolName).getInUseConnections() > originalPoolCount );
    }

    *m_reporter << "Number in pool before test: " << originalPoolCount << "\n";
    *m_reporter << "Number in pool after test:  " << DBPool::getInstance(m_poolName).getAvailableConnections() << "\n";

    // now that everything is cleaned up verify that our pool is bigger than when we started
    assertTrue(DBPool::getInstance(m_poolName).getAvailableConnections() > originalPoolCount);
}

//-------------------------------------------------------------------
// Test maxing out the connections in the pool - this test will
// only run when our timeout for getting a connection is some value
// (i.e. not infinite), the net result of this test is a timeout
// waiting for a connection to become available
void DBPoolTestCase::testMaxingOutPool() throw (TestFail)
{
    // before we can run this test make sure our wait state isn't infinite
    long waitPeriod = DBPoolConfiguration::getConfiguration().getConnectionWaitPeriod(m_poolName);
    if (waitPeriod == -1)
    {
        // fail this test since it cannot execute because our timeout is set to infinite
        // and since this test is not threaded it will never complete
        assertTrue("Wait period is infinite, the test cannot execute", false);
    }


    // get 1 more connection than our maximum pool size
    size_t maxPoolSize = DBPoolConfiguration::getConfiguration().getMaxPoolSize(m_poolName);

    // wrap this in {} so we can verify our pool after clean-up to make sure everything
    // gets put back

    // use a local variable outside the scope of the exception so we can
    // see when we exhaust the pool
    size_t poolCount = 0;
    try {
        DBConnectionPtr connections[500];

        // now create connections to overrun the pool
        for (; poolCount <= maxPoolSize + 1; poolCount++)
        {
            connections[poolCount] = DBPool::getInstance(m_poolName).getConnection();
        }

        // if we are here then we failed
        assertTrue("Successfully obtained all the connections requested - should have failed", false);
    }
    catch (DBException de)
    {
        // we should be here since we are trying to overrun the pool
        // we are saying == since poolCount is 0 based and the max is 1 based 
        // (so if poolCount == 10 then we have 11 connections)
        assertTrue("Our pool count is not greater than the max pool size, this is wrong!", poolCount == maxPoolSize);
    }
}


//-------------------------------------------------------------------
// Test knowing we have multiple pools and insuring the interaction
// against one pool does not impact the other
void DBPoolTestCase::testMultiplePool() throw (TestFail)
{
    // we have 2 pools, one called 'default' and another called 'oracle'
    // get the initial size of each pool
    size_t oracleSize = DBPool::getInstance("oracle").getAvailableConnections();
    size_t defaultSize = DBPool::getInstance("default").getAvailableConnections();

    // now get an oracle connection - use {} so we release the connection back to the
    // pool inside of the scope of this test
    {
        DBConnectionPtr connection = DBPool::getInstance("oracle").getConnection();

        // now verify our oracle count is less than the original size
        assertTrue("Connection was not removed from pool!", oracleSize > DBPool::getInstance("oracle").getAvailableConnections());

        // now verify the default pool has not changed
        assertTrue("The default pool was modified!", defaultSize == DBPool::getInstance("default").getAvailableConnections());
    }

    // now verify our initial pool sizes after the connection is released back to the pool
    assertTrue("Connection not released back to pool!", oracleSize == DBPool::getInstance("oracle").getAvailableConnections());
    assertTrue("The default pool was modified!", defaultSize == DBPool::getInstance("default").getAvailableConnections());
}
