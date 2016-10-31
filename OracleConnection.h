#ifndef __ORACLECONNECTION_H__
#define __ORACLECONNECTION_H__

#include "DBConnection.h"
#include <string>

#ifndef __NO_DB__
#include <occi.h>
using namespace oracle::occi;
#else
typedef long Connection;
typedef long Environment;
#endif

using namespace std;

class OracleConnection : public DBConnection
{
public:
    OracleConnection(const string& userid, const string& password, const string& connectString);
    virtual ~OracleConnection();

    Connection* getConnection();

private:

   Connection* m_connection;    
   Environment* m_environment;

   void getConnection(const string& userid, const string& password, const string& connectString);
   void releaseConnection();
};


#endif


