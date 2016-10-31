#ifndef __DBEXCEPTION_H__
#define __DBEXCEPTION_H__

#include <string>

using namespace std;


//*******************************************************************
// Exception class used when there is a problem with the database
class DBException
{

  public:
	string message;

  public: 
    DBException(string theMessage) : message(theMessage) {
	};

};


#endif

