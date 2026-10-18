#include "GenericToolbox.Os.h"
#include "GenericToolbox.Fs.h"
#include "GenericToolbox.Time.h"
#include "GenericToolbox.Thread.h"
#include "GenericToolbox.Stream.h"
#include "GenericToolbox.Map.h"
#include "GenericToolbox.Misc.h"
#include "GenericToolbox.ZipIterator.h"

#ifdef TOOLBOX_TEST_ROOT
#include "GenericToolbox.Root.h"
#endif
#ifdef TOOLBOX_TEST_JSON
#include "GenericToolbox.Json.h"
#endif
#ifdef TOOLBOX_TEST_YAML
#include "GenericToolbox.Yaml.h"
#endif

#include "ModulePeer.h"

#include <atomic>
#include <chrono>
#include <cstdint>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
  void require(bool condition_, const std::string& message_){
    if(not condition_) throw std::runtime_error(message_);
  }
}

int main() try {
  const auto directory = GenericToolbox::getCurrentWorkingDirectory();
  require(not directory.empty(), "working directory");
  require(getPeerWorkingDirectory() == directory, "cross-file Os");
  require(GenericToolbox::isDir(directory), "Fs directory");
#if defined(_WIN32)
  require(GenericToolbox::expandEnvironmentVariables("${USERPROFILE}") == GenericToolbox::getHomeDirectory(), "environment expansion");
  require(not GenericToolbox::getHostName().empty(), "Windows hostname");
  require(not GenericToolbox::getOsVersion().empty(), "Windows version");
  require(GenericToolbox::getOsArchitecture() != "Unknown", "Windows architecture");
  require(GenericToolbox::getTotalDiskSpace(directory) > 0, "Windows disk space");
#else
  require(GenericToolbox::expandEnvironmentVariables("${HOME}") == GenericToolbox::getHomeDirectory(), "environment expansion");
#endif
  require(GenericToolbox::expandEnvironmentVariables("${TOOLBOX_TEST_UNDEFINED_VARIABLE}") == "${TOOLBOX_TEST_UNDEFINED_VARIABLE}", "missing environment variable");
  require(GenericToolbox::expandEnvironmentVariables("~/file") == GenericToolbox::getHomeDirectory() + "/file", "home expansion");
  const auto file = GenericToolbox::joinPath(directory, "ToolboxModules.txt");
  GenericToolbox::ScopedGuard removeFile{[]{}, [&]{ GenericToolbox::rm(file); }};
  GenericToolbox::dumpStringInFile(file, "first\nsecond\n");
#if defined(_WIN32)
  require(GenericToolbox::dumpFileAsString(file) == "first\r\nsecond\r\n", "Fs text round trip");
#else
  require(GenericToolbox::dumpFileAsString(file) == "first\nsecond\n", "Fs round trip");
#endif
  require(GenericToolbox::getFileSize(file) == static_cast<decltype(GenericToolbox::getFileSize(file))>(GenericToolbox::dumpFileAsString(file).size()), "Fs file size");
  const auto scratch = GenericToolbox::joinPath(directory, "ToolboxScratch");
  const auto nested = GenericToolbox::joinPath(scratch, "nested");
  const auto copied = GenericToolbox::joinPath(nested, "copied.txt");
  const auto moved = GenericToolbox::joinPath(nested, "moved.txt");
  GenericToolbox::ScopedGuard removeDirectory{[]{}, [&]{
    GenericToolbox::rm(copied);
    GenericToolbox::rm(moved);
    GenericToolbox::rmDir(nested);
    GenericToolbox::rmDir(scratch);
  }};
  require(GenericToolbox::mkdir(nested), "Fs nested directories");
  require(GenericToolbox::cp(file, copied), "Fs copy");
  require(not GenericToolbox::cp(file, copied), "Fs no overwrite");
  require(GenericToolbox::lsDirs(scratch) == std::vector<std::string>{"nested"}, "Fs directory listing");
  require(GenericToolbox::lsFiles(nested, "*.txt", 1) == std::vector<std::string>{"copied.txt"}, "Fs filtered listing");
  require(GenericToolbox::mv(copied, moved), "Fs move");
  require(GenericToolbox::lsFilesRecursive(scratch) == std::vector<std::string>{"nested/moved.txt"}, "Fs recursive listing");
  require(GenericToolbox::rm(moved), "Fs remove file");
  require(GenericToolbox::rmDir(nested), "Fs remove directory");
  require(GenericToolbox::isDirEmpty(scratch), "Fs empty directory");
  require(GenericToolbox::getFileName(file) == "ToolboxModules.txt", "Fs filename");
  require(GenericToolbox::replaceExtension(file, "json") == GenericToolbox::joinPath(directory, "ToolboxModules.json"), "Fs extension");

  require(GenericToolbox::getPrintSize("\033[31mhello\033[0m") == 5, "ANSI print size");
  require(GenericToolbox::splitString("a::b", "::") == std::vector<std::string>{"a", "b"}, "split");
  require(GenericToolbox::formatString("answer=%d", 42) == "answer=42", "format template");
  require(GenericToolbox::toString(std::vector<std::string>{"a", "b"}).find("a") != std::string::npos, "String specialization");
  std::vector<std::string> words{"alpha"};
  GenericToolbox::addIfNotInVector("beta", words);
  require(GenericToolbox::findElementIndex("beta", words) == 1, "Vector non-template overload");
  require(GenericToolbox::findElementIndex(2, std::vector<int>{1, 2, 3}) == 1, "Vector template");

  GenericToolbox::TablePrinter table;
  table.setColTitles({"name", "value"});
  table.addTableLine({"alpha", "42"});
  require(table.generateTableString().find("alpha") != std::string::npos, "TablePrinter");
  GenericToolbox::AnyType value{42};
  const GenericToolbox::AnyType copy{value};
  require(copy.getValue<int>() == 42, "AnyType copy and template");
  require(copy.getValueAsDouble() == 42., "AnyType conversion");
  std::ostringstream formatted;
  formatted << copy;
  require(formatted.str() == "42", "AnyType friend operator");
  runPeerModules();

  GenericToolbox::Time::StopWatch watch;
  require(not watch.toString().empty(), "Time class");
  const std::string key{"cross-file-clock"};
  GenericToolbox::Time::Internals::lastTimePointMapStr[key] = std::chrono::high_resolution_clock::now() - std::chrono::seconds(3);
#ifdef CPP_GENERIC_TOOLBOX_TIME_COMPILED
  require(hasPeerClock(key), "compiled clock must be visible in another file");
  require(getPeerElapsedTime(key) > 2000000, "compiled clock state must be shared");
#else
  require(not hasPeerClock(key), "header-only clock state remains local");
#endif
#ifdef CPP_GENERIC_TOOLBOX_UTILS_COMPILED
  require(getPeerProgressBar() == &GenericToolbox::ProgressBar::gProgressBar, "compiled progress state must be shared");
#else
  require(getPeerProgressBar() != &GenericToolbox::ProgressBar::gProgressBar, "header-only progress state remains local");
#endif

  std::atomic<int> calls{0};
  {
    GenericToolbox::ParallelWorker worker;
    worker.setNThreads(2);
    worker.addJob("count", [&](int){ ++calls; });
    worker.runJob("count");
  }
  require(calls == 2, "Thread workers");

  std::istringstream bytes{"abcd"};
  GenericToolbox::DataStream stream;
  stream.load(bytes);
  require(stream.read<uint8_t>() == 'a', "Stream template");
  require(stream.readString(3) == "bcd", "Stream string");

#ifdef TOOLBOX_TEST_JSON
  auto json = GenericToolbox::Json::readConfigJsonStr(R"({"value":42,"nested":{"name":"original"}})");
  require(GenericToolbox::Json::fetchValue<int>(json, "value") == 42, "Json template");
  GenericToolbox::Json::applyOverrides(json, GenericToolbox::Json::readConfigJsonStr(R"({"nested":{"name":"updated"}})"));
  require(json["nested"]["name"] == "updated", "Json override");
  require(not GenericToolbox::Json::toReadableString(json).empty(), "Json formatting");
#endif
#ifdef TOOLBOX_TEST_YAML
  const auto yaml = YAML::Load("value: 42");
  require(GenericToolbox::Yaml::fetchValue<int>(yaml, "value") == 42, "Yaml template");
  require(GenericToolbox::Yaml::toJsonString(yaml).find("42") != std::string::npos, "Yaml formatting");
#endif
#ifdef TOOLBOX_TEST_ROOT
  TMatrixD covariance(2, 2);
  covariance(0, 0) = 4.; covariance(0, 1) = 1.;
  covariance(1, 0) = 1.; covariance(1, 1) = 3.;
  std::unique_ptr<TMatrixD> cholesky{GenericToolbox::getCholeskyMatrix(&covariance)};
  require(cholesky != nullptr, "Root Cholesky");
  const auto random = GenericToolbox::throwCorrelatedParameters(cholesky.get());
  require(random.size() == 2, "Root throws");
  require(GenericToolbox::getLinearBinning(2, 0., 1.) == std::vector<double>{0., .5, 1.}, "Root binning");
#endif
}
catch( const std::exception& error ){
  std::cerr << error.what() << std::endl;
  return 1;
}
