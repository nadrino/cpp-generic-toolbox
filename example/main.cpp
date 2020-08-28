//
// Created by Adrien BLANCHET on 28/08/2020.
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

  std::cout << GenericToolbox::formatString("count %i", 4) << std::endl;

  return EXIT_SUCCESS;
}