#include "GenericToolbox.Os.h"
#include "GenericToolbox.Time.h"
#include "GenericToolbox.Stream.h"
#include "GenericToolbox.Map.h"

#include "ModulePeer.h"

const void* getPeerProgressBar(){ return &GenericToolbox::ProgressBar::gProgressBar; }
bool hasPeerClock(const std::string& key_){
  return GenericToolbox::Time::Internals::lastTimePointMapStr.count(key_) != 0;
}
long long getPeerElapsedTime(const std::string& key_){
  return GenericToolbox::getElapsedTimeSinceLastCallInMicroSeconds(key_);
}
std::string getPeerWorkingDirectory(){ return GenericToolbox::getCurrentWorkingDirectory(); }
void runPeerModules(){
  GenericToolbox::AnyType value{17};
  if(value.getValue<int>() != 17) throw std::runtime_error("peer AnyType");
}
