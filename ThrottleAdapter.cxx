#include <iostream>
#include <time.h>
#ifdef _WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif
#include "ThrottleAdapter.h"
#include "DebugUtil.h"

using namespace std;

//-------------------------------------------------------------------
// Constructor - initialize the adapter
ThrottleAdapter::ThrottleAdapter(void)
{
   m_fWaitState = false;
}

//-------------------------------------------------------------------
// Destructor
ThrottleAdapter::~ThrottleAdapter(void)
{
}

//-------------------------------------------------------------------
// This method performs the actual throttling of the server by first
// checking if we need to throttle and then if so, setting ourselves
// in a wait state until we can turn ourselves back on
void ThrottleAdapter::throttle(void) throw (ThrottleException)
{
	// now run this against the database to see if we should block
	if (isThrottleRequired())
	{
        Debugger::debugger << "Throttling..." << "\n";

        // set that we are in a wait state
		m_fWaitState = true;

		// this is the actual throttling
        this->sleep(getThrottlePeriod());
	}

	// make sure t set our wait state to "not waiting"
	m_fWaitState = false;
}

//-------------------------------------------------------------------
// Sleep this process for the given number of seconds. This is the 
// throttling itself where we shut ourselves down allowing the back
// end to catch up
void ThrottleAdapter::sleep(long seconds)
{
#ifdef _WINDOWS
        Sleep(seconds * 1000);
#else
        // just a note, when using the UNIX 'sleep' (to sleep the process) API it 
        // was causing a lot of problem on the HP box so ended up implementing
        // this functionality by hand. This is not ideal below but was the best 
        // I could come up with
        time_t startTime;
        time_t currentTime;

        // basically we get a start-time and then loop getting the current time until
        // our sleep period has elapsed
        time(&startTime);
        do 
        {
            time(&currentTime);
        } while (difftime(currentTime, startTime) < seconds);
#endif
}

//-------------------------------------------------------------------
// Determines if our server is currently in a wait state
bool ThrottleAdapter::waitState(void) 
{
	return m_fWaitState;
}

