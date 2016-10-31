//
//	Copyright (c) 2002 IONA Technologies PLC. All Rights Reserved.
//

#ifndef _DBPOOL_H_
#define _DBPOOL_H_

#include <iostream>
#include <queue>
#include <set>
#include <map>
#include <string>
#include "DBConnectionPtr.h"
#include "Mutex.h"

using namespace std;

class DBPool;

typedef queue<DBConnection*> DBConnectionQueue;
typedef set<void*> InUseConnections;
typedef map<string, DBPool*, less< string > > DBConnectionPools;


// define a function pointer that can be used to define the test for the database connection
typedef bool (*CONNECTIONTEST)(DBConnection* _test);


//*******************************************************************
// Manages a pool of database connections (configurable). It works
// by allowing people to check-out a connection for use and when
// done check the connection back in (handled through smart-pointers)
// A set of active connections is maintained within this class to allow
// for the fast hand-out of connections.
class DBPool 
{
  public :
    static DBPool& getInstance(const string& poolName = "") throw (DBException);
    
     ~DBPool();
 
	 DBConnectionPtr getConnection(void) throw (DBException);
     void releaseConnection(DBConnectionPtr& connection) throw (DBException);
     void registerTestFunction(CONNECTIONTEST test);

     size_t getAvailableConnections(void);
     size_t getInUseConnections(void);

protected:
    void initializePool(void);
    void cleanupPool(void);

    void createConnections(long numConnections);
    DBConnection* getConnectionFromPool(void) throw (DBException);
    void addInUseConnection(DBConnection* connection);

private:
    // constructor as private so only access is through static getInstance
    DBPool(const string& poolName);
    DBPool(void);

    bool testConnection(DBConnection* connection);

    DBConnectionQueue m_availablePool;
    InUseConnections m_inUsePool;
    string m_poolName;
    CONNECTIONTEST m_testFunction;      // our test function

    Mutex m_availablePoolMutex;
    Mutex m_inUsePoolMutex;

    // static initialization methods and our pool static variable
    static void initializePools(void);
    static DBConnectionPools* s_pools;
    static Mutex s_initializationMutex;

};

#endif

