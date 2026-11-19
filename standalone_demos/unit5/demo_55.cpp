#include <iostream>
#include <memory>
#include <utility>

struct Lidar
{
  Lidar() {std::cout << "lidar on\n";}
  ~Lidar() {std::cout << "lidar off\n";}
};

int main()
{
  auto lidar = std::make_unique<Lidar>();               // unique_ptr: exactly one owner
  std::unique_ptr<Lidar> owner = std::move(lidar);      // ownership moves; a copy won't compile
  std::cout << "old owner: " << (lidar ? "set" : "nullptr") << '\n';

  std::weak_ptr<Lidar> observer;                        // weak_ptr: watches, never owns
  {
    auto shared = std::make_shared<Lidar>();            // shared_ptr: counted owners
    std::shared_ptr<Lidar> second_owner = shared;
    observer = shared;
    std::cout << "owners: " << shared.use_count() << '\n';
  }                                                     // the last owner is gone: freed here
  std::cout << "observer sees: " << (observer.lock() ? "alive" : "nullptr") << '\n';
  std::cout << "end of main\n";
}                                                       // owner frees the first lidar here
