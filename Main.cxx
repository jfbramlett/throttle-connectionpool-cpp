#include "Throttle.h"
#include <time.h>
#include <iostream>
#include "DBConnectionPtr.h"

#ifdef _WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "DBPool.h"
#include "DBPoolTestCase.h"
#include "TextRunner.h"

using namespace std;

void throttle();
void testPassByReference(DBConnectionPtr& ptr);
void testPassByValue(DBConnectionPtr ptr);


int main(int argc, char** argv) 
{
    //TestTemplate<DBPoolTestCase> myTest(DBPoolTestCase::test1, "Test 1");
    TextRunner<DBPoolTestCase> myTest;

    myTest.run();

/*
    // DBPOOL TEST STUFF

    DBPoolTestCase testCase;

    if (testCase.run())
    {
        cout << "All test passed" << "\n";
    }
    else 
    {
        cout << "A test failed" << "\n";
    }
*/

 //   throttle();
}

void testDBConnectionPtr()
{
    // get a connection
    DBConnectionPtr connection = DBPool::getInstance().getConnection();

    // test passing it by reference
    testPassByReference(connection);

    // test passing by value
    testPassByValue(connection);
}

//-------------------------------------------------------------------
// Test routine for smart point pass by reference
void testPassByReference(DBConnectionPtr& ptr)
{
    cout << "In method passed by reference" << "\n";
    cout << "Reference count " << ptr.getReferenceCount() << "\n";
}

//-------------------------------------------------------------------
// Test routine for smart point pass by value
void testPassByValue(DBConnectionPtr ptr)
{
    cout << "In method passed by value" << "\n";
    cout << "Reference count " << ptr.getReferenceCount() << "\n";
}

void throttle()
{

	Throttle throttle;
	throttle.throttle();

	// now throttle again immediately
	throttle.throttle();

	// now wait a period of time
#ifdef _WINDOWS
    Sleep(7 * 1000);
#else
    time_t start;
    time_t current;
    
    time(&start);
    do {
        time(&current);
    } while (difftime(current, start) < 7);
//    sleep(7);
#endif

	throttle.throttle();
}

