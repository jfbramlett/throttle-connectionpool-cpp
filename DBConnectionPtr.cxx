#include "DBConnectionPtr.h"
#include "DBPool.h"

//-------------------------------------------------------------------
// This is a default constructor and should never be used directly
DBConnectionPtr::DBConnectionPtr()
{
    //Debugger::debugger << "In default constructor" << "\n";
    m_reference = NULL;
}

//-------------------------------------------------------------------
// Construct a new smart wrapper around the given database connection
// We assume ownership of the database connection until we are destroyed
DBConnectionPtr::DBConnectionPtr(DBConnection* DBConnectionPtr, string poolName)
{
    //Debugger::debugger << "Constructing from *" << "\n";

    m_reference = new Reference();
    m_reference->ptr = DBConnectionPtr;
    m_reference->count = 1;
    m_reference->pool = poolName;
}

//-------------------------------------------------------------------
// Copy constructor
DBConnectionPtr::DBConnectionPtr(const DBConnectionPtr& DBConnectionPtr)
{
    //Debugger::debugger << "Copy constructor" << "\n";

    m_reference = DBConnectionPtr.m_reference;
    m_reference->count = m_reference->count + 1;
}

//-------------------------------------------------------------------
// Destructor - clean-up ourselves
DBConnectionPtr::~DBConnectionPtr()
{
    //Debugger::debugger << "Destructor" << "\n";
    this->release();
}

//-------------------------------------------------------------------
// Overload assignment operator - allows us to assign this objects
// value from an existing smart pointer reference. In this case
// they will both point to the same underlying database connection
DBConnectionPtr& DBConnectionPtr::operator=(const DBConnectionPtr& ref)
{
    release();
    m_reference = ref.m_reference;
    m_reference->count = m_reference->count + 1;

    return *this;
}

//-------------------------------------------------------------------
// Overload of the '->' operator so you can invoke methods directly 
// on the underlying connection
DBConnection* DBConnectionPtr::operator->()
{
    return getConnection();
}

//-------------------------------------------------------------------
// Overload of the cast operator so you can invoke methods directly 
// on the underlying connection by casting to a DBConnection*
DBConnectionPtr::operator DBConnection*()
{
    return getConnection();
}

//-------------------------------------------------------------------
// Utility method to get the connection that this is the smart pointer
// for
DBConnection* DBConnectionPtr::getConnection(void)
{
    if (m_reference != NULL)
    {
        return m_reference->ptr;
    }

    return NULL;
}
//-------------------------------------------------------------------
// Release this connection back to our connection pool
void DBConnectionPtr::release()
{
    if (m_reference != NULL)
    {
        m_reference->count = m_reference->count - 1;
        if (m_reference->count == 0)
        {
            Debugger::debugger << "Reference count is 0 - releasing connection" << "\n";
            DBPool::getInstance(m_reference->pool).releaseConnection(*this);
            delete m_reference;
        }
        else 
        {
            Debugger::debugger << "Just decrementing reference count" << "\n";
        }
    }
}

//-------------------------------------------------------------------
// Return the pool the underlying connection is associated with
string DBConnectionPtr::getAssociatedPool(void) throw (DBException)
{
    // make sure we have an underlying connection before doing anything
    // with our pointer
    if (m_reference != NULL)
    {
        return m_reference->pool;
    }

    // if we are here then our underlying reference is null meaning this is ptr
    // is not associated with a connection (which should never happen
    throw DBException("Object not initialized - not associated with any connection!");
}

//-------------------------------------------------------------------
// Debugging utility to get the reference count on our underlying connection
// we are using reference couting to manage the object lifecycle so we know
// when we can release the connection back to the pool
unsigned short DBConnectionPtr::getReferenceCount(void)
{
    unsigned short result = 0;

    // make sure we have an underlying reference
    if (m_reference != NULL)
    {
        result = m_reference->count;
    }

    return result;
}

