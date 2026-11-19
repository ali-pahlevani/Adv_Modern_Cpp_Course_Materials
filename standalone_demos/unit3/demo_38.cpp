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

  {
    ScanBuffer snapshot = scan;                 // compiler-written copy: copies the POINTER
    snapshot.set(0, 9.99f);
    std::cout << "scan[0] = " << scan.get(0) << "   (changed through the copy!)\n";
  }                                             // snapshot's destructor frees the array...

  std::cout << "end of main\n";
  return 0;
}                                               // ...and scan's destructor frees it again
