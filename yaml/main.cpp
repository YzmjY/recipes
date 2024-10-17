#include <iostream>

#include "yaml-cpp/yaml.h"

int main() {
  YAML::Emitter out;
  out << YAML::BeginSeq;
  out << "eggs";
  out << "bread";
  out << "milk";
  out << YAML::EndSeq;

  std::cout << out.c_str() << std::endl;

  return 0;
}