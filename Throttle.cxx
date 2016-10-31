#include "Throttle.h"
#include "ThrottleAdapterFactory.h"

#include "DebugUtil.h"

Debugger Debugger::debugger;


//-------------------------------------------------------------------
// Constructor - intialize our service
Throttle::Throttle(void)
{
}

//-------------------------------------------------------------------
// Destructor - do any clean-up required
Throttle::~Throttle(void)
{
}

//-------------------------------------------------------------------
// This method performs the actually throttling of the server by blocking
// if need be until the system is back in a state ready to process additional
// requests. This method doesn't actually do the work but makes a request on
// an adapter that is responsible for an environment specific throttle
void Throttle::throttle() throw (ThrottleException)
{
	// get our throttle adapter from the factory (which is a singleton)
	IThrottleAdapter& adapter = ThrottleAdapterFactory::getInstance().getAdapter();

	// now tell the adapter to throttle (the adapter will be some implementation
	// specific component that nows how to throttle within a specific environment
	adapter.throttle();
}
