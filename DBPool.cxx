#include "DBPool.h"
#include "DBPoolConfiguration.h"
#include "DebugUtil.h"
#include "DBConnectionFactory.h"
#include <time.h>

// initialize our singleton instance
DBConnectionPools* DBPool::s_pools = NULL;
Mutex DBPool::s_initializationMutex;

//-------------------------------------------------------------------
// Singleton accessor method - this insures we have an automatic way
// to initialize the class (it will be initialized when this method is 
// first called
DBPool& DBPool::getInstance(const string& poolName) throw (DBException)
{
    // use a mutex to make sure we don't have multiple threads trying to 
    // initialize simultaneously. Since we are only concerned about
    // initialization and not returning this back to the user
    // we are only blocking around the initialization piece so wrap
    // that in {} - this is done to try and make this as efficient as
    // possible
    {
        MutexHandler initializationMutex(s_initializationMutex);

        // we are lazily creating the pool so if we haven't created our DB Pool
        // create it
        if (s_pools == NULL)
        {
            // create our holder for our connection pools
            s_pools = new DBConnectionPools();

            // now initialize the pools (will be initialized from our configuration)
            initializePools();
        }
    }

    // now that we know we have an initialized pool find the pool the user
    // is requesting

    // first see if we are given a pool name, we may not be (which indicates to use the default)
    string name;
    if (poolName.length() == 0)
    {
        // ok we are not given a pool name so use the default pool as specified in the config
        name = DBPoolConfiguration::getConfiguration().getDefaultPool();
        Debugger::debugger << "No pool name provided, using default of: " << name << "\n";
    }
    else
    {
        // the user gave us a pool so use it
        name = poolName;
        Debugger::debugger << "Using pool " << name << "\n";
    }

    // now get our pool from the set of pools
    DBPool*& pool = (*s_pools)[name];

    // if we found the pool then return it to the user
    if (pool != NULL)
    {
        return *pool;
    }
    else 
    {
        // we didn't find the pool - we have a serious problem here!!
        throw DBException(poolName + " is not a valid pool name!");
    }
}

//-------------------------------------------------------------------
// Static method that initializes the individual instances of DBPool
// with each representing a distinct connection pool
void DBPool::initializePools(void)
{
    // get our list of pools from the config service
    vector<string> pools = DBPoolConfiguration::getConfiguration().getPools();

    // now iterator our list of pools and initialize each pool    
    for (vector<string>::iterator it = pools.begin(); it != pools.end(); it++)
    {
        // create a new instance of DBPool for the given pool (this constructor
        // will cause the pool to initialize itself
        DBPool* pool = new DBPool(*it);

        // now add this to our map of pools
        (*s_pools)[*it] = pool;
    }
}

//-------------------------------------------------------------------
// Constructor - initializes our pool based on configuration information
//
DBPool::DBPool(void)
    : m_poolName(""), m_testFunction(NULL)
{
    // this constructor should never be used
}

DBPool::DBPool(const string& poolName) 
    : m_poolName(poolName), m_testFunction(NULL)
{
    Debugger::debugger << "Creating pool " << m_poolName << "\n";
    initializePool();
}

//-------------------------------------------------------------------
// Destructor
DBPool::~DBPool() 
{
    cleanupPool();
}

//-------------------------------------------------------------------
// Initialize our pool of database connections
void DBPool::initializePool(void)
{
    // our initial pool size is based on configuration
    long initialPoolSize = DBPoolConfiguration::getConfiguration().getInitialPoolSize(m_poolName);

    Debugger::debugger << "Initializing connection pool with " << initialPoolSize << " connections" << "\n";

    // now create our connections for our initial pool
    createConnections(initialPoolSize);
}

//-------------------------------------------------------------------
// Releases all connections in the pool
void DBPool::cleanupPool(void)
{
    // go through our set of available connections and release the connections
    while (!m_availablePool.empty())
    {
        DBConnection* connection = m_availablePool.front();
        m_availablePool.pop();
        DBConnectionFactory::getInstance().releaseConnection(connection);
    }

    // now clean-up any connections orphaned in our in-use list - this is bad
    // but I'm not sure I can
    for (InUseConnections::iterator it = m_inUsePool.begin(); it != m_inUsePool.end(); it++)
    {
        // release our connection
        DBConnectionFactory::getInstance().releaseConnection((DBConnection*) (*it));
    }
    m_inUsePool.clear();
    
}

//-------------------------------------------------------------------
// Gets a new connection from the pool
DBConnectionPtr DBPool::getConnection(void) throw (DBException)
{
    DBConnection* connection = NULL;

    // we need to insure serialized access to this method so we don't run
    // in to timer problems where multiple people are accessing the connection
    // list or this logic simultaneously. So I am using a home-grown mutex to do
    // this. I thought STL has one built-in but I could not find it so I ended 
    // up building my own
    MutexHandler mutexHandler(m_availablePoolMutex);

    // Note: one thing to note here is I could get the config info outside of the loop
    // to avoid overhead that might be associated with getting those values, however,
    // I am not doing that to allow the configuration to manage the information such
    // that dynamic updates will be immediately reflected, etc...

    // set a our start-time for when we started trying to get a connection
    time_t startTime = 0;


    // we are putting ourselves in a while loop to try and get a connection
    // our while condition is 'while(true)' which means, in theory, this could
    // be an infinite loop but we have a number of break conditions that will cause
    // us to exit the loop and it is those conditions we are looking for like 
    // getting a valid connection or timing out
    while (true)
    {
        // try and get a connection from the pool
        connection = getConnectionFromPool();

        // if we got a connection back then see if we should test it
        if (connection != NULL)
        {
            // if we are to validate the connection before returning then validate the 
            // connection
            if (DBPoolConfiguration::getConfiguration().validateConnectionBeforeReturning(m_poolName))
            {
                // test the connection, if it is not valid then we will release the connection
                // and set our reference to bull
                if (!testConnection(connection))
                {
                    // bad connection - release this reference and reset our variable
                    DBConnectionFactory::getInstance().releaseConnection(connection);
                    connection = NULL;
                }
            }

            // if our connection variable is still valid then we have a connection
            // good enough to return to the user
            if (connection != NULL)
            {
                // add this connection to our in-use pool
                addInUseConnection(connection);

                Debugger::debugger << "Got a connection from the pool" << "\n";
                // break out of our loop
                return DBConnectionPtr(connection, m_poolName);
            }
        }

        // if we are here then we didn't get a connection on the above try so
        // see if we should wait for the connection and if so, have we timed
        // out (we are in a loop after all)
        if (DBPoolConfiguration::getConfiguration().waitForConnection(m_poolName))
        {
            // see if we have waited at all yet (our first time through the loop
            // we initialize a start time - otherwise we see if we have timed out
            if (startTime == 0)
            {
                // initialize our start time
                Debugger::debugger << "Waiting for a connection to become available...." << "\n";
                time(&startTime);
            }
            else 
            {
                // this is not our first time with this so see if we should have expired

                // get our wait period
                long wait = DBPoolConfiguration::getConfiguration().getConnectionWaitPeriod(m_poolName);

                // if our wait period is not infinite we need to see if we have waited
                // long enough (in which case we will error out). If it is infinite
                /// we just keep waiting for a valid connections
                if (wait != DBPoolConfiguration::CONNECTION_WAIT_PERIOD_INFINITE)
                {
                    time_t currentTime;
                    time(&currentTime);
                    if (difftime(currentTime, startTime) > wait)
                    {
                        Debugger::debugger << "Timed out waiting for a connection to become available" << "\n";

                        // oh, to bad - we have waited long enough for a connection
                        // so throw the user out with an error
                        throw DBException("Failed to obtain a connection from the pool");
                    }
                }
            }
        }
        else
        {
            // we are not configured to wait for a connection to become available
            // so break out without acquiring a connection
            Debugger::debugger << "No connection available in pool!" << "\n";
            throw DBException("Failed to obtain a connection from the pool!");
        }
    }
}

//-------------------------------------------------------------------
// Releases a connection back to the pool
void DBPool::releaseConnection(DBConnectionPtr& connection) throw (DBException)
{
    Debugger::debugger << "Releasing a connection back to our pool" << "\n";

    // since we are using iterators we have to be sure we are thread safe here 
    // so we are using a mutex to insure only one person is releasing a connection
    // at a time - the mutex will automatically release once this method completes
    MutexHandler inUsePoolMutexHandler(m_inUsePoolMutex);

    // find this connection in our in-use pool
    InUseConnections::iterator iterator = m_inUsePool.find((void*)connection.getConnection());
    
    // if we found our element then erase it 
    if (iterator != m_inUsePool.end())
    {
        m_inUsePool.erase(iterator);
        
        // now put this connection back in to our available pool

        // first, make sure this is a valid connection
        if (connection->invalidConnection())
        {
            // this is an invalid connection so just drop it (we do this by deleting our
            // pointer to the connection
            DBConnectionFactory::getInstance().releaseConnection((DBConnection*)connection);
        }
        else 
        {
            // ok, our connection has not been flagged by the caller as being invalid
            // so add it back to our pool
            m_availablePool.push(connection.getConnection());
        }
    }
    else 
    {
        throw DBException("Failed to find connection in pool - something is very wrong");
    }

}

//-------------------------------------------------------------------
// Add connections to our connection pool - this can be used to seed
// our pool or to add additional connections up to our max size
void DBPool::createConnections(long numConnections)
{
    Debugger::debugger << "Creating " << numConnections << " connections in pool." << "\n";

    unsigned long maxPoolSize = DBPoolConfiguration::getConfiguration().getMaxPoolSize(m_poolName);

    // add a number of connections in to our pool
    for (int i = 0; i < numConnections; i++)
    {
        // make sure we have not reached out limit - if we haven't then add
        // the new connection
        if ((m_availablePool.size() + m_inUsePool.size()) < maxPoolSize)    
        {
            m_availablePool.push(DBConnectionFactory::getInstance().createConnection(m_poolName));
        }
        else 
        {
            // we are at our max so we cannot add any more connections
            Debugger::debugger << "Connection cannot be created, maximum pool size has been reached" << "\n";
            break;
        }
    }
}

//-------------------------------------------------------------------
// Common routine for popping a connection off of the available connection
// pool. 
DBConnection* DBPool::getConnectionFromPool(void) throw (DBException)
{
    Debugger::debugger << "Getting a connection from the pool..." << "\n";

    DBConnection* connection = NULL;

    // if our pool is currently empty then try and add some connections to the pool
    if (m_availablePool.empty())
    {
        // try to add some connections to the pool
        createConnections(DBPoolConfiguration::getConfiguration().getPoolIncrement(m_poolName));
    }

    // now try and get a connection from the pool - the pool could still be empty so
    // make sure we have available connections before doing this
    if (!m_availablePool.empty())
    {
        // pop a connection off of the pool
        connection = (DBConnection*) m_availablePool.front();
        m_availablePool.pop();
    }

    // return our connection
    return connection;
}

//-------------------------------------------------------------------
// Adds a connection as an in-use connection
void DBPool::addInUseConnection(DBConnection* connection)
{
    // simply add it to our set of in-use connections
    m_inUsePool.insert((void*) connection);
}

//-------------------------------------------------------------------
// A debug routine for querying how many connections are availabe in the
// current pool
size_t DBPool::getAvailableConnections(void)
{
    return m_availablePool.size();
}

//-------------------------------------------------------------------
// Debug routine for querying how many connections are currently in use
size_t DBPool::getInUseConnections(void)
{
    return m_inUsePool.size();
}

//-------------------------------------------------------------------
// Registers a test function with the connection.
// This function will be called whenever the connection needs to 
// be tested to insure it is "live"
void DBPool::registerTestFunction(CONNECTIONTEST test) 
{ 
    m_testFunction = test;
}

//-------------------------------------------------------------------
// Test the database connection to insure it is valid
bool DBPool::testConnection(DBConnection* connection) 
{
    // if we have a registered test function for this pool us it
    if (m_testFunction != NULL) 
    {
        return m_testFunction(connection);
    } 
    else
    {
        // else we have no registered test function so just ask the connection
        return connection->testConnection();
    }
}

