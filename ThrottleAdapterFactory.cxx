#include "stddef.h"
#include "ThrottleAdapterFactory.h"
#include "POMDBThrottleAdapter.h"

//-------------------------------------------------------------------
// Initialize our singleton instance of the factory - this insures
// that we have one and only one instance of this factory class
ThrottleAdapterFactory ThrottleAdapterFactory::s_adapter;

//-------------------------------------------------------------------
// Singleton accessor method - used to obtain the one and only
// one instance of the ThrottleAdapterFactory
ThrottleAdapterFactory& ThrottleAdapterFactory::getInstance() 
{
	return s_adapter;
}

//-------------------------------------------------------------------
// Get a reference to the adapter that should be used by the service
IThrottleAdapter& ThrottleAdapterFactory::getAdapter() 
{
	// We want to lazy load the adapter since we don't want to incur 
	// the expense and overhead of creating an unnecessary object
	// so if we haven't created the object then create it
	if (m_adapter == NULL) 
	{
		m_adapter = new POMDBThrottleAdapter();
	}

	// return it out as a reference so we maintain "ownership"
	// so when it comes time to cleanup we are ready to go
	return *m_adapter;
}

//-------------------------------------------------------------------
// Constructor for the factory - do whatever needs to be done
// when this class is estantiated
ThrottleAdapterFactory::ThrottleAdapterFactory(void)
{
    // initialize our underlying adapter so we don't create an instance
    // until we actually need it
	m_adapter = NULL;
}

//-------------------------------------------------------------------
// Destructor - time to clean-up!
ThrottleAdapterFactory::~ThrottleAdapterFactory(void)
{
	// since we newed up an instance of the adapter, 
	// we must now make sure to delete it
	if (m_adapter != NULL) 
	{
		delete m_adapter;
	}
}
