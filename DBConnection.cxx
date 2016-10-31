#include "DBConnection.h"
#include "DebugUtil.h"


//-------------------------------------------------------------------
// Constructor - initialize our connection
DBConnection::DBConnection(void) 
        : m_invalidConnection(false)
{
}

//-------------------------------------------------------------------
// Destructor
DBConnection::~DBConnection(void)
{
    Debugger::debugger << "Destructing connection" << "\n";
}

//-------------------------------------------------------------------
// Tests the connection to insure it is valid
bool DBConnection::testConnection(void) 
{ 
    return true;
}

//-------------------------------------------------------------------
// Setter for setting the flag indicating if this connection is valid
// or not
void DBConnection::invalidConnection(bool flag) 
{ 
    m_invalidConnection = flag;
}

//-------------------------------------------------------------------
// Return a flag indicating if this connection is valid
bool DBConnection::invalidConnection(void) 
{
    return m_invalidConnection;
}

