//
// Created by Роман Агеев on 2019-04-15.
//

#ifndef BIGINTEGER_BIGINTEGER_H
#define BIGINTEGER_BIGINTEGER_H
#include <iostream>
#include <string>
#include <algorithm>
#include "my_vector.hpp"

class big_integer {
  typedef uint32_t main_type;
  typedef int64_t signed_main_type;
  typedef uint64_t buffer_type;
  typedef int64_t signed_buffer_type;

  static const main_type MAIN_MAX = UINT32_MAX;
  static const main_type MAIN_TYPE_SIZE = 32;
  static const main_type POWER_OF_SIZE = 5;
  static const buffer_type MAIN_DIGIT = (buffer_type)MAIN_MAX + (buffer_type) 1;

  bool sgn = false;
  my_vector<main_type> data = my_vector<main_type>();

  big_integer &mod_or_div(big_integer const &, bool);
  big_integer &part_add(big_integer const &);
  big_integer &part_sub(big_integer const &);
  big_integer div_short(main_type);

  void negate();
  main_type const &operator[](size_t) const;
  main_type &operator[](size_t);
  main_type safe_get(size_t) const;
  void pop_back();
  void push_back(main_type);
  main_type const &back() const;
  main_type &back();
  bool empty() const;
  void shrink_to_fit();
  void mov_right(size_t);
  void mov_left(size_t);
  bool is_two_pow();
  void to_addition();
  void invert_bytes();
  void back_from_addition();
  void increase_to_size(size_t);
  void increase_size_on(size_t);
  static inline big_integer moved(size_t);
  int32_t compare_by_abs(big_integer const&) const;
  big_integer(my_vector<main_type> &, bool);

 public:
  big_integer();
  big_integer(big_integer const &);
  big_integer(int);
  big_integer(uint32_t);
  big_integer(uint64_t);
  explicit big_integer(std::string const &);
  ~big_integer();
  size_t size() const;
  bool is_positive() const;
  bool is_zero() const;
  bool is_one() const;
  void clear();

  void abs();

  void swap(big_integer &);
  big_integer &operator*=(main_type);
  big_integer &operator+=(main_type);
  big_integer &operator*=(int);

  big_integer &operator=(big_integer const &);

  big_integer &operator+=(big_integer const &);
  big_integer &operator-=(big_integer const &);
  big_integer &operator*=(big_integer const &);
  big_integer &operator/=(big_integer const &);
  big_integer &operator%=(big_integer const &);

  big_integer &operator&=(big_integer const &);
  big_integer &operator|=(big_integer const &);
  big_integer &operator^=(big_integer const &);

  big_integer &abstract_binary(big_integer &, std::function<main_type(main_type,main_type)> const &);
  big_integer &operator<<=(int);
  big_integer &operator>>=(int);

  big_integer operator+() const;
  big_integer operator-() const;
  big_integer operator~() const;

  big_integer &operator++();
  const big_integer operator++(int);

  big_integer &operator--();
  const big_integer operator--(int);

  friend bool operator==(big_integer const &, big_integer const &);
  friend bool operator!=(big_integer const &, big_integer const &);
  friend bool operator<(big_integer const &, big_integer const &);
  friend bool operator>(big_integer const &, big_integer const &);
  friend bool operator<=(big_integer const &, big_integer const &);
  friend bool operator>=(big_integer const &, big_integer const &);

  friend big_integer operator|(big_integer , big_integer const &);
  friend big_integer operator&(big_integer , big_integer const &);
  friend big_integer operator^(big_integer , big_integer const &);

  friend big_integer operator+(big_integer , big_integer const &);
  friend big_integer operator-(big_integer , big_integer const &);
  friend big_integer operator*(big_integer , big_integer const &);
  friend big_integer operator/(big_integer , big_integer const &);
  friend big_integer operator%(big_integer , big_integer const &);

  friend big_integer operator<<(big_integer , int);
  friend big_integer operator>>(big_integer , int);

  friend big_integer abs(big_integer const &);

  friend std::ostream &operator<<(std::ostream &, const big_integer &);
  friend std::istream &operator>>(std::istream &, big_integer &);

  friend std::string to_string(big_integer in);
};

big_integer big_integer::moved(size_t in) {
  my_vector<main_type> out(in + 1);
  out.back() = (main_type) 1;
  return big_integer(out, false);
}

big_integer &big_integer::operator+=(main_type in) {
  this->data.detach();
  auto now = (buffer_type) in;
  for (size_t i = 0; i < size(); ++i) {
    now += (buffer_type) data[i];
    data[i] = (main_type) (now & MAIN_MAX);
    now >>= MAIN_TYPE_SIZE;
  }
  while (now > 0) {
    push_back((main_type) (now & MAIN_MAX));
    now >>= MAIN_TYPE_SIZE;
  }
  shrink_to_fit();
  return *this;
}

big_integer &big_integer::operator*=(main_type in) {
  this->data.detach();
  buffer_type now = 0;
  for (size_t i = 0; i < size(); ++i) {
    now += (buffer_type) data[i] * (buffer_type) in;
    data[i] = (main_type) (now & MAIN_MAX);
    now >>= MAIN_TYPE_SIZE;
  }
  while (now > 0) {
    push_back((main_type) (now & MAIN_MAX));
    now >>= MAIN_TYPE_SIZE;
  }
  shrink_to_fit();
  return *this;
}

big_integer &big_integer::operator*=(int in) {
  this->data.detach();
  main_type input = (main_type)std::abs(in);
  *this *= input;
  sgn ^= (in < 0);
  return *this;
}

big_integer big_integer::div_short(main_type in) {
  this->data.detach();
  buffer_type now = 0, carry_flag = 0;
  main_type mod = 0;
  const main_type m_t_size_mod_in = (main_type) (MAIN_DIGIT % in);
  for (size_t i = data.size(); i--;) {
    now = (buffer_type) data[i] + carry_flag * MAIN_DIGIT;
    mod = (main_type) (((buffer_type) data[i] + (buffer_type) m_t_size_mod_in * (buffer_type) mod) % in);
    carry_flag = now % in;
    data[i] = (main_type) (now / in);
  }
  shrink_to_fit();
  big_integer out = big_integer(mod);
  out.sgn = sgn;
  return out;
}

big_integer &big_integer::part_add(big_integer const &in) {
  this->data.detach();
  buffer_type now = 0;
  increase_to_size(in.size());
  size_t j = in.size();
  for (size_t i = 0; i < in.size(); ++i) {
    now += (buffer_type) data[i] + (buffer_type) in[i];
    data[i] = (main_type) (now & MAIN_MAX);
    now >>= MAIN_TYPE_SIZE;
  }
  while (now > 0) {
    increase_to_size(j + 1);
    now += (buffer_type) data[j];
    data[j] = (main_type) (now & MAIN_MAX);
    now >>= MAIN_TYPE_SIZE;
  }
  shrink_to_fit();
  return *this;
}

big_integer &big_integer::part_sub(big_integer const &in) {
  this->data.detach();
  signed_buffer_type now = 0;
  increase_to_size(in.size());
  size_t j = in.size();
  for (size_t i = 0; i < in.size(); ++i) {
    now += (signed_buffer_type) data[i] - (signed_buffer_type) in[i];
    if (now >= 0) {
      data[i] = (main_type) (now);
      now = 0;
    } else {
      data[i] = (main_type) (now + (signed_buffer_type) MAIN_DIGIT);
      now = -1;
    }
  }
  increase_to_size(in.size() + 1);
  while (now != 0) {
    now += (signed_buffer_type) data[j];
    if (now >= 0) {
      data[j] = (main_type) (now);
      now = 0;
    } else {
      data[j] = (main_type) (now + MAIN_DIGIT);
      now = -1;
    }
  }
  shrink_to_fit();
  return *this;
}

void big_integer::negate() {
  sgn = !sgn;
}

big_integer::main_type const &big_integer::operator[](size_t i) const {
  return data[i];
}

big_integer::main_type &big_integer::operator[](size_t i) {
  return data[i];
}

void big_integer::pop_back() {
  data.pop_back();
}

void big_integer::push_back(big_integer::main_type in) {
  data.push_back(in);
}

big_integer::main_type const &big_integer::back() const {
  return data.back();
}

big_integer::main_type &big_integer::back() {
  return data.back();
}

big_integer::big_integer() : sgn(false), data() {}

big_integer::big_integer(my_vector<main_type> &v, bool sgn = false) : sgn(sgn), data(v) {
  shrink_to_fit();
}

big_integer::big_integer(const big_integer &in) = default;

big_integer::big_integer(int in) {
  signed_buffer_type out = in;
  clear();
  sgn = out < 0;
  push_back((main_type) (sgn ? -out : out));
  shrink_to_fit();
}

big_integer::big_integer(uint32_t in) {
  clear();
  data.push_back((main_type) in);
  sgn = false;
}

big_integer::big_integer(uint64_t in) {
  clear();
  push_back((main_type) in);
  auto new_in = (main_type) (in >> 32);
  if (new_in > 0) {
    push_back(new_in);
  }
  sgn = false;
}

big_integer::big_integer(std::string const &in) {
  clear();
  push_back(0);
  bool wassgn = false;
  for (char i : in) {
    if (isdigit(i)) {
      *this *= ((main_type) 10);
      *this += ((main_type) i - '0');
    } else if (i == '-') {
      wassgn = true;
    } else if (i == '+') {
      wassgn = false;
    } else {
      assert(false);
    }
  }
  sgn = wassgn;
  if (sgn && empty()) {
    sgn = false;
  }
  shrink_to_fit();
}

big_integer::~big_integer() = default;

size_t big_integer::size() const {
  return data.size();
}

bool big_integer::is_positive() const {
  return !sgn;
}

bool big_integer::is_zero() const {
  return (!sgn) && (empty());
}

bool big_integer::is_one() const {
  return (size() == 1) && (data[0] == 1);
}

bool big_integer::empty() const {
  return data.empty();
}

bool big_integer::is_two_pow() {
  if (back() != 1) return false;
  for (size_t i = 0; i < size() - 1; ++i) {
    if (data[i] != 0) {
      return false;
    }
  }
  return true;
}

void big_integer::to_addition() {
  this->data.detach();
  if (sgn && !is_zero()) {
    invert_bytes();
    sgn = false;
    ++(*this);
    sgn = true;
  }
}

void big_integer::back_from_addition() {
  this->data.detach();
  if (sgn && !is_zero()) {
    sgn = false;
    --(*this);
    sgn = true;
    invert_bytes();
  }
}

void big_integer::invert_bytes() {
  this->data.detach();
  for (size_t i = 0; i < size(); ++i) {
    data[i] = ~data[i];
  }
}

void big_integer::increase_to_size(size_t in) {
  this->data.detach();
  shrink_to_fit();
  if (size() < in) {
    data.resize(in);
  }
}

void big_integer::increase_size_on(size_t in) {
  increase_to_size(size() + in);
}

void big_integer::shrink_to_fit() {
  while (!(empty()) && (back() == 0)) {
    pop_back();
  }
  if (empty()) {
    sgn = false;
  }
}

void big_integer::clear() {
  data.clear();
}

void big_integer::mov_right(size_t n = 1) {
  this->data.detach();
  size_t new_size = size() - n;
  for (size_t i = 0; i < new_size; ++i) {
    data[i] = data[i + n];
  }
  if (new_size > 0) {
    data.resize(new_size);
  } else {
    data.clear();
  }
  shrink_to_fit();
}

big_integer &big_integer::operator=(big_integer const &in) = default;

big_integer &big_integer::abstract_binary(big_integer &in, std::function<main_type(main_type,main_type)> const & f) {
  this->data.detach();
  size_t max_size = std::max(size(), in.size());
  to_addition();
  in.to_addition();
  increase_to_size(max_size);
  in.increase_to_size(max_size);
  for (size_t i = 0; i < max_size; ++i) {
    data[i] = f(data[i], in[i]);
  }
  sgn = (bool) f((main_type) sgn, (main_type) in.sgn);
  shrink_to_fit();
  back_from_addition();
  return (*this);
}

big_integer &big_integer::operator&=(big_integer const &in) {
  big_integer in_new(in);
  abstract_binary(in_new, std::bit_and<>());
  return *(this);
}

big_integer &big_integer::operator|=(big_integer const &in) {
  big_integer in_new(in);
  abstract_binary(in_new, std::bit_or<>());
  return *(this);
}

big_integer &big_integer::operator^=(big_integer const &in) {
  big_integer in_new(in);
  abstract_binary(in_new, std::bit_xor<>());
  return *(this);
}

bool operator==(big_integer const &in1, big_integer const &in2) {
  if ((in1.sgn != in2.sgn) || (in1.size() != in2.size())) {
    return false;
  }
  for (size_t i = 0; i < in1.size(); ++i) {
    if (in1[i] != in2[i]) {
      return false;
    }
  }
  return true;
}

bool operator!=(big_integer const &in1, big_integer const &in2) {
  return !(in1 == in2);
}

bool operator<(big_integer const &in1, big_integer const &in2) {
  if (in1.sgn ^ in2.sgn) {
    return in1.sgn > in2.sgn;
  }
  return (in1.sgn) ^ (in1.compare_by_abs(in2) > 0);
}

bool operator>(big_integer const &in1, big_integer const &in2) {
  return (in2 < in1);
}

bool operator<=(big_integer const &in1, big_integer const &in2) {
  return !(in1 > in2);
}

bool operator>=(big_integer const &in1, big_integer const &in2) {
  return !(in1 < in2);
}

big_integer abs(big_integer const &in) {
  big_integer new_b = in;
  new_b.sgn = false;
  return new_b;
}

big_integer &big_integer::operator+=(big_integer const &in) {
  this->data.detach();
  if (in.is_zero()) {
    return (*this);
  } else if (is_zero()) {
    (*this) = in;
  } else if (sgn == in.sgn) {
    part_add(in);
  } else if (sgn) {
    negate();
    if (*this > in) {
      part_sub(in);
      negate();
    } else {
      big_integer now = in;
      now.part_sub(*this);
      swap(now);
    }
  } else {
    big_integer now = in;
    now.negate();
    if (*this > now) {
      part_sub(now);
    } else {
      now.part_sub(*this);
      now.sgn ^= 1;
      swap(now);
    }
  }
  shrink_to_fit();
  return (*this);
}

big_integer &big_integer::operator-=(big_integer const &in) {
  this->data.detach();
  *this += (-in);
  return (*this);
}

void big_integer::mov_left(size_t in = 1) {
  this->data.detach();
  size_t last = size();
  data.resize(last + in);
  for (size_t i = last; last != 0 && i--;) {
    data[i + in] = data[i];
  }
  for (size_t i = in; in != 0 && i--;) {
    data[i] = 0;
  }
  shrink_to_fit();
}

big_integer &big_integer::operator*=(big_integer const &in) {
  this->data.detach();
  big_integer ans;
  if (is_zero() || in.is_zero()) {
    return *this = 0;
  }
  ans.increase_size_on(in.size() + size());
  buffer_type now = 0;
  for (size_t i = 0; i < size(); ++i) {
    size_t k = i + in.size();
    for (size_t j = 0; j < in.size(); ++j) {
      now += (buffer_type) ans[j + i] +((buffer_type) data[i])* in[j];
      ans[j + i] = (main_type) (now & MAIN_MAX);
      now >>= MAIN_TYPE_SIZE;
    }
    while (now > 0) {
      now += (buffer_type) ans[k];
      ans[k++] = (main_type) (now & MAIN_MAX);
      now >>= MAIN_TYPE_SIZE;
    }
  }
  ans.shrink_to_fit();
  ans.sgn = sgn ^ in.sgn;
  swap(ans);
  return (*this);
}

big_integer &big_integer::mod_or_div(big_integer const &in, bool mode = false) {
  this->data.detach();
  if (is_zero() || (compare_by_abs(in) == 1)) {
    if (mode) {
      *this = in;
    } else {
      *this = big_integer();
    }
    return *this;
  }
  bool ans_sgn = sgn ^in.sgn;
  bool start_sgn = sgn;
  this->abs();
  if (in.size() == 1) {
    big_integer mod = div_short(in[0]);
    sgn = ans_sgn;
    if (mode) {
      *this = mod;
      this->sgn = start_sgn;
    }
    shrink_to_fit();
    return *this;
  }
  if (compare_by_abs(in) == 0) {
    return mode ? *this = big_integer() : *this = (ans_sgn ? big_integer(-1) : big_integer(1));
  }
  big_integer ans;
  big_integer divisor = in;
  main_type norm = (main_type) ((buffer_type) MAIN_DIGIT / (divisor.back() + 1));
  *this *= norm;
  divisor *= norm;
  divisor.abs();
  size_t n = divisor.size();
  size_t m = size() - n;
  big_integer now = divisor;
  now.mov_left(m);
  if (*this >= now) {
    ans.push_back(1);
    *this -= now;
  }
  now.mov_right();
  buffer_type div;
  for (size_t i = m; i--; now.mov_right()) {
    div = 0;
    div += (safe_get(n + i - 1));
    div += (((buffer_type) safe_get(n + i)) * MAIN_DIGIT);
    div /= divisor.back();
    if (div > MAIN_MAX) {
      div = std::numeric_limits<main_type>::max();
    }
    big_integer tmp(now);
    tmp *= (main_type) (div & MAIN_MAX);
    *this -= tmp;
    while (*this < 0) {
      --div;
      *this += now;
    }
    ans.push_back((main_type) (div & MAIN_MAX));
  }
  ans.data.reverse();
  ans.sgn = ans_sgn;
  ans.shrink_to_fit();
  this->div_short(norm);
  this->sgn = start_sgn;
  shrink_to_fit();
  if (mode) {
    return *this;
  } else {
    swap(ans);
    return *this;
  }
}

big_integer &big_integer::operator/=(big_integer const &in) {
  return mod_or_div(in);
}

big_integer &big_integer::operator%=(big_integer const &in) {
  return mod_or_div(in, true);
}

void big_integer::swap(big_integer &in) {
  this->data.detach();
  in.data.detach();
  std::swap(data, in.data);
  std::swap(sgn, in.sgn);
}

big_integer big_integer::operator+() const {
  big_integer ans = *this;
  return ans;
}

big_integer big_integer::operator-() const {
  if (!is_zero()) {
    big_integer ans = *this;
    ans.sgn ^= 1;
    return ans;
  } else {
    return *this;
  }
}

big_integer big_integer::operator~() const {
  return -*this - 1;
}

void big_integer::abs() {
  this->data.detach();
  sgn = false;
}

big_integer &big_integer::operator++() {
  this->data.detach();
  *this += big_integer(1);
  return *this;
}
const big_integer big_integer::operator++(int) {
  this->data.detach();
  const big_integer tmp = *this;
  ++(*this);
  return tmp;
}
big_integer &big_integer::operator--() {
  this->data.detach();
  *this -= 1;
  return *this;
}

const big_integer big_integer::operator--(int) {
  this->data.detach();
  const big_integer tmp = *this;
  --(*this);
  return tmp;
}

big_integer operator|(big_integer in1, big_integer const &in2) {
  return in1 |= in2;
}

big_integer operator&(big_integer in1, big_integer const &in2) {
  return in1 &= in2;
}

big_integer operator^(big_integer in1, big_integer const &in2) {
  return in1 ^= in2;
}

big_integer operator+(big_integer in1, big_integer const &in2) {
  return in1 += in2;
}

big_integer operator-(big_integer in1, big_integer const &in2) {
  return in1 -= in2;
}

big_integer operator*(big_integer in1, big_integer const &in2) {
  return in1 *= in2;
}

big_integer operator/(big_integer in1, big_integer const &in2) {
  return in1 /= in2;
}

big_integer operator%(big_integer in1, big_integer const &in2) {
  return in1 %= in2;
}

std::string to_string(big_integer in) {
  in.data.detach();
  if (in.is_zero()) {
    return "0";
  }
  bool sg = in.sgn;
  std::string ans;
  ans.reserve(in.size() * 10);
  big_integer now;
  while (!in.is_zero()) {
    now = in.div_short(10);
    const big_integer::main_type i = now[0];
    ans.push_back(char('0' + i % 10));
    in.shrink_to_fit();
  }
  while (!ans.empty() && ans.back() == '0')
    ans.pop_back();
  if (sg) {
    ans.push_back('-');
  }
  std::reverse(ans.begin(), ans.end());
  return ans;
}
big_integer &big_integer::operator<<=(int in) {
  this->data.detach();
  if (in < 0) {
    return *this >>= (-in);
  } else {
    bool sg = sgn;
    sgn = false;
    to_addition();
    buffer_type now = 0;
    size_t ans = (size_t) in >> POWER_OF_SIZE;
    in &= (MAIN_TYPE_SIZE - 1);
    for (size_t i = 0; i < size(); ++i) {
      now |= ((buffer_type) data[i]) << in;
      data[i] = (main_type) (now & MAIN_MAX);
      now >>= MAIN_TYPE_SIZE;
    }
    if (now > 0) {
      push_back((main_type) now);
    }
    shrink_to_fit();
    *this *= moved(ans);
    sgn = sg;
    return *this;
  }
}
big_integer &big_integer::operator>>=(int in) {
  this->data.detach();
  if (in < 0) {
    return *this >>= (-in);
  } else {
    bool sg = sgn;
    sgn = false;
    to_addition();
    buffer_type now = 0;
    size_t ans = (size_t) in >> POWER_OF_SIZE; //because in > 0
    in &= (MAIN_TYPE_SIZE - 1);
    for (size_t i = size(); i--;) {
      buffer_type temp = data[i];
      data[i] = (main_type) ((now << (MAIN_TYPE_SIZE - in)) + (data[i] >> in));
      now = (((buffer_type) 1 << in) - 1) & temp;
    }
    shrink_to_fit();
    if (sg && !is_two_pow()) {
      ++*this;
    }
    shrink_to_fit();
    *this *= moved(ans);
    sgn = sg;
    return *this;
  }
}
big_integer operator<<(big_integer in, int i) {
  return in <<= i;
}
big_integer operator>>(big_integer in, int i) {
  return in >>= i;
}

std::ostream &operator<<(std::ostream &s, const big_integer &in) {
  s << to_string(in);
  return s;
}

std::istream &operator>>(std::istream &s, big_integer &in) {
  std::string source;
  s >> source;
  in = big_integer(source);
  return s;
}

int32_t big_integer::compare_by_abs(big_integer const &in) const {
  if (size() != in.size()) {
    return (size() < in.size() ? 1 : -1);
  }
  for (size_t i = size(); i--;) {
    if (data[i] != in.data[i]) {
      return data[i] < in.data[i] ? 1 : -1;
    }
  }
  return 0;
}

big_integer::main_type big_integer::safe_get(size_t in) const {
  return (in < size()) ? data[in] : 0;
}
#endif //BIGINTEGER_BIGINTEGER_H
