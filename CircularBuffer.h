#pragma once
#include "Types.h"

template<typename T>
class circular_buffer
{
    private:
        T* m_array;
        ui32 m_size;
        ui32 m_front;
        ui32 m_back;
        circular_buffer() : m_array(nullptr) {};
        circular_buffer(const circular_buffer&) = delete;
        circular_buffer& operator=(const circular_buffer) = delete;
    public:
        circular_buffer(ui32 size) :
            m_size(size),
            m_array(nullptr),
            m_front(0), m_back(0)
        {
            m_array = new T[size];
        }
        ~circular_buffer()
        {   
            if(m_array)
                delete[] m_array;
        }
        void push_front(T& value)
        {
            m_array[m_front] = value;
            m_front--;
            m_front %= m_size;
            if (m_front == m_back)
            {
                m_back--;
                m_back %= m_size;
            }
        }
        void pop_front()
        {
            if (m_front == m_back)
                return;
            m_front++;
            m_front %= m_size;
        }
        const T& front()
        {
            return m_array[(m_front + 1)%m_size];
        }

        void push_back(T& value)
        {
            m_array[m_back] = value;
            m_back++;
            m_back %= m_size;
            if (m_front == m_back)
            {
                m_front++;
                m_front %= m_size;
            }
        }
        void pop_back()
        {
            if (m_front == m_back)
                return;
            m_back--;
            m_back %= m_size;
        }
        const T& back()
        {
            return m_array[(m_back - 1) % m_size];
        }

        bool empty()
        {
            return m_front == m_back;
        }
};
