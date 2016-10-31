#ifndef __THROTTLE_H__
#define __THROTTLE_H__

#include <string>

using namespace std;

//*******************************************************************
// Exception class for when things go wrong in the throttling process
class ThrottleException
{
  public:
    ThrottleException(string theMessage) : message(theMessage) {};
	string message;
};


//*******************************************************************
// The outer-level controller for the throttling process. This class
// delegates the actual throttle implementation to an instance of
// IThrottleAdapter - but this wrapper layer simply insulates the developers
// from possible changes to the underlying adapter.
class Throttle 
{
public:
	Throttle(void);
	virtual ~Throttle(void);

	// Our one method to throttle the server to prevent 
	// the processing of incoming requests
	void throttle() throw (ThrottleException);
};

#endif
