#include <algorithm>
#include <cstddef>
#include <iostream>

class ScanBuffer
{
public:
  explicit ScanBuffer(std::size_t size)
  : size_{size}, ranges_{new float[size]{}}     // allocate the array on the heap
  {}

  ~ScanBuffer()
  {
    delete[] ranges_;                           // free it when the object dies
  }

  ScanBuffer(const ScanBuffer & other)                  // copy constructor: a deep copy
  : size_{other.size_}, ranges_{new float[other.size_]}
  {
    std::copy(other.ranges_, other.ranges_ + other.size_, ranges_);
  }

  ScanBuffer & operator=(const ScanBuffer & other)      // copy assignment: a = b
  {
    if (this != &other) {                               // a = a must not free its own data
      float * fresh = new float[other.size_];
      std::copy(other.ranges_, other.ranges_ + other.size_, fresh);
      delete[] ranges_;                                 // release the old array
      ranges_ = fresh;
      size_ = other.size_;
    }
    return *this;
  }

  void set(std::size_t i, float range_m) {ranges_[i] = range_m;}
  float get(std::size_t i) const {return ranges_[i];}
  std::size_t size() const {return size_;}

private:
  std::size_t size_;
  float * ranges_;                              // a raw pointer that OWNS the array
};

int main()
{
  ScanBuffer scan{360};
  scan.set(0, 1.25f);

  ScanBuffer snapshot = scan;                           // copy constructor
  snapshot.set(0, 9.99f);
  std::cout << "scan[0] = " << scan.get(0) << ", snapshot[0] = " << snapshot.get(0) << '\n';

  ScanBuffer small{10};
  small = scan;                                         // copy assignment
  std::cout << "small now holds " << small.size() << " rays, small[0] = " << small.get(0) << '\n';

  std::cout << "end of main\n";
  return 0;
}                                                       // three arrays, three deletes: no crash
