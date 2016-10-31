#ifndef __THROTTLEADAPTERFACTORY_H__
#define __THROTTLEADAPTERFACTORY_H__

#include "IThrottleAdapter.h"

//*******************************************************************
// Singleton. This is the factory that is used to obtain instances of the 
// abstract class IThrottleAdapter. This pattern allows us to have multiple
// implementations of IThrottleAdapter that can be plugged in to our throttling
// process since our Throttle does not directly construct the underlying adapter
// (instead it delegates to this factory).
class ThrottleAdapterFactory
{
public:
	static ThrottleAdapterFactory& getInstance();
	IThrottleAdapter& getAdapter();

	virtual ~ThrottleAdapterFactory(void);

protected:
	ThrottleAdapterFactory(void);

private:
	static ThrottleAdapterFactory s_adapter;
	IThrottleAdapter* m_adapter;
};

#endif
