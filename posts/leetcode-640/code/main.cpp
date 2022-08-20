#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


class Solution {
 public:
  std::string solveEquation(const std::string &equation) {
    int k = 0, m = 0;
    int sign = 1;
    size_t pos = 0;
    char c;
    // left on '='
    while ((c = equation[pos]) != '=') {
      if (c == '-') {
        sign = -1;
        pos++;
      } else if (c == '+') {
        sign = 1;
        pos++;
      } else {
        // term ::= number | number "x" | "x"
        int factor = 1;
        if (std::isdigit(c)) {
          factor = parse_number(equation, pos);
        }
        c = equation[pos];
        if (c == 'x') {
          k += sign * factor;
          pos++;
        } else {
          m -= sign * factor;
        }
      }
    }
    pos++;
    sign = 1;
    // right on '='
    while (pos < equation.length()) {
      c = equation[pos];
      if (c == '-') {
        sign = -1;
        pos++;
      } else if (c == '+') {
        sign = 1;
        pos++;
      } else {
        int factor = 1;
        if (std::isdigit(c)) {
          factor = parse_number(equation, pos);
        }
        if (pos == equation.length()) {
          m += sign * factor;
        } else {
          c = equation[pos];
          if (c == 'x') {
            k -= sign * factor;
            pos++;
          } else {
            m += sign * factor;
          }
        }
      }
    }

    // transform the equation to form 'kx = m'
    if (k == 0 && m == 0) {
      return "Infinite solutions";
    } else if (k == 0 && m != 0) {
      return "No solution";
    } else {
      int x = m / k;
      std::ostringstream oss;
      oss << "x=" << x;
      return oss.str();
    }
  }

 private:
  // parse_number parses a number in equation starting from pos and stops at
  // the first non-digit character
  // Preconditions:
  // - 0 <= pos < equation.length()
  // - equation[pos] is a digit
  int parse_number(const std::string &equation, size_t &pos) {
    int ans = 0;
    char c;
    while (pos < equation.length() && isdigit(c = equation[pos])) {
      ans = ans * 10 + (c - '0');
      pos++;
    }
    return ans;
  }
};

int main() {
  struct test_case_t {
    std::string equation;
    std::string want;
    test_case_t(const std::string &e, const std::string &w)
        : equation(e), want(w) {}
  };

  std::vector<test_case_t> test_cases = {
      {"x+5-3+x=6+x-2", "x=2"}, {"x=x", "Infinite solutions"},
      {"2x=x", "x=0"},          {"x=2", "x=2"},
      {"2x+3x-6x=x+2", "x=-1"}, {"x=1+2+3+4", "x=10"},
      {"x+2+2x+4=x", "x=-3"},
  };
  Solution s;
  for (auto &tc : test_cases) {
    std::string got = s.solveEquation(tc.equation);
    if (got != tc.want) {
      std::cout << "equation: " << tc.equation << ", got: " << got
                << ", want: " << tc.want << std::endl;
    }
  }
}