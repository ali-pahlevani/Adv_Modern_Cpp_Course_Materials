// Unit 2, slides 21 to 25. std::set and std::multiset.
#include <cmath>
#include <compare>
#include <functional>
#include <iostream>
#include <set>
#include <string>

void show(const char * label, const auto & container)
{
  std::cout << label;
  for (const auto & v : container) {std::cout << v << ' ';}
  std::cout << '\n';
}

struct GridPoint
{
  long x_cm{};
  long y_cm{};
  auto operator<=>(const GridPoint &) const = default;   // C++20: gives all six comparisons
};

int main()
{
  // ---- slide 21: unique and always sorted ----
  std::set<char> letters;
  for (const char c : {'R', 'O', 'B', 'O', 'T'}) {letters.insert(c);}
  show("set      ROBOT: ", letters);                            // B O R T
  std::cout << "size " << letters.size() << '\n';

  std::set<std::string> frames;
  for (const char * frame : {"odom", "base_link", "odom"}) {
    const auto [it, inserted] = frames.insert(frame);
    std::cout << *it << (inserted ? " -> new\n" : " -> already known\n");
  }
  std::cout << "contains(\"odom\"): " << std::boolalpha << frames.contains("odom") << '\n';

  // ---- slide 22: custom ordering, erase, extract ----
  const std::set<int, std::greater<>> descending{100, 1, 80, 180, 0, 510};
  show("descending : ", descending);
  std::set<int> s(descending.begin(), descending.end());
  show("ascending  : ", s);
  s.erase(s.begin(), s.find(100));
  show("erase < 100: ", s);
  std::cout << "erase(510) removed " << s.erase(510) << '\n';

  auto node = s.extract(100);          // C++17: take the node out, change it, put it back; no new allocation
  node.value() = 150;
  s.insert(std::move(node));
  show("100 -> 150 : ", s);

  // ---- slide 23: lower_bound / upper_bound ----
  const std::set<int> b{0, 1, 80, 100, 180, 510};
  std::cout << "lower_bound(80) = " << *b.lower_bound(80)
            << ", upper_bound(80) = " << *b.upper_bound(80) << '\n';
  std::cout << "range [80, 180]: ";
  for (auto it = b.lower_bound(80); it != b.upper_bound(180); ++it) {std::cout << *it << ' ';}
  std::cout << '\n';

  // ---- slide 24: floating-point keys don't de-duplicate ----
  std::set<double> xs{0.1 + 0.2, 0.3};
  std::cout << "set{0.1 + 0.2, 0.3}.size() = " << xs.size() << "   <- not 1!\n";
  std::set<GridPoint> grid;
  grid.insert(GridPoint{std::lround((0.1 + 0.2) * 100.0), 0});
  grid.insert(GridPoint{std::lround(0.3 * 100.0), 0});
  std::cout << "quantised to cm, size = " << grid.size() << '\n';

  // ---- slide 25: multiset keeps duplicates, and erase(k) removes all of them ----
  std::multiset<int> ms{100, 1, 80, 180, 0, 510, 510, 100};
  show("multiset   : ", ms);
  std::cout << "count(510) = " << ms.count(510) << '\n';
  ms.erase(ms.find(100));                                // exactly one
  show("erase one 100 : ", ms);
  std::cout << "erase(510) removed " << ms.erase(510) << "   <- every copy\n";
  show("after      : ", ms);
}
