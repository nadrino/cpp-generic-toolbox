// Build and run with -std=c++14 and -std=c++17, -Iinclude and -pthread.
#include "GenericToolbox.Map.h"
#include "GenericToolbox.Thread.h"
#include "GenericToolbox.ZipIterator.h"

#include <cassert>
#include <cmath>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

int main(){
  std::map<std::string, int> values{{"present", 42}};
  assert(GenericToolbox::getElementPtrIsInMap(std::string("present"), values) == &values.at("present"));
  assert(GenericToolbox::getElementPtrIsInMap(std::string("missing"), values) == nullptr);
  assert(values.size() == 1);

  using Strings = std::vector<std::string>;
  assert(GenericToolbox::splitString("::a::::b::", "::") == Strings({"", "a", "", "b", ""}));
  assert(GenericToolbox::splitString("::a::::b::", "::", true) == Strings({"a", "b"}));
  assert(GenericToolbox::splitString("", "::") == Strings({""}));
  assert(GenericToolbox::splitString("", "::", true).empty());
  assert(GenericToolbox::splitString("abc", "::") == Strings({"abc"}));
  bool invalidDelimiter{false};
  try{ GenericToolbox::splitString("abc", ""); }
  catch(const std::invalid_argument&){ invalidDelimiter = true; }
  assert(invalidDelimiter);

  std::vector<int> numbers{1, 2, 3};
  std::function<double(int)> convert = [](int value_){ return value_ * 0.5; };
  assert(GenericToolbox::convertVectorType(numbers, convert) == std::vector<double>({0.5, 1., 1.5}));
  assert(GenericToolbox::convertVectorType(std::vector<int>{}, convert).empty());

  const auto evaluate = [](const int& value_){ return value_ * 2.; };
  const std::function<double(const int&)> erased = evaluate;
  assert(GenericToolbox::getAverage(numbers) == 2.);
  assert(GenericToolbox::getAverage(numbers, evaluate) == 4.);
  assert(GenericToolbox::getAverage<int>(numbers, evaluate) == 4.);
  assert(GenericToolbox::getAverage(numbers, erased) == 4.);
  double (*legacyAverage)(const std::vector<int>&, const std::function<double(const int&)>&) = &GenericToolbox::getAverage<int>;
  assert(legacyAverage(numbers, erased) == 4.);
  assert(GenericToolbox::getAverage<int>(numbers, [factor = 2](const int& value_) mutable { return factor * value_; }) == 4.);
  assert(std::abs(GenericToolbox::getStdDev(numbers) - std::sqrt(2. / 3.)) < 1E-12);
  assert(std::abs(GenericToolbox::getStdDev(numbers, evaluate) - std::sqrt(8. / 3.)) < 1E-12);
  assert(GenericToolbox::getStdDev(numbers, evaluate) == GenericToolbox::getStdDev(numbers, erased));
  const auto moveOnly = [factor = std::unique_ptr<int>(new int(2))](const int& value_){ return value_ * *factor; };
  assert(GenericToolbox::getAverage(numbers, moveOnly) == 4.);
  assert(GenericToolbox::getStdDev(numbers, moveOnly) == GenericToolbox::getStdDev(numbers, erased));

  std::vector<int> other{4, 5, 6};
  int count{0};
  for( auto entry : GenericToolbox::zip(numbers, other) ){
    std::get<0>(entry) += std::get<1>(entry);
    ++count;
  }
  assert(count == 3);
  assert(numbers == std::vector<int>({5, 7, 9}));
  count = 0;
  for( auto entry : GenericToolbox::zip(numbers) ){
    assert(std::get<0>(entry) == numbers[count]);
    ++count;
  }
  assert(count == 3);
  assert(GenericToolbox::not_equal_tuples(std::make_tuple(1, 2), std::make_tuple(1, 3)));
  assert(!GenericToolbox::not_equal_tuples(std::make_tuple(1, 2), std::make_tuple(1, 2)));

  GenericToolbox::ParallelWorker worker;
  int calls{0};
  worker.addJob("first", [&](int){ ++calls; });
  for( int iJob = 0 ; iJob < 32 ; ++iJob ){
    worker.addJob(std::to_string(iJob), [&](int){ ++calls; });
  }
  worker.runJob("first");
  worker.runJob("31");
  assert(calls == 2);
  bool duplicateRejected{false};
  try{ worker.addJob("first", [](int){}); }
  catch(const std::logic_error&){ duplicateRejected = true; }
  assert(duplicateRejected);
}
