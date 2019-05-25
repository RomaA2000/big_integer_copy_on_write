
#ifndef BIGINTEGER_MY_VECTOR_HPP
#define BIGINTEGER_MY_VECTOR_HPP

#include <memory>
#include <algorithm>
#include <shared_ptr.hpp>
#include <type_traits>
template <typename T, typename>
struct my_vector;

template<typename T, typename = typename std::enable_if<std::is_default_constructible<T>::value>::type>
struct my_vector {
 private:
  static const size_t SMALL_ARRAY_SIZE = 6;
  using sh_p = shared_ptr<T>;

  size_t v_size = 0;
  size_t capacity = SMALL_ARRAY_SIZE;
  T small[SMALL_ARRAY_SIZE];
  sh_p shp;
  T *data = small;

  T * allocator(T const *ptr, size_t size, size_t need_to) {
    T *new_data = (T *) (operator new(need_to * sizeof(T)));
    std::copy(ptr, ptr + size, new_data);
    return new_data;
  }


  void resize_not_small(size_t in) {
    T *new_data = allocator(data, v_size, in);
    construct(new_data, v_size, in);
    for (size_t i = 0; i < v_size; ++i) {
      data[i].~T();
    }
    set_not_small(new_data, in);
  }

  void resize_small(size_t new_size) {
    if (new_size > v_size) {
      construct(data, v_size, new_size);
    } else {
      destruct(data, new_size, v_size);
    }
  }

  void push_back_alloc(const T &x) {
    size_t new_size = capacity << (size_t)1;
    T *new_data = allocator(data, v_size, new_size);
    new(new_data + v_size) T(x);
    destruct(data, 0, v_size);
    set_not_small(new_data, new_size);
  }

  void push_back_no_alloc(const T &in) {
    new(data + v_size) T(in);
  }

  void set_small() {
    shp.reset(nullptr);
    capacity = SMALL_ARRAY_SIZE;
    data = small;
  }

  void set_not_small(T *in, size_t new_capacity) {
    capacity = new_capacity;
    data = in;
    shp.reset(in);
  }

  void construct(T *ptr, const size_t begin, const size_t end) {
    for (size_t i = begin; i < end; ++i) {
      ptr[i] = T();
    }
  }

  void destruct(T *ptr, const size_t begin, const size_t end) {
    for (size_t i = begin; i < end; ++i) {
      ptr[i].~T();
    }
  }

 public:

  my_vector() = default;


  my_vector(const my_vector &in) {
    if (in.v_size <= SMALL_ARRAY_SIZE) {
      std::copy(in.data, in.data + in.v_size, data);
    } else {
      data = in.data;
      capacity = in.capacity;
      shp = in.shp;
    }
    v_size = in.v_size;
  }

  ~my_vector() {
    destruct(data, 0, v_size);
  }

  my_vector(size_t in) {
    if (in <= SMALL_ARRAY_SIZE) {
      construct(data, 0, in);
    } else {
      auto alloc_data_ = (T *) (operator new(in * sizeof(T)));
      construct(alloc_data_, 0, in);
      set_not_small(alloc_data_, in);
    }
    v_size = in;
  }

  void clear() {
    my_vector tmp;
    swap(tmp);
  }

  void pop_back() {
    data[v_size - 1].~T();
    --v_size;
  }

  bool empty() const {
    return v_size == 0;
  }

  bool is_small() const {
    return data == small;
  }

  T & operator[](size_t i) {
    return data[i];
  }

  T const & operator[](size_t i) const {
    return data[i];
  }

  T & back() {
    return data[v_size - 1];
  }

  T const & back() const {
    return data[v_size - 1];
  }

  size_t size() const {
    return v_size;
  }

  void resize(size_t in) {
    if (in > capacity) {
      resize_not_small(in);
    } else {
      resize_small(in);
    }
    v_size = in;
  }

  void reverse() {
    for (size_t i = 0; i < (v_size >> (size_t) 1); ++i) {
      std::swap(data[i], data[v_size - 1 - i]);
    }
  }

  void push_back(const T &in) {
    if (v_size >= capacity) {
      push_back_alloc(in);
    } else {
      push_back_no_alloc(in);
    }
    ++v_size;
  }

  void detach() {
    if (!shp.unique()) {
      T *new_ptr = allocator(data, v_size, capacity);
      set_not_small(new_ptr, capacity);
    }
  }

  bool unique() const {
    return shp.unique();
  }

  my_vector & operator=(my_vector const &in) {
    if (in.v_size <= SMALL_ARRAY_SIZE) {
      std::copy(in.data, in.data + in.v_size, small);
      destruct(data, 0, v_size);
      shp.reset(nullptr);
      set_small();
    } else {
      if (shp.unique()) {
        destruct(data, 0, v_size);
      }
      shp = in.shp;
      data = in.data;
      capacity = in.capacity;
    }
    v_size = in.v_size;
    return *this;
  }

  my_vector &swap(my_vector &in) {
    if (is_small() || in.is_small()) {
      if (is_small() && in.is_small()) {
        for (size_t i = 0; i < SMALL_ARRAY_SIZE; ++i) {
          std::swap(data[i], in.data[i]);
        }
      } else if (is_small()) {
        std::copy(small, small + v_size, in.small);
        std::swap(shp, in.shp);
        std::swap(data, in.data);
        capacity = in.capacity;
        in.set_small();
      } else {
        in.swap(*this);
      }
    } else {
      std::swap(shp, in.shp);
      std::swap(capacity, in.capacity);
      std::swap(data, in.data);
    }
    std::swap(v_size, in.v_size);
    return *this;
  }
};
#endif //BIGINTEGER_MY_VECTOR_HPP