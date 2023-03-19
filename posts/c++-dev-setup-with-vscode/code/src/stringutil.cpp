#include "stringutil.hpp"

#include <algorithm>
#include <string>

auto demo::reverse(const std::string &s) -> std::string {
  std::string ans = s;
  std::reverse(ans.begin(), ans.end());
  return std::move(ans);
}
