//
// Created by Travis Garrahan on 10/8/25.
//

#ifndef SIMPLE_DELAY_CIRCULARBUFFER_H
#define SIMPLE_DELAY_CIRCULARBUFFER_H
#include <type_traits>
#endif //SIMPLE_DELAY_CIRCULARBUFFER_H

template<typename FloatType>
class CircularBuffer
{
    // make sure the wrong datatype is not used
    static_assert(std::is_floating_point<FloatType>::value == true, "template type must be float or double");
public:


private:
    int read = 0;
    int write;

};
