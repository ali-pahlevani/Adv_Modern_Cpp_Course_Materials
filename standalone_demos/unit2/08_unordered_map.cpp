// Unit 2, slides 28 and 29. std::unordered_map, buckets, and custom keys.
#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>

struct GridKey
{
  std::int32_t x{};
  std::int32_t y{};
  bool operator==(const GridKey &) const = default;   // C++20: required for lookup
};

// A custom key needs a hash function. This simple one is fine for a demo; real code
// should mix the bits better. Equal keys must give equal hashes, but different keys
// are allowed to share one.
template<>
struct std::hash<GridKey>
{
  std::size_t operator()(const GridKey & k) const noexcept
  {
    const std::size_t hx = std::hash<std::int32_t>{}(k.x);
    const std::size_t hy = std::hash<std::int32_t>{}(k.y);
    return hx ^ (hy << 1);
  }
};

int main()
{
  std::unordered_map<std::string, double> m;
  m.reserve(8);                          // pre-size: no rehash while filling

  m["one"] = 1.1;
  m["two"] = 2.22;
  m["three"] = 3.333;
  m.emplace("four", 4.4444);
  m.try_emplace("five", 5.55555);

  for (const std::string key : {"four", "nine"}) {
    std::cout << key << (m.contains(key) ? " found\n" : " not found\n");
  }

  std::cout << "\nall elements (the order is unspecified, so yours may differ):\n";
  for (const auto & [key, value] : m) {
    std::cout << "  " << key << ' ' << value << '\n';
  }

  std::cout << "\nbuckets     : " << m.bucket_count()
            << "\nload factor : " << m.load_factor()
            << "\nbucket(four): " << m.bucket("four") << '\n';

  // A sparse occupancy grid: only visited cells are stored
  std::unordered_map<GridKey, float> occupancy;
  occupancy[GridKey{12, -3}] = 0.9f;
  occupancy[GridKey{12, -2}] = 0.1f;
  std::cout << "\noccupancy of cell (12, -3): " << occupancy.at(GridKey{12, -3})
            << "  (" << occupancy.size() << " cells stored)\n";
}
