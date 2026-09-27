// Unit 2, slides 26 and 27. std::map: inserting, the operator[] trap, and safe reads.
#include <cmath>
#include <iostream>
#include <map>
#include <ranges>
#include <string>

int main()
{
  std::map<std::string, double> joint_positions;
  joint_positions["left_wheel_joint"] = 1.57;
  joint_positions["right_wheel_joint"] = 3.14;
  joint_positions["caster_joint"] = 0.00;

  // Inserted as left, right, caster, but printed alphabetically: a map is sorted by key
  for (const auto & [name, position] : joint_positions) {
    std::cout << name << " -> " << position << '\n';
  }

  // ---- slide 26: operator[] inserts silently ----
  std::cout << "\nsize before reading a typo: " << joint_positions.size() << '\n';
  const double oops = joint_positions["typo_joint"];         // careful: this inserts typo_joint with 0.0
  std::cout << "read value " << oops << ", size is now " << joint_positions.size() << '\n';
  joint_positions.erase("typo_joint");

  // ---- the five ways to insert ----
  std::map<std::string, double> m;
  m["a"] = 1.0;
  m.insert({"a", 99.0});                                      // does nothing, "a" already exists
  m.emplace("b", 2.0);
  const auto [it, inserted] = m.insert_or_assign("a", 10.0);  // C++17, overwrites and tells you if it inserted
  m.try_emplace("c", 3.0);                                    // C++17
  std::cout << "\na = " << m.at("a") << " (insert_or_assign inserted? "
            << std::boolalpha << inserted << ")\n";

  // ---- slide 27: reading safely ----
  if (const auto found = joint_positions.find("left_wheel_joint");
    found != joint_positions.end())
  {
    std::cout << "left wheel: " << found->second << " rad\n";   // one lookup
  }
  std::cout << "contains(\"wheel\"): " << joint_positions.contains("wheel") << '\n';
  try {
    std::cout << joint_positions.at("missing_joint") << '\n';
  } catch (const std::out_of_range &) {
    std::cout << "at(\"missing_joint\") threw std::out_of_range\n";
  }

  // ---- C++20 views over keys, and erase_if ----
  std::cout << "keys:";
  for (const auto & name : joint_positions | std::views::keys) {std::cout << ' ' << name;}
  std::cout << '\n';

  joint_positions["broken_joint"] = std::nan("");
  const auto removed = std::erase_if(joint_positions, [](const auto & entry) {
    return !std::isfinite(entry.second);
  });
  std::cout << "erase_if removed " << removed << " non-finite entr"
            << (removed == 1 ? "y" : "ies") << '\n';
}
