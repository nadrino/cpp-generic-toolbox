//
// Created by Nadrino on 18/10/2024.
//

#ifndef GUNDAM_GENERICTOOLBOX_LOOPS_H
#define GUNDAM_GENERICTOOLBOX_LOOPS_H

#include "GenericToolbox.Macro.h"

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
#if HAS_CPP_17
    ((++std::get<I>(t)), ...);  // Increment each iterator
#else
    // this is a trick to expand the ++std::get<I>(t) for each element I in the tuple.
    using expander = int[];
    // the following executes the increment operation for each element and discards the result (using void).
    // The void casts are used to avoid unused value warnings, and 0 is a dummy value to make the initializer list expansion work.
    (void)expander{0, (void(++std::get<I>(t)), 0)...};  // Use an initializer list to expand the operation
#endif
  }

  // Helper function to compare the end of each iterator with the end of the corresponding container
  template <typename Tuple1, typename Tuple2, std::size_t... I>
  bool tuple_not_equal(const Tuple1& t1, const Tuple2& t2, std::index_sequence<I...>) {
#if HAS_CPP_17
    return ((std::get<I>(t1) != std::get<I>(t2)) && ...);  // Compare iterators with end
#else
    bool result = true;
    using expander = int[];
    (void)expander{0, (void(result = result && (std::get<I>(t1) != std::get<I>(t2))), 0)...};  // Expand the comparison
    return result;
#endif
  }

  // Zip iterator class
  template <typename... Iterators>
  class ZipIterator {
  public:
    explicit ZipIterator(Iterators... iterators_) : _iterators_(iterators_...) {}

    auto operator*() const {
#if HAS_CPP_17
      return std::apply([](auto&... it) { return std::tie(*it...); }, _iterators_);  // Dereference all iterators
#else
      return dereference(std::index_sequence_for<Iterators...>{});
#endif
    }

    void operator++() {
      TupleIncrement(_iterators_, std::index_sequence_for<Iterators...>{});  // Increment all iterators
    }

    bool operator!=(const ZipIterator& other) const {
      return tuple_not_equal(_iterators_, other._iterators_, std::index_sequence_for<Iterators...>{});  // Compare iterators
    }

  protected:
#if !HAS_CPP_17
    // Expanding the tuple in-place inside operator*()
    template <std::size_t... I>
    auto dereference(std::index_sequence<I...>) const {
        return std::tie(*std::get<I>(_iterators_)...);  // Dereference each iterator
    }
#endif

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
