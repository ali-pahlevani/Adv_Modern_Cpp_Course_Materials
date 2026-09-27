// Unit 2, slide 18. std::list and std::forward_list.
#include <forward_list>
#include <iostream>
#include <iterator>
#include <list>

void show(const char * label, const auto & container)   // C++20 abbreviated template
{
  std::cout << label;
  for (const auto & v : container) {std::cout << v << ' ';}
  std::cout << '\n';
}

int main()
{
  // ---- std::list: O(1) at both ends, list-only algorithms ----
  std::list<int> l{1, 2, 3};
  l.push_back(5);
  l.pop_front();
  show("list basics          : ", l);                 // 2 3 5

  std::list<int> l1{1, 2, 3, 4, 8};
  std::list<int> l2{5, 7, 9};
  l1.swap(l2);   show("after swap           : ", l1);  // 5 7 9
  l1.reverse();  show("after reverse        : ", l1);  // 9 7 5
  l1.sort();     show("after sort (member!) : ", l1);  // 5 7 9  (std::sort won't compile on a list)
  l1.merge(l2);  show("after merge          : ", l1);  // both sorted; l2 is now empty
  std::cout << "l2 is empty: " << std::boolalpha << l2.empty() << '\n';

  // ---- std::forward_list: smallest node, no size(), forward only ----
  std::forward_list<int> f{10, 20, 30, 40, 50};
  f.push_front(60);
  show("forward_list         : ", f);
  auto it = f.insert_after(f.begin(), {1, 2, 3});     // forward_list inserts after a position, not at it
  show("insert_after         : ", f);
  f.erase_after(it);                                   // erases the element after it
  show("erase_after          : ", f);
  f.remove_if([](int x) {return x > 20;});
  show("remove_if(x > 20)    : ", f);

  // f.size();   // doesn't exist
  std::cout << "size via distance (O(n)): " << std::distance(f.begin(), f.end()) << '\n';
}
