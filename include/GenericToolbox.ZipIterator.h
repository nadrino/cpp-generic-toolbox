//
// Created by Adrien Blanchet on 15/10/2023.
//

#ifndef GENERIC_TOOLBOX_ZIP_ITERATOR_H
#define GENERIC_TOOLBOX_ZIP_ITERATOR_H

#include <iterator>
#include <iostream>
#include <utility>
#include <vector>
#include <tuple>

namespace GenericToolbox{

//  // Define a zip iterator
//  template <typename... Containers>
//  class ZipIterator {
//  public:
//
//    using value_type = std::tuple<typename std::iterator_traits<typename Containers::iterator>::value_type...>;
//    using reference = value_type;
////    using value_type = std::tuple<typename std::iterator_traits<typename Containers::iterator>::value_type...>;
////    using reference = std::tuple<typename std::add_lvalue_reference<typename Containers::value_type>::type...>;
//
//    explicit ZipIterator(Containers&... containers_)
//      : _iterators_( std::make_tuple(containers_.begin()...) ),
//        _begin_( std::make_tuple(containers_.begin()...) ),
//        _end_( std::make_tuple(containers_.end()...) )
//      {}
//
//    ZipIterator& operator++() {
//      increment(std::index_sequence_for<Containers...>());
//      return *this;
//    }
//
//    ZipIterator operator++(int) {
//      ZipIterator temp{ *this };
//      increment(std::index_sequence_for<Containers...>());
//      return temp;
//    }
//
//    reference operator*() {
//      return dereference(std::index_sequence_for<Containers...>());
//    }
//
//    reference begin() {
//      return dereferenceBegin(std::index_sequence_for<Containers...>());
//    }
//    reference end() {
//      return dereferenceEnd(std::index_sequence_for<Containers...>());
//    }
//
////    bool operator==(const ZipIterator& other) const {
////      return std::get<0>(_iterators_) == std::get<0>(other._iterators_);
////    }
////
////    bool operator!=(const ZipIterator& other) const {
////      return !(*this == other);
////    }
//
//  private:
//    template <std::size_t... Index>
//    void increment(std::index_sequence<Index...>) {
//      (..., void(++std::get<Index>(_iterators_)));
//    }
//
//    template <std::size_t... Index>
//    reference dereference(std::index_sequence<Index...>) {
//      return std::make_tuple(std::ref(*std::get<Index>(_iterators_))...);
//    }
//    template <std::size_t... Index>
//    reference dereferenceBegin(std::index_sequence<Index...>) {
//      return std::make_tuple(std::ref(*std::get<Index>(_begin_))...);
//    }
//    template <std::size_t... Index>
//    reference dereferenceEnd(std::index_sequence<Index...>) {
//      return std::make_tuple(std::ref(*std::get<Index>(_end_))...);
//    }
//
//    std::tuple<typename Containers::iterator...> _iterators_;
//    std::tuple<typename Containers::iterator...> _begin_;
//    std::tuple<typename Containers::iterator...> _end_;
//  };
//
//  // Define a zip function
//  template <typename... Containers>
//  ZipIterator<Containers...> zip(Containers&... containers_) {
//    return ZipIterator<Containers...>(containers_...);
//  }

  /***************************
// helper for tuple_subset and tuple_tail (from http://stackoverflow.com/questions/8569567/get-part-of-stdtuple)
***************************/
  template <size_t... n>
  struct ct_integers_list {
    template <size_t m>
    struct push_back
    {
      typedef ct_integers_list<n..., m> type;
    };
  };

  template <size_t max>
  struct ct_iota_1
  {
    typedef typename ct_iota_1<max-1>::type::template push_back<max>::type type;
  };

  template <>
  struct ct_iota_1<0>
  {
    typedef ct_integers_list<> type;
  };

/***************************
// return a subset of a tuple
***************************/
//  template <size_t... indices, typename Tuple>
//  auto tuple_subset(const Tuple& tpl, ct_integers_list<indices...>)
//  -> decltype(std::make_tuple(std::get<indices>(tpl)...))
//  {
//    return std::make_tuple(std::get<indices>(tpl)...);
//    // this means:
//    //   make_tuple(get<indices[0]>(tpl), get<indices[1]>(tpl), ...)
//  }

  template <size_t... indices, typename Tuple>
  auto tuple_subset(Tuple&& tpl, ct_integers_list<indices...>)
  -> decltype(std::make_tuple(std::get<indices>(std::forward<Tuple>(tpl))...))
  {
    return std::make_tuple(std::get<indices>(std::forward<Tuple>(tpl))...);
    // this means:
    //   make_tuple(get<indices[0]>(tpl), get<indices[1]>(tpl), ...)
  }

/***************************
// return the tail of a tuple
***************************/
  template <typename Head, typename... Tail>
  inline std::tuple<Tail...> tuple_tail(const std::tuple<Head, Tail...>& tpl)
  {
    return tuple_subset(tpl, typename ct_iota_1<sizeof...(Tail)>::type());
    // this means:
    //   tuple_subset<1, 2, 3, ..., sizeof...(Tail)-1>(tpl, ..)
  }

/***************************
// increment every element in a tuple (that is referenced)
***************************/
  template<std::size_t I = 0, typename... Tp>
  inline typename std::enable_if<I == sizeof...(Tp), void>::type
  increment(std::tuple<Tp...>& t)
  { }

  template<std::size_t I = 0, typename... Tp,
      typename = typename std::enable_if<I < sizeof...(Tp), void>::type>
  inline void increment(std::tuple<Tp...>& t)
  {
    std::get<I>(t)++ ;
    increment<I + 1, Tp...>(t);
  }

/****************************
// check equality of a tuple
****************************/
  template<typename T1>
  inline bool not_equal_tuples( const std::tuple<T1>& t1,  const std::tuple<T1>& t2 )
  {
    return (std::get<0>(t1) != std::get<0>(t2));
  }

  template<typename T1, typename... Ts>
  inline bool not_equal_tuples( const std::tuple<T1, Ts...>& t1,  const std::tuple<T1, Ts...>& t2 )
  {
    return (std::get<0>(t1) != std::get<0>(t2)) && not_equal_tuples( tuple_tail(t1), tuple_tail(t2) );
  }

/****************************
// dereference a subset of elements of a tuple (dereferencing the iterators)
****************************/
  template <size_t... indices, typename Tuple>
  auto dereference_subset(const Tuple& tpl, ct_integers_list<indices...>)
  -> decltype(std::tie(*std::get<indices - 1>(tpl)...))
  {
    return std::tie(*std::get<indices - 1>(tpl)...);
  }

/****************************
// dereference every element of a tuple (applying operator* to each element, and returning the tuple)
****************************/
  template<typename... Ts>
  inline auto
  dereference_tuple(std::tuple<Ts...>& t1) -> decltype( dereference_subset( std::tuple<Ts...>(), typename ct_iota_1<sizeof...(Ts)>::type()))
  {
    return dereference_subset( t1, typename ct_iota_1<sizeof...(Ts)>::type());
  }


  template< typename T1, typename... Ts >
  class zipper
  {
  public:

    class iterator : std::iterator<std::forward_iterator_tag, std::tuple<typename T1::value_type, typename Ts::value_type...> >
    {
    protected:
      std::tuple<typename T1::iterator, typename Ts::iterator...> current;
    public:

      explicit iterator(  typename T1::iterator s1, typename Ts::iterator... s2 ) :
          current(s1, s2...) {};

      iterator( const iterator& rhs ) :  current(rhs.current) {};

      iterator& operator++() {
        increment(current);
        return *this;
      }

      iterator operator++(int) {
        auto a = *this;
        increment(current);
        return a;
      }

      bool operator!=( const iterator& rhs ) {
        return not_equal_tuples(current, rhs.current);
      }

      typename iterator::value_type operator*() {
        return dereference_tuple(current);
      }
    };


    explicit zipper( T1& a, Ts&... b):
        begin_( a.begin(), (b.begin())...),
        end_( a.end(), (b.end())...) {};

    zipper(const zipper<T1, Ts...>& a) :
        begin_(  a.begin_ ),
        end_( a.end_ ) {};

    template<typename U1, typename... Us>
    zipper<U1, Us...>& operator=( zipper<U1, Us...>& rhs) {
      begin_ = rhs.begin_;
      end_ = rhs.end_;
      return *this;
    }

    zipper<T1, Ts...>::iterator& begin() {
      return begin_;
    }

    zipper<T1, Ts...>::iterator& end() {
      return end_;
    }

    const zipper<T1, Ts...>::iterator& begin() const {
      return begin_;
    }

    const zipper<T1, Ts...>::iterator& end() const {
      return end_;
    }
    const zipper<T1, Ts...>::iterator& cbegin() const {
      return begin_;
    }

    const zipper<T1, Ts...>::iterator& cend() const {
      return end_;
    }

    zipper<T1, Ts...>::iterator begin_;
    zipper<T1, Ts...>::iterator end_;
  };



//from cppreference.com:
  template <class T>
  struct special_decay
  {
    using type = typename std::decay<T>::type;
  };

//allows the use of references:
  template <class T>
  struct special_decay<std::reference_wrapper<T>>
  {
    using type = T&;
  };

  template <class T>
  using special_decay_t = typename special_decay<T>::type;

//allows template type deduction for zipper:
  template <class... Types>
  zipper<special_decay_t<Types>...> zip(Types&&... args)
  {
    return zipper<special_decay_t<Types>...>(std::forward<Types>(args)...);
  }

}

#endif //GENERIC_TOOLBOX_ZIP_ITERATOR_H
