#include <iostream>
#include <unordered_map>
#include <vector>
struct TAC {
  std::string T;
  char op;
  std::string L;
  std::string R;

  TAC(const std::string &T, char op, const std::string &L, const std::string &R)
      : T(T), op(op), L(L), R(R) {}
};

void do_value_numbering(std::vector<TAC> &bb) {
  std::unordered_map
}

int main() {
  std::vector<TAC> bb;
  do_value_numbering(bb);
}
