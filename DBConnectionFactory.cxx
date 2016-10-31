#include "DBConnectionFactory.h"
#include "DebugUtil.h"
#include "OracleConnection.h"
#include "DBPoolConfiguration.h"

DBConnectionFactory* DBConnectionFactory::s_instance;

const string DBConnectionFactory::ORACLE_CONNECTION("Oracle");


//-------------------------------------------------------------------
// Constructor - this is private so to insure we use our singleton
// 'getInstance' method
DBConnectionFactory::DBConnectionFactory(void)
{
}

//-------------------------------------------------------------------
// Destructor - clean-up the factory
DBConnectionFactory::~DBConnectionFactory(void)
{
}

//-------------------------------------------------------------------
// Gets our singleton instance
DBConnectionFactory& DBConnectionFactory::getInstance(void)
{
    // if we haven't created our singleton instance then create it
    if (s_instance == NULL)
    {
        s_instance = new DBConnectionFactory();
    }

    // return our singleton instance as a reference
    return *s_instance;
}

//-------------------------------------------------------------------
// Gets a database connection for the given pool configuration
DBConnection* DBConnectionFactory::createConnection(const string& pool)
{
    string connectionType = DBPoolConfiguration::getConfiguration().getConnectionType(pool);
    
    if (connectionType == ORACLE_CONNECTION) 
    {
        string userid = DBPoolConfiguration::getConfiguration().getUserId(pool);
        string password = DBPoolConfiguration::getConfiguration().getPassword(pool);
        string connectString = DBPoolConfiguration::getConfiguration().getConnectString(pool);
        return new OracleConnection(userid, password, connectString);
    }
    else
    {
        return NULL;    // unsupport connection type
    }
}

//-------------------------------------------------------------------
// Cleans up a connection
void DBConnectionFactory::releaseConnection(DBConnection* connection)
{
    delete connection;
}

