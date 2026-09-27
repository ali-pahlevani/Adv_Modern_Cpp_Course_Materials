// Unit 2, slides 32 to 36. Iterators, and the helper functions that move them.
#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <ranges>
#include <vector>

int main()
{
  // ---- slide 35: one loop, any container ----
  const std::vector<int> v{1, 2, 3, 4, 4, 1, 6, 7};
  const std::list<int> l{1, 2, 3, 4, 4, 1, 6, 7};

  for (auto it = v.begin(); it != v.end(); ++it) {std::cout << *it << ' ';}
  std::cout << "  <- vector\n";
  for (auto it = l.begin(); it != l.end(); ++it) {std::cout << *it << ' ';}
  std::cout << "  <- list, identical loop\n\n";

  // ---- slide 34: advance changes the iterator, next/prev return a new one ----
  std::vector<int> w{1, 2, 3, 4, 4, 6, 7};
  //                 0  1  2  3  4  5  6
  auto it = w.begin();
  std::cout << "*begin()            : " << *it << '\n';
  std::advance(it, 5);
  std::cout << "after advance(+5)   : " << *it << '\n';
  std::advance(it, -1);
  std::cout << "after advance(-1)   : " << *it << '\n';

  it = w.begin();
  std::cout << "distance(begin, end): " << std::distance(w.begin(), w.end()) << '\n';
  std::cout << "*next(it, 4)        : " << *std::next(it, 4) << '\n';
  std::cout << "  ...but it is still: " << *it << "   <- next() did not move it\n";
  it = std::next(it, 4);
  std::cout << "after it = next(4)  : " << *it << '\n';
  std::cout << "*prev(it, 2)        : " << *std::prev(it, 2) << "\n\n";

  // ---- slide 36: const_iterator, reverse, back_inserter ----
  auto mut = w.begin();
  *mut = 99;                                          // iterator: writable
  auto ro = w.cbegin();                               // const_iterator: *ro = 1 won't compile
  std::cout << "after *begin() = 99 : " << *ro << '\n';

  std::cout << "reversed            : ";
  for (const int x : w | std::views::reverse) {std::cout << x << ' ';}
  std::cout << '\n';

  std::vector<int> dest;                              // empty, so nothing to write into yet
  dest.reserve(w.size());
  std::ranges::copy(w, std::back_inserter(dest));     // each write becomes a push_back
  std::cout << "copied via back_inserter: " << dest.size() << " elements\n";
}
