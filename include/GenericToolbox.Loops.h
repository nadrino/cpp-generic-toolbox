//
// Created by Nadrino on 18/10/2024.
//

#ifndef GUNDAM_GENERICTOOLBOX_LOOPS_H
#define GUNDAM_GENERICTOOLBOX_LOOPS_H

#include <iostream>
#include <vector>
#include <tuple>

#include <iostream>
#include <tuple>
#include <vector>
#include <utility>



/// Zip iterator structures:
/// example:
/// int main() {
//    std::vector<int> vec1{1, 2, 3, 4};
//    std::vector<double> vec2{1.1, 2.2, 3.3, 4.4};
//    std::vector<char> vec3{'a', 'b', 'c', 'd'};
//
//    // Zip over three containers
//    for (auto [a, b, c] : GenericToolbox::Zip(vec1, vec2, vec3)) {
//        std::cout << "vec1: " << a << ", vec2: " << b << ", vec3: " << c << std::endl;
//    }
//
//    return 0;
// }

namespace GenericToolbox{
  // Helper function to increment each iterator in the tuple
  template <typename Tuple, std::size_t... I>
  void TupleIncrement(Tuple& t, std::index_sequence<I...>) {
    ((++std::get<I>(t)), ...);  // Increment each iterator
  }

// Helper function to compare the end of each iterator with the end of the corresponding container
  template <typename Tuple1, typename Tuple2, std::size_t... I>
  bool tuple_not_equal(const Tuple1& t1, const Tuple2& t2, std::index_sequence<I...>) {
    return ((std::get<I>(t1) != std::get<I>(t2)) && ...);  // Compare iterators with end
  }

// Zip iterator class
  template <typename... Iterators>
  class ZipIterator {
  public:
    explicit ZipIterator(Iterators... iterators_) : _iterators_(iterators_...) {}

    auto operator*() const {
      return std::apply([](auto&... it) { return std::tie(*it...); }, _iterators_);  // Dereference all iterators
    }

    void operator++() {
      TupleIncrement(_iterators_, std::index_sequence_for<Iterators...>{});  // Increment all iterators
    }

    bool operator!=(const ZipIterator& other) const {
      return tuple_not_equal(_iterators_, other._iterators_, std::index_sequence_for<Iterators...>{});  // Compare iterators
    }

  private:
    std::tuple<Iterators...> _iterators_;
  };

  // Zip range class to hold the begin() and end() iterators
  template <typename... Containers>
  class ZipRange {
  public:

    explicit ZipRange(Containers&... containers_)
      : _begin_(std::begin(containers_)...), _end_(std::end(containers_)...) {}
    explicit ZipRange(size_t start_, size_t end_, Containers&... containers_)
      : _begin_(std::next(std::begin(containers_), start_)...),
        _end_(std::next(std::begin(containers_), end_)...) {}

    auto begin() { return _begin_; }
    auto end() { return _end_; }


  private:
    ZipIterator<decltype(std::begin(std::declval<Containers&>()))...> _begin_, _end_;
  };

  // Zip function to create a ZipRange from multiple containers
  template <typename... Containers>
  auto Zip(Containers&... containers) {
    return ZipRange<Containers...>(containers...);
  }

  // Zip function to create a ZipRange from multiple containers
  template <typename... Containers>
  auto ZipPartial(size_t start_, size_t end_, Containers&... containers) {
    return ZipRange<Containers...>(start_, end_, containers...);
  }
}



#endif //GUNDAM_GENERICTOOLBOX_LOOPS_H
