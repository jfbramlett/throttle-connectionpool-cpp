#ifndef __ITHROTTLEADAPTER_H__
#define __ITHROTTLEADAPTER_H__

//*******************************************************************
// Base interface for all throttle adapters. This interface allows us
// to implement different throttle adapters based on system requirements
// without changing the throttler itself (the adapter plugs in to the
// throttler through the use of a factory). Currently, we have single
// throttler which is based around the POM database
class IThrottleAdapter
{
public:
	virtual void throttle() = 0;
	virtual bool waitState() = 0;
};

#endif
