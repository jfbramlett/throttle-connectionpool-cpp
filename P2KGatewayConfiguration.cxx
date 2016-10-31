#include "P2KGatewayConfiguration.h"
#include <fstream>
#include <iostream>
#include "DebugUtil.h"

using namespace std ;

//-------------------------------------------------------------------
// Initialize our static constants
const string P2KGatewayConfiguration::THROTTLE_PERIOD_KEY("throttlePeriod");				// key within config    
const string P2KGatewayConfiguration::MAX_RECORDS_KEY("maxRecords");	                    // key within config
const string P2KGatewayConfiguration::MAX_RECORD_TIMEOUT_KEY("maxRecordTimeout");	        // key within config
const string P2KGatewayConfiguration::THROTTLE_THREADED_KEY("throttleThreaded");            // key within config
const string P2KGatewayConfiguration::THROTTLE_THREADED_YES("Y");                           // constant
const string P2KGatewayConfiguration::THROTTLE_THREADED_NO("N");                            // constant

// initialize our static instance
P2KGatewayConfiguration P2KGatewayConfiguration::s_adapter;				                    // singleton instance initialization

//-------------------------------------------------------------------
// Singleton accessor method - used to obtain the one and only
// one instance of the ThrottleAdapterFactory
P2KGatewayConfiguration& P2KGatewayConfiguration::getConfiguration() throw (ConfigurationException)
{
	// if we have not initialized our adapter then we must do so
	if (!s_adapter.initialized()) 
	{
		s_adapter.loadFromFile(P2KGatewayConfiguration::getConfigFile());
	}

	return s_adapter;
}

//-------------------------------------------------------------------
// Utility to get the fully qualified path and file to the config file
// (static)
string P2KGatewayConfiguration::getConfigFile() throw (ConfigurationException)
{
	string fullyQualifiedConfigFile;

	// get our path from the environment
	char* configFile = getenv("P2KGATEWAY_CONFIG");

    // if the environment variable does not exist we are out of luck so throw an exception
	if (configFile == NULL)
	{
        Debugger::debugger << "Config file does not exist!!" << "\n";		
        throw ConfigurationException("Configuration file environment variable 'P2KGATEWAY_CONFIG' not set!");
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
P2KGatewayConfiguration::P2KGatewayConfiguration(void)
{
}

//-------------------------------------------------------------------
// Destructor
P2KGatewayConfiguration::~P2KGatewayConfiguration(void)
{
}

//-------------------------------------------------------------------
// Get the value from the config entry for the throttle period
long P2KGatewayConfiguration::getThrottlePeriod(void) throw (ConfigurationException)
{
	string sleepVal = this->get(THROTTLE_PERIOD_KEY);
	return atol(sleepVal.c_str());
}

//-------------------------------------------------------------------
// Get the record threshold used to determine when we should throttle
long P2KGatewayConfiguration::getRecordThreshold(void) throw (ConfigurationException)
{
	string maxRecordsVal = this->get(MAX_RECORDS_KEY);
	return atol(maxRecordsVal.c_str());
}

//-------------------------------------------------------------------
// Determine if we are running the throttle checking in a separate thread
// or in the main flow
bool P2KGatewayConfiguration::isThrottleThreaded(void) throw (ConfigurationException)
{
    try {
	    string throttleThreaded = this->get(THROTTLE_THREADED_KEY);
	    if (throttleThreaded == THROTTLE_THREADED_YES) 
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
        // we are using our default of not threaded
        ;
    }

    // default setting is not threaded
    return false;
}

//-------------------------------------------------------------------
// Gets the period of time the between database queries for determining
// the maximum number of records in the DB
long P2KGatewayConfiguration::getRecordCountTimeout() throw (ConfigurationException)
{
	string timeoutVal = this->get(MAX_RECORD_TIMEOUT_KEY);
	return atol(timeoutVal.c_str());
}

