//
// Created by Nadrino on 28/08/2020.
//

#include <string>
#include <iostream>
#include <thread>

#include <GenericToolbox.h>

#ifdef ENABLE_ROOT_EXTENSION
#include <GenericToolboxRootExt.h>
#endif

#include "classExample.h"

using namespace std;

int main(){

  // String management
  string str("This is a simple message.");
  cout << str << endl;
  cout << "Does str contains \"simple message\"? " << GenericToolbox::doesStringContainsSubstring(str, "simple message") << endl;
  cout << "Remove the caps chars: " << GenericToolbox::toLowerCase(str) << endl;
  auto splitVec = GenericToolbox::splitString(str, " ");
  cout << "Those are the words in str: "; GenericToolbox::printVector(splitVec);
  cout << "Let's_join_the_vector_element:_" << GenericToolbox::joinVectorString(splitVec, "_") << endl;
  cout << GenericToolbox::formatString("I would %s to create a printf-style string %i, %p", "like", 4, &splitVec) << endl;


  // IO examples
  cout << "Current working directory: " << GenericToolbox::getCurrentWorkingDirectory() << endl;
  cout << "Is this a directory? " << GenericToolbox::doesPathIsFolder("../include") << endl;
  cout << "Is this a file? " << GenericToolbox::doesPathIsFile("../include/GenericToolbox.h") << endl;
  cout << "Its size: " << GenericToolbox::parseSizeUnits(GenericToolbox::getFileSizeInBytes("../include/GenericToolbox.h")) << endl;
  cout << "Hash of this file: " << GenericToolbox::getHashFile("../include/GenericToolbox.h") << endl;
  cout << "Are they identical?: " << GenericToolbox::doFilesAreTheSame("../include/GenericToolbox.h","../include/GenericToolbox_impl.h") << endl;


  // Hardware
  cout << "Process takes this amount of RAM: " << GenericToolbox::parseSizeUnits(GenericToolbox::getProcessMemoryUsage()) << endl;
  cout << "Process has taken this amount of RAM at max: " << GenericToolbox::parseSizeUnits(GenericToolbox::getProcessMaxMemoryUsage()) << endl;

  cout << "getTerminalWidth() = " << GenericToolbox::getTerminalWidth() << endl;

  for(int i = 0 ; i < 1000 ; i++){
    GenericToolbox::displayProgressBar(i, 1000, "Loading...");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  GenericToolbox::ProgressBar::enableRainbowProgressBar = true;
  GenericToolbox::ProgressBar::barLength = 18;
  for(int i = 0 ; i < 1000 ; i++){
    GenericToolbox::displayProgressBar(i, 1000, "If you like colors, but not the default bar length:");
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }

  ClassExample example;
  example.MyGreatMethod();

  return EXIT_SUCCESS;
}
