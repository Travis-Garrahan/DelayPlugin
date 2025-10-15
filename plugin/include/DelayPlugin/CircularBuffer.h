//
// Created by Travis Garrahan on 10/8/25.
//

#ifndef SIMPLE_DELAY_CIRCULARBUFFER_H
#define SIMPLE_DELAY_CIRCULARBUFFER_H
#include <type_traits>
#include <vector>
#include <cassert>
#include <juce_core/juce_core.h>


#endif //SIMPLE_DELAY_CIRCULARBUFFER_H

template<typename FloatType>
class CircularBuffer
{
    // make sure the wrong datatype is not used
    static_assert(std::is_floating_point_v<FloatType> == true, "template type must be float or double");

public:
    // size of buffer
    const size_t size;

    /*
    * creates CircularBuffer of size n
    * @param n: The buffer size, must be power of 2.
    * @param value: Value to fill the buffer with
    */
    explicit CircularBuffer(std::size_t n, FloatType value = FloatType()) :
        size(n), data(n, value), firstElement(0)
    {
        // ensure buffer size is power of 2:
        assert(n == juce::nextPowerOfTwo(n));
    }


    /*
    * Returns a copy of element at a given index
    * @param x: The index
    * @return Copy of the buffer element
    */
    FloatType operator()(size_t x) const
    {
        assert(x >= 0 && x < size);
        return data[mask(firstElement + x)];
    }

    /*
     * Returns reference to buffer element at the given index
     * starting from the end of the buffer
     * @param x: the index
     * @return a reference to the buffer element
    */
    FloatType &operator[](size_t x)
    {
        assert(x >= 0 && x < size);
        return data[mask(firstElement + x)];
    }

    /*
     * Returns a copy of the buffer element at the given index
     * @param x: the index
     * @return a copy of a buffer element
     */
    FloatType operator[](size_t x) const
    {
        return operator()(size - x - 1);
    }
    /*
    * Insert element at front of buffer, shifting out last element
    * Return last element to send to audio output
    */
    void shift(FloatType element)
    {
        auto pushed = operator()(0);
        push(element);
        return pushed;
    }

    /*
    * Insert element at front of buffer, shifting out last element
    * @param element: element to push into buffer
    */
    void push(FloatType element)
    {
        data[mask(firstElement++)] = element;
    }

    // Replace every element in buffer with default value
    void clear()
    {
        fill(FloatType());
    }

    // Replace every element in buffer with given value
    void fill(FloatType value)
    {
        std::fill(data.begin(), data.end(), value);
    }
private:
    // vector containing underlying data
    std::vector<FloatType> data;

    // index of current first element of buffer
    std::size_t firstElement;

    size_t mask(size_t val) const
    {
        /*
         * This is effectively the buffer's "wrapping" function
         * it always gives the same result as val % size, as long as size == power of 2
         * but without having to perform (potentially slow) division
         * modern compilers usually optimize this anyway if size == power of 2 (which it always is)
         * but this is more in line with how DSP code is usually written
         * and it's a neat fun trick
         */
        return val & (size - 1);
    }
};
