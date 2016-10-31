#ifndef __P2KGATEWAYCONFIGURATION_H__
#define __P2KGATEWAYCONFIGURATION_H__

#include <string>
#include <iostream>
#include "PropertyConfigurationUtility.h"

using namespace std ;

//*******************************************************************
// Wrapper around a property file that provides specific 'get' methods
// for various configuration parameters. This class is a convenience and
// is not required to use the configuration classes but the more 'friendly'
// nature of the API and the encapsulation of the constants that represent
// the keys in the configuration seems to add value to the system design.
class P2KGatewayConfiguration : public PropertyConfigurationUtility
{
public:
	// factory method to obtain instance
	static P2KGatewayConfiguration& getConfiguration() throw (ConfigurationException);
	static string getConfigFile() throw (ConfigurationException);

	virtual ~P2KGatewayConfiguration(void);

	long getThrottlePeriod(void) throw (ConfigurationException);
	long getRecordThreshold(void) throw (ConfigurationException);
	long getRecordCountTimeout(void) throw (ConfigurationException);
	bool isThrottleThreaded(void) throw (ConfigurationException);

protected:
	// make protected so only access is through factory method
	P2KGatewayConfiguration(void);

	// singleton instance
	static P2KGatewayConfiguration s_adapter;
	const static long LINE_SIZE = 50;

    // the following represent constants for the keys used to access information
    // in the configuration
    const static string THROTTLE_PERIOD_KEY;
	const static string MAX_RECORD_TIMEOUT_KEY;
	const static string MAX_RECORDS_KEY;
	const static string THROTTLE_THREADED_KEY;
	const static string THROTTLE_THREADED_YES;
	const static string THROTTLE_THREADED_NO;
};

#endif

