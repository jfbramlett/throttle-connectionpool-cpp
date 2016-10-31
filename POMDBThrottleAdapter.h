#ifndef __POMDBTHROTTLEADAPTER_H__
#define __POMDBTHROTTLEADAPTER_H__

#include <time.h>
#include "ThrottleAdapter.h"

//*******************************************************************
// The POM DB implementation of a throttle adapter. This class uses
// information stored in the POM database to determine whether throttling
// is required. (It uses the number of rows in the Q1 table to determine
// the throttling)
class POMDBThrottleAdapter : public ThrottleAdapter
{
public:
	POMDBThrottleAdapter(void);
	virtual ~POMDBThrottleAdapter(void);

	// exposed for the tread routine
	void setDatabaseRecordCount(void) throw (ThrottleException);

        void pollDatabase(void) throw (ThrottleException);

protected:	
	long databaseRecordCount(void) throw (ThrottleException);
	void databaseRecordCount(long count);

	virtual long getThrottlePeriod(void) throw (ThrottleException);
	virtual bool isThrottleRequired(void) throw (ThrottleException);

private:
	long m_dbRecordCount;
	time_t m_dbRecordCountTime;
    bool m_threadInitialized;
};

#endif
