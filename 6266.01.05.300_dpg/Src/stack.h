#include "stdint.h"

//template for all stack 
template <typename T, int size> class MyStack
{  
  public:
    T stack[size];
    uint8_t index  = 0;
    uint8_t level  = 0;
    
    MyStack( void );
  
    void push(T item);
    void rewrite_last(T value);
//    T& last( void );
    T& sum( void );
    double& average( void );
    T& max( void );
    T& min( void );
   
};

// explicit instatiation for specific types
extern template class MyStack<double, 4>;
extern template class MyStack<double, 6>;
extern template class MyStack<double, 10>;
extern template class MyStack<uint8_t, 8>;
extern template class MyStack<uint8_t, 5>;
extern template class MyStack<uint8_t, 6>;
