#include "stack.h"


//constructor
template<typename T, int i>
MyStack<T, i>::MyStack() { /* ... */ }


//implementation of methods of the STACK class
template< typename T, int size > void MyStack< T, size >::push( const T item )
{
  stack[index] = item;
  index = (index + 1) % size;
  if (level < size)
  {
    level++;
  } 
};



template< typename T, int size > void MyStack< T, size >::rewrite_last( T value )
{ 
  int i = (index > 0) ? (index - 1) : ((level > 0)? level - 1 : 0) ; 
  stack[i] = value;
};

template< typename T, int size > T& MyStack< T, size >::sum( void )
{
  T rslt = 0;
  for (uint8_t i = 0; i < level; i++)
  {
    rslt += stack[i];
  }
  return rslt;
};

template< typename T, int size > double& MyStack< T, size >::average( void )
{
  double rslt = 0;
  for (uint8_t i = 0; i < level; i++)
  {
    rslt += stack[i];
  }
  return rslt /= (double)level;
};

template< typename T, int size > T& MyStack< T, size >::max( void )
{
  T max = stack[0];
  for (uint8_t i = 1; i < level; i++)
  {
    if(stack[i] > max)
      max = stack[i];
  }
  return max;
};
template< typename T, int size > T& MyStack< T, size >::min( void )
{
  T min = stack[0];
  for (uint8_t i = 1; i < level; i++)
  {
    if(stack[i] < min)
      min = stack[i];
  }
  return min;
};

//explicit instatiation 
template class MyStack<double, 4>;
template class MyStack<double, 10>;
template class MyStack<double, 6>;
template class MyStack<uint8_t, 8>;
template class MyStack<uint8_t, 5>;
template class MyStack<uint8_t, 6>;




