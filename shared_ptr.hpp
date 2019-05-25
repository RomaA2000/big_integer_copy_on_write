//
// Created by Роман Агеев on 2019-05-16.
//

#ifndef BIGINTEGER_SHARED_PTR_HPP
#define BIGINTEGER_SHARED_PTR_HPP

template<typename T>
struct inside_ptr;

template<typename T, typename = typename std::enable_if<std::is_default_constructible<T>::value>::type>
struct shared_ptr {
 private:
  inside_ptr<T> *inf = nullptr;
 public:

  explicit shared_ptr(T *in) {
    inf = new inside_ptr<T>(in);
  }

  shared_ptr(shared_ptr const &in) : inf(in.inf) {
    if (!empty()) {
      ++(*inf);
    }
  }

  ~shared_ptr() {
    if (!empty()) {
      --(*inf);
      if (inf->counter == 0) {
        delete inf;
      }
    }
  }

  shared_ptr &operator=(shared_ptr const &in) {
    if (inf != in.inf) {
      if (!empty()) {
        --(*inf);
        if (inf->counter == 0) {
          delete inf;
        }
      }
      if (!in.empty()) {
        ++(*in.inf);
      }
      inf = in.inf;
    }
    return (*this);
  }

  bool empty() const {
    return inf == nullptr;
  }

  void swap(shared_ptr &in) {
    std::swap(inf, in.inf);
  }

  bool unique() {
    return inf == nullptr || inf->counter <= 1;
  }

  T *get() {
    return inf->ptr;
  }

  const T *get() const {
    return inf->ptr;
  }

  void reset(T *in) {
    shared_ptr(in).swap(*this);
  }

  shared_ptr() = default;

};

template<typename T>
struct inside_ptr {
 public:
  uint64_t counter = 1;
  T *ptr = nullptr;
  inside_ptr() = default;
  explicit inside_ptr(T *);
  ~inside_ptr();
  inside_ptr &operator++();
  inside_ptr &operator--();
};

template<typename T>
inside_ptr<T>::inside_ptr(T *in) : ptr(in) {}

template<typename T>
inside_ptr<T> &inside_ptr<T>::operator++() {
  ++counter;
  return (*this);
}

template<typename T>
inside_ptr<T> &inside_ptr<T>::operator--() {
  --counter;
  return (*this);
}

template<typename T>
inside_ptr<T>::~inside_ptr() {
  delete ptr;
}

#endif //BIGINTEGER_SHARED_PTR_HPP
