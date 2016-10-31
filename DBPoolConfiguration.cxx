#include "DBPoolConfiguration.h"
#include <fstream>
#include <iostream>
#include "DebugUtil.h"

using namespace std ;

//-------------------------------------------------------------------
// Initialize our static constants
const string DBPoolConfiguration::INITIAL_POOL_SIZE_KEY("initialPoolSize");
const string DBPoolConfiguration::MAX_POOL_SIZE_KEY("maxPoolSize");
const string DBPoolConfiguration::POOL_INCREMENT_KEY("poolIncrement");
const string DBPoolConfiguration::WAIT_FOR_CONNECTION_KEY("waitForConnection");
const string DBPoolConfiguration::CONNECTION_WAIT_PERIOD_KEY("connectionWaitPeriod");
const string DBPoolConfiguration::VALIDATE_CONNECTION_KEY("validateConnectionBeforeReturning");
const string DBPoolConfiguration::POOL_NAMES_KEY("pools");
const string DBPoolConfiguration::CONNECTION_TYPE_KEY("connectionType");
const string DBPoolConfiguration::DEFAULT_POOL_KEY("defaultPool");
const string DBPoolConfiguration::USERID_KEY("userId");
const string DBPoolConfiguration::PWD_KEY("password");
const string DBPoolConfiguration::CONNECTSTRING_KEY("connectString");

const string DBPoolConfiguration::DBPOOL_CONFIGURATION_YES("Y");
const string DBPoolConfiguration::DBPOOL_CONFIGURATION_NO("N");

const string DBPoolConfiguration::SEPARATOR(".");


DBPoolConfiguration DBPoolConfiguration::s_adapter;	// singleton instance initialization

//-------------------------------------------------------------------
// Singleton accessor method - used to obtain the one and only
// one instance of the ThrottleAdapterFactory
DBPoolConfiguration& DBPoolConfiguration::getConfiguration() throw (ConfigurationException)
{
	// if we have not initialized our adapter then we must do so
	if (!s_adapter.initialized()) 
	{
		s_adapter.loadFromFile(DBPoolConfiguration::getConfigFile());
	}

	return s_adapter;
}

//-------------------------------------------------------------------
// Utility to get the fully qualified path and file to the config file
// (static)
string DBPoolConfiguration::getConfigFile() throw (ConfigurationException)
{
	string fullyQualifiedConfigFile;

	// get our path from the environment
	char* configFile = getenv("DBPOOL_CONFIG");

    // if the environment variable does not exist we are out of luck so throw an exception
	if (configFile == NULL)
	{
        Debugger::debugger << "Config file does not exist!!" << "\n";		
        throw ConfigurationException("Configuration file environment variable 'DBPOOL_CONFIG' not set!");
	}
	else
	{
		fullyQualifiedConfigFile = configFile;
	}

    Debugger::debugger << "Using config file: " << fullyQualifiedConfigFile << "\n";

	return fullyQualifiedConfigFile;
}

//-------------------------------------------------------------------
// Constructor
DBPoolConfiguration::DBPoolConfiguration(void)
{
}

//-------------------------------------------------------------------
// Destructor
DBPoolConfiguration::~DBPoolConfiguration(void)
{
}

//-------------------------------------------------------------------
// Configuration value for the initial size of our connection pool
long DBPoolConfiguration::getInitialPoolSize(const string&  pool) throw (ConfigurationException)
{
	string initialPoolSize = this->get(pool + SEPARATOR + INITIAL_POOL_SIZE_KEY);
	return atol(initialPoolSize.c_str());
}

//-------------------------------------------------------------------
// Configuration value for the maximum number of connections we will have
// in the pool
long DBPoolConfiguration::getMaxPoolSize(const string&  pool) throw (ConfigurationException)
{
	string maxPoolSize = this->get(pool + SEPARATOR + MAX_POOL_SIZE_KEY);
	return atol(maxPoolSize.c_str());
}

//-------------------------------------------------------------------
// Configuration value defining when we need to increase the size of the
// pool (up to max) how many new connections should we create at a time
long DBPoolConfiguration::getPoolIncrement(const string&  pool) throw (ConfigurationException)
{
	string poolIncrement = this->get(pool + SEPARATOR + POOL_INCREMENT_KEY);
	return atol(poolIncrement.c_str());
}

//-------------------------------------------------------------------
// Return a bool flag based on a config entry that indicates if 
// we should wait for a connection when one is not immediately
// available
bool DBPoolConfiguration::waitForConnection(const string&  pool) throw (ConfigurationException)
{
    // wrap this in a try-catch block to handle the case when this is not
    // specified. If it is not specified we use our default setting which
    // is to wait for the connection
    try {
	    string waitForConnection = this->get(pool + SEPARATOR + WAIT_FOR_CONNECTION_KEY);
	    if (waitForConnection == DBPOOL_CONFIGURATION_YES) 
	    {
		    return true;
	    }
	    else 
	    {
		    return false;
	    }
    }
    catch (ConfigurationException ce)
    {
        // we are catching this exception because if this value is not specified
        // we are using our default waiting
        ;
    }

    // default setting is to wait for a connection to become available
    return true;
}

//-------------------------------------------------------------------
// Configuration value for how long to wait for a connection. A value
// of -1 indicates an infinite wait
long DBPoolConfiguration::getConnectionWaitPeriod(const string&  pool) throw (ConfigurationException)
{
    string poolIncrement = this->get(pool + SEPARATOR + CONNECTION_WAIT_PERIOD_KEY);
	return atol(poolIncrement.c_str());
}

//-------------------------------------------------------------------
// Configuration value that says whether or not the connection should be
// validated prior to returning it. If yes, the pool will verify the connection
// is valid before returning it and if not valid will try to obtain another connection
// until the pool is exhausted or a valid connection is obtained
bool DBPoolConfiguration::validateConnectionBeforeReturning(const string&  pool) throw (ConfigurationException)
{
    // wrap this in a try-catch block to handle the case when this is not
    // specified. If it is not specified we use our default setting which
    // is to validate the connection
    try {
	    string validateConnection = this->get(pool + SEPARATOR + VALIDATE_CONNECTION_KEY);
	    if (validateConnection == DBPOOL_CONFIGURATION_YES) 
	    {
		    return true;
	    }
	    else 
	    {
		    return false;
	    }
    }
    catch (ConfigurationException ce)
    {
        // we are catching this exception because if this value is not specified
        // we are using our default which is to validate
        ;
    }

    // default setting is to validate the connection
    return true;
}

//-------------------------------------------------------------------
// Configuration value that identifies the connectio type for the given pool
string DBPoolConfiguration::getConnectionType(const string&  pool) throw (ConfigurationException)
{
	return this->get(pool + SEPARATOR + CONNECTION_TYPE_KEY);
}

//-------------------------------------------------------------------
// Returns the default pool (the pool to use when none is specified
string DBPoolConfiguration::getDefaultPool()  throw (ConfigurationException)
{
    return this->get(DEFAULT_POOL_KEY);
}

//-------------------------------------------------------------------
// Gets the list of pools that define the set of configured pools that
// should be initialized. This list is a comma separated list of pool
// names
vector<string> DBPoolConfiguration::getPools() throw (ConfigurationException)
{
    string pools = get(POOL_NAMES_KEY);

    // now we have our list of pools we must string apart in to a vector
    // since this list may contain multiple pools
    vector<string> poolNames;

    size_t pos = pools.find(POOL_NAMES_SEPARATOR);
    while (pos != string::npos) 
	{
		string name = pools.substr(0, pos);
		pools = pools.substr(pos+1, pools.length() - name.length());

        poolNames.push_back(name);

        pos = pools.find(POOL_NAMES_SEPARATOR);
	}

    // take our last guys and add him (since he won't have a trailing ','
    poolNames.push_back(pools);

    // return our list of pools
    return poolNames;
}

//-------------------------------------------------------------------
// Gets the user id that should be used when connecting to the database
// for the given pool
string DBPoolConfiguration::getUserId(const string& pool) throw (ConfigurationException)
{
    return get(pool + SEPARATOR + USERID_KEY);   
}

//-------------------------------------------------------------------
// Gets the password for the user id that should be used when connecting 
// to the database for the given pool
string DBPoolConfiguration::getPassword(const string& pool) throw (ConfigurationException)
{
    return get(pool + SEPARATOR + PWD_KEY);
}

//-------------------------------------------------------------------
// Gets the connection string that should be used when connecting to the database
// for the given pool
string DBPoolConfiguration::getConnectString(const string& pool) throw (ConfigurationException)   
{
    return get(pool + SEPARATOR + CONNECTSTRING_KEY);   
}

