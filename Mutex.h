#ifndef __MUTEX_H__
#define __MUTEX_H__

//*******************************************************************
// Simple mutex class used to insure thread-safety within a class
// when the mutex is "locked" then all threads trying to access the
// same functionality will be blocked until the mutex releases
class Mutex
{
public:
    Mutex() : m_lock(false) {};
    ~Mutex() {};

    void lock() { while (m_lock) {;} m_lock = true; };
    void unlock() { m_lock = false;};
    bool tryLock() { if (m_lock == true) return 0; else { m_lock=true; return 1;}};

private:
    bool m_lock;
};

//*******************************************************************
// This is the mutex handler that manages the lock/unlock automatically
// so this is the class that is exposed within the implementation. You
// construct an instance of this class and provide it your mutex
// (which should be a member variable). Upon construction the mutex is
// locked and when this object goes out of scope it releases the lock
// so the user doesn't have to write any special handling code
class MutexHandler
{
public:
    MutexHandler(Mutex& mutex) : m_mutex(mutex) { m_mutex.lock();};
    ~MutexHandler() { m_mutex.unlock();};
private:
    Mutex& m_mutex;
};
#endif
