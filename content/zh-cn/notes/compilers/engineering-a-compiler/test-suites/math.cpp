#include <iostream>

template <typename T>
struct Storage {
  T *data;
  ssize_t len;

  Storage(ssize_t len) : len(len), data(new T[len]) {}
  ~Storage() {
    if (data == nullptr) {
      return;
    }
    delete[] data;
  }
};

template <typename T>
class Vector : private Storage<T> {
 public:
  Vector(ssize_t dim, const T &val = T());

  T &at(ssize_t i);
  const T &at(ssize_t i) const;

  T &operator[](ssize_t i);
  const T &operator[](ssize_t i) const;

  Vector operator*(const Vector &rhs) const;

  friend std::ostream &operator<<(std::ostream &out, const Vector &v);
};

template <typename T>
class Matrix : private Storage<T> {
 public:
  Matrix(ssize_t n_rows, ssize_t n_cols) : n_rows_(n_rows), n_cols_(n_cols) {}
  T **data;
  ssize_t n_rows_, n_cols_;
};
`
