#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <utility>

// nav_msgs/OccupancyGrid values: -1 unknown, 0 free, 100 occupied
class OccupancyGrid
{
public:
  OccupancyGrid(std::size_t width, std::size_t height)
  : width_{width}, height_{height}, cells_{new std::int8_t[width * height]{}}
  {}

  ~OccupancyGrid() {delete[] cells_;}

  OccupancyGrid(const OccupancyGrid & other)                 // copy constructor
  : width_{other.width_}, height_{other.height_},
    cells_{new std::int8_t[other.width_ * other.height_]}
  {
    std::copy(other.cells_, other.cells_ + other.cell_count(), cells_);
  }

  OccupancyGrid & operator=(const OccupancyGrid & other)     // copy assignment
  {
    if (this != &other) {
      auto * fresh = new std::int8_t[other.cell_count()];
      std::copy(other.cells_, other.cells_ + other.cell_count(), fresh);
      delete[] cells_;
      cells_ = fresh;
      width_ = other.width_;
      height_ = other.height_;
    }
    return *this;
  }

  OccupancyGrid(OccupancyGrid && other) noexcept             // move constructor
  : width_{std::exchange(other.width_, 0)},
    height_{std::exchange(other.height_, 0)},
    cells_{std::exchange(other.cells_, nullptr)}
  {}

  OccupancyGrid & operator=(OccupancyGrid && other) noexcept // move assignment
  {
    if (this != &other) {
      delete[] cells_;
      width_ = std::exchange(other.width_, 0);
      height_ = std::exchange(other.height_, 0);
      cells_ = std::exchange(other.cells_, nullptr);
    }
    return *this;
  }

  void set(std::size_t x, std::size_t y, std::int8_t value) {cells_[y * width_ + x] = value;}
  std::int8_t at(std::size_t x, std::size_t y) const {return cells_[y * width_ + x];}
  std::size_t width() const {return width_;}
  std::size_t height() const {return height_;}
  std::size_t cell_count() const {return width_ * height_;}

private:
  std::size_t width_;
  std::size_t height_;
  std::int8_t * cells_;        // owns width_ * height_ cells
};

int main()
{
  OccupancyGrid map{4, 3};                     // 4 x 3 cells, all 0 (free)
  map.set(1, 2, 100);                          // cell (1, 2) is occupied

  OccupancyGrid copy = map;                    // copy constructor
  copy.set(1, 2, 0);
  std::cout << "1. map(1,2) = " << static_cast<int>(map.at(1, 2))
            << ", copy(1,2) = " << static_cast<int>(copy.at(1, 2)) << '\n';

  OccupancyGrid moved = std::move(map);        // move constructor
  std::cout << "2. moved is " << moved.width() << " x " << moved.height()
            << ", map has " << map.cell_count() << " cells left\n";

  copy = moved;                                // copy assignment
  std::cout << "3. copy(1,2) = " << static_cast<int>(copy.at(1, 2)) << '\n';

  moved = OccupancyGrid{2, 2};                 // move assignment
  std::cout << "4. moved is " << moved.width() << " x " << moved.height() << '\n';
  return 0;
}
