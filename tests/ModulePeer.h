#pragma once

#include <string>

const void* getPeerProgressBar();
bool hasPeerClock(const std::string& key_);
long long getPeerElapsedTime(const std::string& key_);
std::string getPeerWorkingDirectory();
void runPeerModules();
