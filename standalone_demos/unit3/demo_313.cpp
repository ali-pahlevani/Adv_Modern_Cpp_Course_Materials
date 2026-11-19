#include <iostream>
#include <vector>

class WithoutNoexcept
{
public:
  WithoutNoexcept() = default;
  WithoutNoexcept(const WithoutNoexcept &) {++copies;}
  WithoutNoexcept(WithoutNoexcept &&) {++moves;}           // move constructor, NOT noexcept

  inline static int copies{0};
  inline static int moves{0};
};

class WithNoexcept
{
public:
  WithNoexcept() = default;
  WithNoexcept(const WithNoexcept &) {++copies;}
  WithNoexcept(WithNoexcept &&) noexcept {++moves;}        // move constructor, noexcept

  inline static int copies{0};
  inline static int moves{0};
};

int main()
{
  std::vector<WithoutNoexcept> a;
  std::vector<WithNoexcept> b;
  for (int i = 0; i < 1000; ++i) {     // the vectors reallocate as they grow
    a.emplace_back();
    b.emplace_back();
  }

  std::cout << "without noexcept: " << WithoutNoexcept::copies << " copies, "
            << WithoutNoexcept::moves << " moves\n";
  std::cout << "with noexcept:    " << WithNoexcept::copies << " copies, "
            << WithNoexcept::moves << " moves\n";
  return 0;
}
