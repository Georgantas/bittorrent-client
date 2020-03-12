
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
  std::vector<std::string> arguments(argv + 1, argv + argc);
  for (auto arg : arguments) {
    std::cout << arg << " ";
  }
}
