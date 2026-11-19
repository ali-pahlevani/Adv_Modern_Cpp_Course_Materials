#include <algorithm>
#include <cstddef>
#include <iostream>
#include <utility>

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
    std::cout << "  copy constructor\n";
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
    std::cout << "  copy assignment\n";
    return *this;
  }

  ScanBuffer(ScanBuffer && other) noexcept              // move constructor: steal the array
  : size_{std::exchange(other.size_, 0)},
    ranges_{std::exchange(other.ranges_, nullptr)}      // leave the source empty
  {
    std::cout << "  move constructor\n";
  }

  ScanBuffer & operator=(ScanBuffer && other) noexcept  // move assignment
  {
    if (this != &other) {
      delete[] ranges_;                                 // release our old array
      size_ = std::exchange(other.size_, 0);
      ranges_ = std::exchange(other.ranges_, nullptr);
    }
    std::cout << "  move assignment\n";
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
  std::cout << "1. ScanBuffer a{360};\n";
  ScanBuffer a{360};
  a.set(0, 1.25f);

  std::cout << "2. ScanBuffer b = a;\n";
  ScanBuffer b = a;

  std::cout << "3. ScanBuffer c = std::move(a);\n";
  ScanBuffer c = std::move(a);
  std::cout << "   a now holds " << a.size() << " rays, c[0] = " << c.get(0) << '\n';

  std::cout << "4. b = ScanBuffer{720};\n";
  b = ScanBuffer{720};

  std::cout << "5. b = c;\n";
  b = c;

  std::cout << "end of main\n";
  return 0;
}
