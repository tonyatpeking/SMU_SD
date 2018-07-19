#pragma once
#include <mutex>
#include <queue>

template <typename T>
class ThreadSafeQueue
{
public:
    void Push( T const &v )
    {
        m_lock.lock();
        m_data.push( v );
        m_lock.unlock();
    }


    // return if it succeeds
    bool Pop( T *out_v )
    {
        m_lock.lock();

        bool hasItem = !m_data.empty();
        if( hasItem )
        {
            *out_v = m_data.front();
            m_data.pop();
        }

        m_lock.unlock();
        return hasItem;
    }

public:
    std::queue<T> m_data;
    std::mutex m_lock;
};