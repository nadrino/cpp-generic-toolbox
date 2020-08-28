//
// Created by Nadrino on 28/08/2020.
//

#include <string>
#include <iostream>

#include <GenericToolbox.h>


int main(){

  std::string test = "lol_foo_bar";
  auto split = GenericToolbox::splitString(test, "_");
  for(const auto &str : split){
    std::cout << str << std::endl;
  }

  std::cout << "IsFile?: ../include/GenericToolbox.h: " << GenericToolbox::doesPathIsFile("../include/GenericToolbox.h") << std::endl;
  std::cout << "Hash: ../include/GenericToolbox.h: " << GenericToolbox::getHashFile("../include/GenericToolbox.h") << std::endl;
  std::cout << "Hash: ../include/GenericToolbox_impl.h: " << GenericToolbox::getHashFile("../include/GenericToolbox_impl.h") << std::endl;
  std::cout << "Same?: " << GenericToolbox::doFilesAreTheSame("../include/GenericToolbox.h", "../include/GenericToolbox_impl.h") << std::endl;
  std::cout << "Process RAM: " << GenericToolbox::parseSizeUnits(GenericToolbox::getProcessMemoryUsage()) << std::endl;
  std::cout << "Process Max RAM: " << GenericToolbox::parseSizeUnits(GenericToolbox::getProcessMaxMemoryUsage()) << std::endl;

  std::cout << GenericToolbox::formatString("count %i", 4) << std::endl;

  return EXIT_SUCCESS;
}