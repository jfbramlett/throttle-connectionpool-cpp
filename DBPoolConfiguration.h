#ifndef __P2KGATEWAYCONFIGURATION_H__
#define __P2KGATEWAYCONFIGURATION_H__

#include <string>
#include <iostream>
#include <vector>
#include "PropertyConfigurationUtility.h"

using namespace std ;

//*******************************************************************
// Wrapper around a property file that provides specific 'get' methods
// for various configuration parameters. This class is a convenience and
// is not required to use the configuration classes but the more 'friendly'
// nature of the API and the encapsulation of the constants that represent
// the keys in the configuration seems to add value to the system design.
class DBPoolConfiguration : public PropertyConfigurationUtility
{
public:
	// factory method to obtain instance
	static DBPoolConfiguration& getConfiguration() throw (ConfigurationException);
	static string getConfigFile() throw (ConfigurationException);

	virtual ~DBPoolConfiguration(void);

    // configuration options for the pool size
    long getInitialPoolSize(const string&  pool) throw (ConfigurationException);
    long getMaxPoolSize(const string&  pool) throw (ConfigurationException);
    long getPoolIncrement(const string&  pool) throw (ConfigurationException);
    bool waitForConnection(const string&  pool) throw (ConfigurationException);
    long getConnectionWaitPeriod(const string& pool) throw (ConfigurationException);
    bool validateConnectionBeforeReturning(const string& pool) throw (ConfigurationException);
    string getConnectionType(const string&  pool) throw (ConfigurationException);
    string getUserId(const string& pool) throw (ConfigurationException);
    string getPassword(const string& pool) throw (ConfigurationException);
    string getConnectString(const string& pool) throw (ConfigurationException);    
    vector<string> getPools() throw (ConfigurationException);
    string getDefaultPool() throw (ConfigurationException);

    // this is the constant used to indicate our wait period is infinity
    // we will wait for a connection until one is available
    const static long CONNECTION_WAIT_PERIOD_INFINITE = -1;

protected:
	// make protected so only access is through factory method
	DBPoolConfiguration(void);

	// singleton instance
	static DBPoolConfiguration s_adapter;
	const static long LINE_SIZE = 50;
    const static string SEPARATOR;

    // the following represent constants for the keys used to access information
    // in the configuration
    const static string INITIAL_POOL_SIZE_KEY;
	const static string MAX_POOL_SIZE_KEY;
	const static string POOL_INCREMENT_KEY;
    const static string WAIT_FOR_CONNECTION_KEY;
    const static string CONNECTION_WAIT_PERIOD_KEY;
    const static string VALIDATE_CONNECTION_KEY;
    const static string POOL_NAMES_KEY;
    const static string CONNECTION_TYPE_KEY;
    const static string DEFAULT_POOL_KEY;
    const static char POOL_NAMES_SEPARATOR = ',';
    const static string USERID_KEY;
    const static string PWD_KEY;
    const static string CONNECTSTRING_KEY;
    
    
    // additional constants used for value comparisons
	const static string DBPOOL_CONFIGURATION_YES;
	const static string DBPOOL_CONFIGURATION_NO;

};

#endif

