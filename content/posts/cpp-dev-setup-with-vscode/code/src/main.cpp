#include <iostream>
#include <string>

#include "stringutil.hpp"

int main(int argc, char *argv[]) {
  std::string s;
  if (argc < 2) {
    s = "hello, world";
  } else {
    s = argv[1];
  }
  std::cout << "original: " << s << '\n'
            << "reversed: " << demo::reverse(s) << std::endl;
}
