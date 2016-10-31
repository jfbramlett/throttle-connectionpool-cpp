#ifndef __DBCONNECTIONFACTORY_H__
#define __DBCONNECTIONFACTORY_H__

#include "DBConnection.h"
#include <string>

using namespace std;

//*******************************************************************
// Factory responsible for creating database connections. Each connection
// this factory creates will be a descendant of DBConnection - allowing
// for a single "interface" association. Creation happens by providing this
// factory a connection type and based on the connection type it
// knows the specific subclass to create. 
//
// This class is a singleton
class DBConnectionFactory
{
public:
    static DBConnectionFactory& getInstance();
    ~DBConnectionFactory(void);

    virtual DBConnection* createConnection(const string& poolName);
    virtual void releaseConnection(DBConnection* connection);

    static const string ORACLE_CONNECTION;
private:
    DBConnectionFactory(void);

    static DBConnectionFactory* s_instance;

};

#endif

