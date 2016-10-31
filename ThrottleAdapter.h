#ifndef __THROTTLEADAPTER_H__
#define __THROTTLEADAPTER_H__

#include "Throttle.h"
#include "IThrottleAdapter.h"

//*******************************************************************
// Base class for throttle adapters (optional - as long as an adapter
// implements the IThrottleAdapter then it will plug in, this class
// simply provides some common functionality that might be useful
// across multiple throttle adapters.
class ThrottleAdapter : public IThrottleAdapter
{
public:
	ThrottleAdapter(void);
	virtual ~ThrottleAdapter(void);

	virtual void throttle() throw (ThrottleException);
	virtual bool waitState();

protected:
	virtual bool isThrottleRequired() throw (ThrottleException) = 0;
   virtual long getThrottlePeriod() throw (ThrottleException) = 0;
	void sleep(long seconds);

private:
	bool m_fWaitState;
};

#endif
