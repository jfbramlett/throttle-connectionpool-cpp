#ifndef __DEBUGUTIL_H__
#define __DEBUGUTIL_H__

#include <iostream>
#include <string>
using namespace std;

//*******************************************************************
// Simple utility class used to encapsulate writing debug messages out
// this is basically a wrapper around simple cout's but instead of
// embedding this output messages directly in the code I wanted to
// put them in a form that could be extended to log them to a file
// or some other output form or keep them simply as cout's
class Debugger
{
public:
    static Debugger debugger;
};

// inlined implementations of writing out the debug messages. In this case the
// messages are written to standard out (but could be extended to log to a file
// or some other output if necessary)
inline Debugger& operator<<(Debugger& debugger, const string& a) {cout << a; return debugger;};
inline Debugger& operator<<(Debugger& debugger, const char* a) {cout << a; return debugger;};
inline Debugger& operator<<(Debugger& debugger, int i) {cout << i; return debugger;};
inline Debugger& operator<<(Debugger& debugger, long l) {cout << l; return debugger;};

#endif

