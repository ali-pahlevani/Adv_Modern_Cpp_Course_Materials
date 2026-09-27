// Unit 2, slide 17. std::deque, and using one as a sliding window.
#include <deque>
#include <iostream>

void show(const char * label, const std::deque<char> & d)
{
  std::cout << label;
  for (const char c : d) {std::cout << c << ' ';}
  std::cout << '\n';
}

class HeadingWindow
{
public:
  void add(double heading_rad)
  {
    window_.push_back(heading_rad);
    if (window_.size() > kWindowSize) {
      window_.pop_front();          // O(1); a vector would have to shift every element
    }
  }
  [[nodiscard]] double mean() const
  {
    if (window_.empty()) {return 0.0;}
    double sum = 0.0;
    for (const double h : window_) {sum += h;}
    return sum / static_cast<double>(window_.size());
  }
  [[nodiscard]] std::size_t size() const {return window_.size();}

private:
  static constexpr std::size_t kWindowSize = 3;
  std::deque<double> window_;
};

int main()
{
  std::deque<char> d;
  d.push_back('a');     // a
  d.push_front('b');    // b a
  d.push_back('c');     // b a c
  d.push_front('d');    // d b a c

  show("deque    : ", d);
  std::cout << "at(2)    : " << d.at(2) << '\n';
  std::cout << "front()  : " << d.front() << '\n';
  std::cout << "back()   : " << d.back() << '\n';
  d.pop_front();  show("pop_front: ", d);
  d.pop_back();   show("pop_back : ", d);

  HeadingWindow window;
  for (const double h : {1.0, 2.0, 3.0, 4.0, 5.0}) {
    window.add(h);
    std::cout << "added " << h << " -> window holds " << window.size()
              << ", mean " << window.mean() << '\n';
  }
}
