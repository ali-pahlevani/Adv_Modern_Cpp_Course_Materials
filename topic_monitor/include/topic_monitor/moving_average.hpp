// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// MovingAverage from Demo 6.3 (Unit 6, slide 4), used by Exercise 6.3

#pragma once
#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <numeric>

template <std::floating_point T, std::size_t N>     // a type AND a value as parameters
class MovingAverage
{
  static_assert(N > 0, "the window needs at least one sample");

public:
  void add(T sample)
  {
    samples_[next_] = sample;
    next_ = (next_ + 1) % N;                        // overwrite the oldest sample next
    count_ = std::min(count_ + 1, N);
  }

  T value() const
  {
    if (count_ == 0) {return T{0};}
    return std::accumulate(samples_.begin(), samples_.begin() + count_, T{0}) / static_cast<T>(count_);
  }

private:
  std::array<T, N> samples_{};                      // N is known at compile time: no heap
  std::size_t next_{0};
  std::size_t count_{0};
};
