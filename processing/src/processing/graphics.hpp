#ifndef _PROCESSING_INCLUDE_GRAPHICS_HPP_
#define _PROCESSING_INCLUDE_GRAPHICS_HPP_

#include <processing/processing.hpp>
#include <processing/renderer.hpp>

#include <stack>

namespace processing
{
    template <typename T>
    class NeverEmptyStack
    {
    public:
        explicit NeverEmptyStack(const T& initialValue);
        void push(const T& value);
        void pop();
        void reset(const T& value);
        T& peek();

        usize size() const;

    private:
        T m_defaultValue;
        std::stack<T> m_values;
    };
} // namespace processing

namespace processing
{
    void initGraphics(u32 width, u32 height);
    void beginDraw();
    void endDraw(u32 width, u32 height);
} // namespace processing

#endif // _PROCESSING_INCLUDE_GRAPHICS_HPP_

#ifndef _PROCESSING_INCLUDE_GRAPHICS_INL_
#define _PROCESSING_INCLUDE_GRAPHICS_INL_

namespace processing
{
    template <typename T>
    NeverEmptyStack<T>::NeverEmptyStack(const T& initialValue)
        : m_defaultValue(initialValue),
          m_values()
    {
    }

    template <typename T>
    void NeverEmptyStack<T>::push(const T& value)
    {
        m_values.push(value);
    }

    template <typename T>
    void NeverEmptyStack<T>::pop()
    {
        if (not m_values.empty())
        {
            m_values.pop();
        }
    }

    template <typename T>
    void NeverEmptyStack<T>::reset(const T& value)
    {
        while (not m_values.empty())
        {
            m_values.pop();
        }

        m_defaultValue = value;
    }

    template <typename T>
    T& NeverEmptyStack<T>::peek()
    {
        return m_values.empty() ? m_defaultValue : m_values.top();
    }

    template <typename T>
    usize NeverEmptyStack<T>::size() const
    {
        return m_values.size();
    }
} // namespace processing

#endif // _PROCESSING_INCLUDE_GRAPHICS_INL_
