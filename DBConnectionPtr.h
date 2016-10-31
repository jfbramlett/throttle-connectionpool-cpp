#ifndef __DBCONNECTIONPTR_H__
#define __DBCONNECTIONPTR_H__

#include "DebugUtil.h"
#include "DBConnection.h"
#include "DBException.h"
#include <string>

using namespace std;

//*******************************************************************
// Structure used as the underlying storage within the smart pointer 
// defined below. By using a structure like this it allows us to share
// this information across objects in order to maintain the reference 
// count
typedef struct REFERENCE
{
	DBConnection * ptr;
	unsigned short count;
    string pool;
} Reference;

//*******************************************************************
// Smart pointer wrapper around a database connection. This uses
// reference counting to know when the connection can be released back
// to the connection pool. The user should never create an instance of this
// object but will be provided one from the pool. This class will manage
// the lifecycle of the underlying connection automatically. Since it is 
// reference counted it can be passed around, assigned, etc.. and once
// everyone is done the underlying connection will be released by to the pool
// automatically
class DBConnectionPtr
{
public:
    DBConnectionPtr();
    DBConnectionPtr(DBConnection* ptr, string pool);
    DBConnectionPtr(const DBConnectionPtr& ptr);
    ~DBConnectionPtr();

    DBConnectionPtr& operator=(const DBConnectionPtr& ref);

    DBConnection* operator->();
    operator DBConnection*();

    DBConnection* getConnection();
    string getAssociatedPool() throw (DBException);

    // debug utility
    unsigned short getReferenceCount();

protected:
	void release();

    Reference* m_reference;
};

#endif

