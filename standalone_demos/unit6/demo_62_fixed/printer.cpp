#include "printer.hpp"
#include <iostream>

template <typename T>
void log_twice(const T & value)       // ...defined here, where nobody uses it
{
  std::cout << value << ' ' << value << '\n';
}
template void log_twice<int>(const int &);   // compile the int version here
