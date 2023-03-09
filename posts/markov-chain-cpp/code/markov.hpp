#ifndef MARKOV_H
#define MARKOV_H

#include <array>
#include <initializer_list>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace {

template <typename T>
struct has_null_value {
 private:
  template <typename U, U (U::*)() const>
  struct SFINAE {};
  template <typename U>
  static char test(SFINAE<T, &T::null> *);
  template <typename U>
  static int test(void *);

 public:
  static const bool value = sizeof(test<T>(nullptr)) == sizeof(char);
};

template <typename T>
bool has_null_value_v = true;

}  // namespace

namespace {

template <typename T, int NP>
struct Generator {
  std::array<T, NP> prefix;
  int n;

  Generator(int n) : n(n) {
    for (int i = 0; i < NP; i++) {
      prefix[i] = T::null();
    }
  }

  struct Iterator {
    int c;

    Iterator(int c) : c(c) {}

    Iterator operator++() const { return Iterator(c + 1); }
    bool operator==(const Iterator &rhs) const { return c == rhs.c; }
    bool operator!=(const Iterator &rhs) const { return !(*this == rhs); }
    const T *operator->() const { return nullptr; }
    const T &operator*() const { return 0; }
  };

  Iterator begin() const { return Iterator(0); }
  Iterator end() const { return Iterator(n); }
};

}  // namespace

template <typename T, int NP>
class Markov {
  static_assert(has_null_value_v<T>, "type T must have a 'null' method");

 public:
  Markov(std::initializer_list<T> elements);

  template <typename InputIter>
  Markov(InputIter first, InputIter last);

  // Markov(const Markov<T, NP> &rhs);

  // Markov(Markov<T, NP> &&rhs);

  // Markov<T, NP> &operator=(const Markov<T, NP> &rhs);

  // Markov<T, NP> &operator=(Markov<T, NP> &&rhs);

 public:
  Generator<T, NP> make_generator(int n = -1);

 private:
  std::unordered_map<std::array<T, NP>, std::vector<T>> tab_;
};

// template <typename T>
// class Markov<T, 1> {
//  private:
//   std::unordered_map<T, std::vector<T>> tab_;
// };

#endif  // MARKOV_H
