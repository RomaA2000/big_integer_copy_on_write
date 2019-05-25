
#ifndef BIGINTEGER_MY_VECTOR_HPP
#define BIGINTEGER_MY_VECTOR_HPP

#include <memory>
#include <algorithm>
#include <shared_ptr.hpp>

template<typename T>
class my_vector {
  static const size_t SMALL_ARRAY_SIZE = 7;
  using sh_p = shared_ptr<T>;

 private:
  size_t v_size = 0;
  size_t capacity = SMALL_ARRAY_SIZE;
  T small[SMALL_ARRAY_SIZE];
  sh_p shp;
  T *data = small;
  void push_back_no_alloc(T const &);
  void push_back_alloc(T const &);
  void destruct(T *, const size_t, const size_t);
  void construct(T *, const size_t, const size_t);
  void resize_small(size_t);
  void resize_not_small(size_t);
  T *allocator(T const *, size_t, size_t);
  void set_small();
  void set_not_small(T *, size_t);
 public:
  my_vector() = default;
  explicit my_vector(size_t);

  ~my_vector();

  size_t size() const;
  void resize(size_t);
  bool empty() const;
  bool is_small() const;
  void clear();
  T &operator[](size_t);
  T const &operator[](size_t) const;
  T &back();
  T const &back() const;

  my_vector(my_vector const &);
  my_vector &swap(my_vector &);
  my_vector &operator=(my_vector const &);

  bool unique() const;
  void detach();

  void push_back(T const &);
  void pop_back();
  void reverse();
};

template<typename T>
T *my_vector<T>::allocator(T const *ptr, size_t size, size_t need_to) {
  T *new_data = (T *) (operator new(need_to * sizeof(T)));
  std::copy(ptr, ptr + size, new_data);
  return new_data;
}

template<typename T>
void my_vector<T>::pop_back() {
  data[v_size - 1].~T();
  --v_size;
}

template<typename T>
bool my_vector<T>::empty() const {
  return v_size == 0;
}

template<typename T>
bool my_vector<T>::is_small() const {
  return data == small;
}
template<typename T>
T &my_vector<T>::operator[](size_t i) {
  return data[i];
}
template<typename T>
T const &my_vector<T>::operator[](size_t i) const {
  return data[i];
}
template<typename T>
T &my_vector<T>::back() {
  return data[v_size - 1];
}
template<typename T>
T const &my_vector<T>::back() const {
  return data[v_size - 1];
}
template<typename T>
size_t my_vector<T>::size() const {
  return v_size;
}
template<typename T>
void my_vector<T>::resize(size_t newsize_) {
  if (newsize_ > capacity) {
    resize_not_small(newsize_);
  } else {
    resize_small(newsize_);
  }
  v_size = newsize_;
}
template<typename T>
void my_vector<T>::reverse() {
  for (size_t i = 0; i < (v_size >> (size_t) 1); ++i) {
    std::swap(data[i], data[v_size - 1 - i]);
  }
}
template<typename T>
void my_vector<T>::push_back(const T &x) {
  if (v_size >= capacity) {
    push_back_alloc(x);
  } else {
    push_back_no_alloc(x);
  }
  ++v_size;
}
template<typename T>
void my_vector<T>::detach() {
  if (!shp.unique()) {
    T *new_ptr = allocator(data, v_size, capacity);
    set_not_small(new_ptr, capacity);
  }
}
template<typename T>
bool my_vector<T>::unique() const {
  return shp.unique();
}
template<typename T>
my_vector<T> &my_vector<T>::operator=(my_vector const &rhs) {
  if (rhs.v_size <= SMALL_ARRAY_SIZE) {
    std::copy(rhs.data, rhs.data + rhs.v_size, small);
    destruct(data, 0, v_size);
    shp.reset(nullptr);
    set_small();
  } else {
    if (shp.unique()) {
      destruct(data, 0, v_size);
    }
    shp = rhs.shp;
    data = rhs.data;
    capacity = rhs.capacity;
  }
  v_size = rhs.v_size;
  return *this;
}
template<typename T>
my_vector<T> &my_vector<T>::swap(my_vector &v) {
  if (is_small() || v.is_small()) {
    if (is_small() && v.is_small()) {
      for (size_t i = 0; i < SMALL_ARRAY_SIZE; ++i) {
        std::swap(data[i], v.data[i]);
      }
    } else if (is_small()) {
      std::copy(small, small + v_size, v.small);
      std::swap(shp, v.shp);
      std::swap(data, v.data);
      capacity = v.capacity;
      v.set_small();
    } else {
      v.swap(*this);
    }
  } else {
    std::swap(shp, v.shp);
    std::swap(capacity, v.capacity);
    std::swap(data, v.data);
  }
  std::swap(v_size, v.v_size);
  return *this;
}
template<typename T>
my_vector<T>::my_vector(const my_vector &rhs) {
  if (rhs.v_size <= SMALL_ARRAY_SIZE) {
    std::copy(rhs.data, rhs.data + rhs.v_size, data);
  } else {
    data = rhs.data;
    capacity = rhs.capacity;
    shp = rhs.shp;
  }
  v_size = rhs.v_size;
}
template<typename T>
void my_vector<T>::clear() {
  my_vector tmp;
  swap(tmp);
}
template<typename T>
my_vector<T>::~my_vector() {
  destruct(data, 0, v_size);
}
template<typename T>
my_vector<T>::my_vector(size_t initial_size_) {
  if (initial_size_ <= SMALL_ARRAY_SIZE) {
    construct(data, 0, initial_size_);
  } else {
    auto alloc_data_ = static_cast<T *> (operator new(initial_size_ * sizeof(T)));
    construct(alloc_data_, 0, initial_size_);
    set_not_small(alloc_data_, initial_size_);
  }
  v_size = initial_size_;
}
template<typename T>
void my_vector<T>::resize_not_small(size_t new_size_) {
  T *alloc_data_ = allocator(data, v_size, new_size_);
  construct(alloc_data_, v_size, new_size_);
  for (size_t i = 0; i < v_size; ++i) {
    data[i].~T();
  }
  set_not_small(alloc_data_, new_size_);
}
template<typename T>
void my_vector<T>::resize_small(size_t new_size_) {
  if (new_size_ > v_size) {
    construct(data, v_size, new_size_);
  } else {
    destruct(data, new_size_, v_size);
  }
}
template<typename T>
void my_vector<T>::push_back_alloc(const T &x) {
  size_t new_capacity_ = capacity << 1;
  T *alloc_data_ = allocator(data, v_size, new_capacity_);
  new(alloc_data_ + v_size) T(x);
  destruct(data, 0, v_size);
  set_not_small(alloc_data_, new_capacity_);
}
template<typename T>
void my_vector<T>::push_back_no_alloc(const T &x) {
  new(data + v_size) T(x);
}
template<typename T>
void my_vector<T>::set_small() {
  shp.reset(nullptr);
  data = small;
  capacity = SMALL_ARRAY_SIZE;
}
template<typename T>
void my_vector<T>::set_not_small(T *allocated_, size_t new_capacity_) {
  data = allocated_;
  capacity = new_capacity_;
  shp.reset(allocated_);
}
template<typename T>
void my_vector<T>::construct(T *ptr, const size_t begin, const size_t end) {
  for (size_t i = begin; i < end; ++i) {
    ptr[i] = T();
  }
}
template<typename T>
void my_vector<T>::destruct(T *ptr, const size_t begin, const size_t end) {
  for (size_t i = begin; i < end; ++i) {
    ptr[i].~T();
  }
}
#endif //BIGINTEGER_MY_VECTOR_HPP