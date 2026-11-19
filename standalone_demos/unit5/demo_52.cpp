#include <iostream>

int main()
{
  float * scan = new float[360]{};          // heap: lives until YOU free it
  scan[0] = 1.25f;
  std::cout << "scan[0] = " << scan[0] << '\n';
  delete[] scan;                            // new[] pairs with delete[]
  scan = nullptr;                           // so nobody uses the freed memory

  int * counter = new int{42};
  std::cout << "*counter = " << *counter << '\n';
}                                           // counter is gone, the int is not: a leak
