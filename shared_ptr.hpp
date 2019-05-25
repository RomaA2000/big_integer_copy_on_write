//
// Created by Роман Агеев on 2019-05-16.
//

#ifndef BIGINTEGER_SHARED_PTR_HPP
#define BIGINTEGER_SHARED_PTR_HPP

template<typename T>
struct inside_ptr;

template<typename T>
struct shared_ptr {
 private:
  inside_ptr<T> * inf = nullptr;
 public:
  shared_ptr();
  ~shared_ptr();
  explicit shared_ptr(T*);
  void swap(shared_ptr &);
  shared_ptr(shared_ptr const &);
  bool unique();
  shared_ptr & operator=(shared_ptr const &);
  T* get();
  const T* get() const;
  bool empty() const;
  void reset(T * in);
};

template<typename T>
struct inside_ptr {
 public:
  uint64_t counter = 1;
  T* ptr = nullptr;
  inside_ptr() = default;
  explicit inside_ptr(T*);
  ~inside_ptr();
  inside_ptr & operator++();
  inside_ptr & operator--();
};

template<typename T>
shared_ptr<T>::shared_ptr(T *in)  {
  inf = new inside_ptr<T>(in);
}

template<typename T>
shared_ptr<T>::shared_ptr(shared_ptr const &in) : inf(in.inf) {
  if (!empty()) {
    ++(*inf);
  }
}

template<typename T>
shared_ptr<T>::~shared_ptr() {
  if (!empty()) {
    --(*inf);
    if (inf->counter == 0) {
      delete inf;
    }
  }
}
template<typename T>
shared_ptr<T> &shared_ptr<T>::operator=(shared_ptr const &in) {
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

template<typename T>
bool shared_ptr<T>::empty() const{
  return inf == nullptr;
}
template<typename T>
void shared_ptr<T>::swap(shared_ptr &in) {
  std::swap(inf, in.inf);
}
template<typename T>
bool shared_ptr<T>::unique() {
  return inf == nullptr || inf->counter <= 1;
}
template<typename T>
T *shared_ptr<T>::get() {
  return inf->ptr;
}
template<typename T>
const T *shared_ptr<T>::get() const {
  return inf->ptr;
}
template<typename T>
void shared_ptr<T>::reset(T *in) {
  shared_ptr(in).swap(*this);
}
template<typename T>
shared_ptr<T>::shared_ptr() = default;

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
