#include <time.h>
#ifdef _WINDOWS
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif
#include "POMDBThrottleAdapter.h"
#include "P2KGatewayConfiguration.h"
#include "DBPool.h"
#include "OracleConnection.h"
#include "DebugUtil.h"

//*******************************************************************
// This is the routine that manages the pooling of the database to
// keep the database record count up to date in a separate thread.
// It does this by calling back in to the class to perform the necessary
// functions
//*******************************************************************
#ifdef _WINDOWS
void dbPollingThread(void* throttleAdapter)
{
	POMDBThrottleAdapter* pomDBThrottleAdapter = (POMDBThrottleAdapter*)throttleAdapter;
    pomDBThrottleAdapter->pollDatabase();
}
#else
void* dbPollingThread(void* throttleAdapter)
{
	POMDBThrottleAdapter* pomDBThrottleAdapter = (POMDBThrottleAdapter*)throttleAdapter;
    pomDBThrottleAdapter->pollDatabase();
    return NULL;
}
#endif


//-------------------------------------------------------------------
// Constructor - initialize our PM DB Adapter
POMDBThrottleAdapter::POMDBThrottleAdapter(void)
{
    m_dbRecordCount = 50;

	// if we are running threaded then start our thread
	if (P2KGatewayConfiguration::getConfiguration().isThrottleThreaded())
	{
        // if we are running threaded then we will need to block until our
        // thread is initialized and up and running
        m_threadInitialized = false;

        Debugger::debugger << "Spawning DB Polling Thread";

        // the windows implementation uses a very simple beginthread - the unix implementation
        // is a little more complicated so we have our ifdef's based on the platform
#ifdef _WINDOWS
        _beginthread(dbPollingThread, NULL, this);
#else
        pthread_attr_t attr;
        pthread_t thread;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        int result = pthread_create(&thread, &attr, &dbPollingThread, this);

        // we are checking out result code from creating the thread, this was mainly
        // added for debugging purposes as there are really not a lot of cases when 
        // a thread won't start (was having trouble initially but now that it works it
        // should work)
        if (result == EAGAIN)
        {
            Debugger::debugger << "Failed to create thread - insufficient system resources" << "\n";
        }
        else if (result == EFAULT)
        {
            Debugger::debugger << "Failed to create thread - failed to access buffers or start routine" << "\n";
        }
        else if (result == EINVAL)
        {
            Debugger::debugger << "Failed to create thread - invalid thread attribute" << "\n";
        } 
        else 
        {
            Debugger::debugger << "Created thread: " << result << "\n";
        }
#endif
    
        // block until our thread is initialized. This is done so we can fully initialize
        // everything (which will happen in the thread) before trying to start processing
        // anything
        while (!m_threadInitialized) ;
    }
}

//-------------------------------------------------------------------
// Destructor - clean-up our adapter
POMDBThrottleAdapter::~POMDBThrottleAdapter(void)
{
}

//-------------------------------------------------------------------
// The method is used to determine if we need to throttle by comparing 
// our max record count with the count of records in the database
bool POMDBThrottleAdapter::isThrottleRequired(void) throw (ThrottleException)
{
	bool fThrottle = false;

    try
    {
        // get our threshold amount
        long maxRecords = P2KGatewayConfiguration::getConfiguration().getRecordThreshold();
        long dbRecords = databaseRecordCount();

	    // if the database has more records than our specified max
	    // we should throttle by blocking requests until our DB
	    // state is back to a normal state

        Debugger::debugger << "Max Record: " << maxRecords << "\n";
        Debugger::debugger << "Database Records: " << dbRecords << "\n";

	    if (maxRecords <= dbRecords)
	    {
            Debugger::debugger << "Throttling required" << "\n";

		    // uh-oh we need to throttle
		    fThrottle = true;	
	    }
        else
        {
            Debugger::debugger << "No throttling required" << "\n";
        }
    }
    catch (ConfigurationException ce)
    {
        throw ThrottleException("Failed determining if throttling is required - there is a problem with the configuration: " + ce.message);
    }

	return fThrottle;
}

//-------------------------------------------------------------------
// Gets the number of records in the POM DB
long POMDBThrottleAdapter::databaseRecordCount() throw (ThrottleException)
{
	// we could be running synchronous or threaded, if synchronous then we need
	// to see if our database record count needs to be refreshed
    try 
    {
	    if (!P2KGatewayConfiguration::getConfiguration().isThrottleThreaded()) 
	    {
            // see if our timeout has expired. We do this by seeing how old our database record
            // count is (we timestamp when we retrieve it) and if it is older than our configured
            // timeout value we reset the value
            time_t currentTime;
            time(&currentTime);
            if (difftime(currentTime, m_dbRecordCountTime) > P2KGatewayConfiguration::getConfiguration().getRecordCountTimeout())
            {
                setDatabaseRecordCount();
            }
	    }
    }
    catch (ConfigurationException ce)
    {
        throw ThrottleException("Failed gettng the database record count - there is a problem with your configuration: " + ce.message);
    }

	return m_dbRecordCount;
}

//-------------------------------------------------------------------
// Sets the number of records in the POM database
void POMDBThrottleAdapter::databaseRecordCount(long count)
{
    Debugger::debugger << "Setting database record count: " << count << "\n";
	m_dbRecordCount = count;
}

//-------------------------------------------------------------------
// The call actually does the DB access to get the record count, it is
// abstracted here
void POMDBThrottleAdapter::setDatabaseRecordCount(void) throw (ThrottleException)
{
    Debugger::debugger << "Updating database record count" << "\n";

    // set the time this value was obtained
    time(&m_dbRecordCountTime);

	long lRecordCount = 0;

	try {
      // get our connection and create a statement to select the count
      // for the rows in the Q1 table
      DBConnectionPtr dbConnectionPtr = DBPool::getInstance().getConnection();

#ifdef __NO_DB__
	lRecordCount = 101;
#else
      OracleConnection* oracleConnection = (OracleConnection*) dbConnectionPtr.getConnection();
      Connection* lCon = oracleConnection->getConnection();

      Statement* lstmt;
      lstmt = lCon->createStatement("DECLARE PROCEDURE count_q1 (record_count OUT INT) IS \
                                     BEGIN                                                \
                                         SELECT COUNT(*) INTO record_count FROM Q1;       \
                                     END;                                                 \
                                     BEGIN                                                \
		                                  count_q1(:1);                                    \
                                     END;");         

      // now register an out parameter so we can get back out the count of the number of rows
      lstmt->registerOutParam(1, OCCIINT, sizeof(int));
      
      // execute our query
      lstmt->executeQuery();

      // get the count of the number of rows in Q1 by getting our out parameter
      lRecordCount = lstmt->getInt(1);

      // clean up the statement
      lCon->terminateStatement(lstmt);
#endif

      Debugger::debugger << "Retrieved count of " << lRecordCount << " from Q1 table" << "\n";
    } 
    catch (DBException dbex) 
    {
       Debugger::debugger << "Failed to get record count (DBException) " << "\n";
       throw ThrottleException("Failed querying database for number of records in Q1: " + dbex.message);
    }
#ifndef __NO_DB__
    catch (SQLException sqlex) 
    {
       Debugger::debugger << "Failed to get record count (SQLException) " << "\n";
       throw ThrottleException("Failed querying database for number of records in Q1: " + sqlex.getMessage());
    } 
#endif
    catch (...) {
       Debugger::debugger << "Failed to get record count (Unknown) " << "\n";
       throw ThrottleException("Failed querying database for number of records in Q1: unknown exception");
   }
   databaseRecordCount(lRecordCount);
}

//-------------------------------------------------------------------
// Gets the amount of time we should throttle (i.e. the time we should
// wait before performing an action)
long POMDBThrottleAdapter::getThrottlePeriod(void) throw (ThrottleException)
{
    try 
    {
        return P2KGatewayConfiguration::getConfiguration().getThrottlePeriod();
    }
    catch (ConfigurationException ce) 
    {
        throw ThrottleException("Failed to determine throttle period - there is a problem with your configuration: " + ce.message);
    }
}

//-------------------------------------------------------------------
// This methods runs in a separate thread to poll the database to see the
// number of records in the Q1 table, which is used to determine if we
// need to halt processing allowing the back-end system to catch-up
void POMDBThrottleAdapter::pollDatabase(void) throw (ThrottleException)
{
	// see how long we should wait until we refresh
	long refreshInterval = P2KGatewayConfiguration::getConfiguration().getRecordCountTimeout();

    // sit in a loop refreshing the database record count at a given interval 
	while (1)
	{
        Debugger::debugger << "Setting our database record count in the thread" << "\n";

		// refresh our database record count
		setDatabaseRecordCount();

        // make sure to set out flag saying our thread has been initialized
        m_threadInitialized = true;

        // pause until we need to refresh ourselves again
        this->sleep(refreshInterval);
 	}
}

