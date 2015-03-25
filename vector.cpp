#include <initializer_list>
#include <type_traits>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <stdexcept>
#include <numeric>
#include <ctime>
#include <string>
#include <cassert>
using namespace std;

#define eprintf(...)  fprintf(stderr, __VA_ARGS__)
#define TIMESTAMP(x)  eprintf("[" #x "] Time = %.5lfs\n",clock()*1.0/CLOCKS_PER_SEC)

struct __timestamper {
  ~__timestamper() {
    TIMESTAMP(END);
  }
} __Timestamper;

typedef unsigned int size_type;

template <typename _Tp, typename _Allocator = allocator<_Tp> >
class Vector {
private:
  _Allocator alloc;
  
  _Tp* buffer;
  
  size_type __size;
  size_type __capacity;
  
public:
  
  explicit Vector(const _Allocator& __a = _Allocator()) :
		alloc(__a), __size(0), __capacity(0) { buffer = alloc.allocate(0); }
  
  explicit Vector(size_type __n, const _Allocator& __a = _Allocator()) :
		alloc(__a), __size(__n), __capacity(__n) {
      buffer = alloc.allocate(__n);
      uninitialized_fill_n(buffer, __n, _Tp());
    }
  
  explicit Vector(size_type __n, const _Tp& value, const _Allocator& __a = _Allocator()) :
		alloc(__a), __size(__n), __capacity(__n) {
      buffer = alloc.allocate(__n);
      uninitialized_fill_n(buffer, __n, value);
    }
  
  explicit Vector(const Vector<_Tp, _Allocator>& __v) :
  __size(0), __capacity(0)
  {
    buffer = alloc.allocate(0);
    for (auto it = __v.begin(); it != __v.end(); it++) {
      push_back(*it);
    }
  }
  
  template<class InputIterator, class = typename enable_if<is_same<InputIterator, const _Tp*>::value>::type>
  Vector(InputIterator __first, InputIterator __last, const _Allocator& __a = _Allocator()) :
		alloc(__a), __size(0), __capacity(0) {
      buffer = alloc.allocate(0);
      for (; __first != __last; ++__first) {
        push_back(*__first);
      }
    }
  
  Vector(initializer_list<_Tp> __li, const _Allocator& __a = _Allocator()) :
		alloc(__a), __size(0), __capacity(0) {
      buffer = alloc.allocate(__li.size());
      for (auto _list : __li)
        push_back(_list);
      __capacity = __size;
    }
  
  ~Vector();
  
  Vector<_Tp, _Allocator>& operator=(const Vector<_Tp, _Allocator>& __o) {
    this->__size = 0;
    for (auto it = __o.begin(); it != __o.end(); it++)
      push_back(*it);
    return *this;
  }
  
  Vector<_Tp, _Allocator>& operator=(const initializer_list<_Tp> __li) {
    this->__size = 0;
    for (auto &li : __li)
      push_back(li);
    return *this;
  }
  
  void reserve(const size_type& size);
  void pop_back();
  
  void assign(size_type __n, const _Tp& value);
  void resize(size_type __n, const _Tp& value);
  
  void clear() noexcept {
    this->__size = 0;
  }
  
  const _Tp* begin() const noexcept {
    return buffer;
  }
  
  const _Tp* end() const noexcept {
    return buffer + __size;
  }
  
  _Tp& front() {
    assert(!empty());
    return buffer[0];
  }
  
  const _Tp& front() const {
    assert(!empty());
    return buffer[0];
  }
  
  _Tp& back() {
    assert(!empty());
    return buffer[__size - 1];
  }
  
  const _Tp& back() const {
    assert(!empty());
    return buffer[__size - 1];
  }
  
  const size_type size() const noexcept {
    return this->__size;
  }
  
  const size_type capacity() const noexcept {
    return __capacity;
  }
  
  const bool empty() const noexcept {
    return (__size == 0);
  }
  
  const size_type max_size() const noexcept {
    return size_type(-1) / sizeof(_Tp);
  }
  
  const _Tp& operator[](const size_type& index) const {
    return buffer[index];
  }
  
  _Tp& operator[](const size_type& index) {
    return buffer[index];
  }
  
  const _Tp& at(const size_type index) const {
    if (index < 0 || index >= __capacity)
      throw std::out_of_range("index out of range");
    return buffer[index];
  }
  
  _Tp& at(const size_type index) {
    if (index < 0 || index >= __capacity)
      throw std::out_of_range("index out of range");
    return buffer[index];
  }
  
  void push_back(const _Tp& value) {
    if (__size >= __capacity) {
      reserve(__capacity > 0 ? __capacity * 2 : 1);
    }
    alloc.construct(this->buffer + __size, value);
    ++__size;
  }
  
  void push_back(_Tp&& value) {
    if (__size >= __capacity) {
      reserve(__capacity > 0 ? __capacity * 2 : 1);
    }
    alloc.construct(this->buffer + __size, value);
    ++__size;
  }
  
  template<class... _Args>
  inline
  void emplace_back(_Args&&... value) {
    if (__size >= __capacity) {
      reserve(__capacity > 0 ? __capacity * 2 : 1);
    }
    alloc.construct(this->buffer + __size, forward<_Args>(value)...);
    ++__size;
  }
  
};

template <typename _Tp, typename _Allocator>
void Vector<_Tp, _Allocator>::reserve(const size_type& size) {
  if (size <= __capacity)
    return;
  
  _Tp* newBuffer = alloc.allocate(size);
  
  uninitialized_copy(this->buffer, this->buffer + __size, newBuffer);
  
  for (size_type i = 0; i < __size; i++) {
    alloc.destroy(&buffer[i]);
  }
  
  alloc.deallocate(this->buffer, __capacity);
  this->buffer = newBuffer;
  __capacity = size;
}


template <typename _Tp, typename _Allocator>
void Vector<_Tp, _Allocator>::pop_back() {
  if (__size == 0)
    return;
  alloc.destroy(&buffer[__size - 1]);
  --__size;
}

template<typename _Tp, typename _Allocator>
Vector<_Tp, _Allocator>::~Vector() {
  while (__size > 0) {
    pop_back();
  }
}

template <typename _Tp, typename _Allocator>
void Vector<_Tp, _Allocator>::assign(size_type __n, const _Tp& value) {
  resize(__n, value);
}

template <typename _Tp, typename _Allocator>
void Vector<_Tp, _Allocator>::resize(size_type __n, const _Tp& value) {
  _Tp* newBuffer = alloc.allocate(__n);
  
  uninitialized_fill_n(newBuffer, __n, value);
  
  for (size_type i = 0; i < __size; i++)
    alloc.destroy(buffer + i);
  
  alloc.deallocate(buffer, __capacity);
  
  this->buffer = newBuffer;
  
  __size = __capacity = __n;
}

template <typename _Tp, typename _Allocator>
inline bool
operator==(const Vector<_Tp, _Allocator>& x, const Vector<_Tp, _Allocator>& y) {
  return (x.size() == y.size() &&
          equal(x.begin(), x.end(), y.begin()));
}

template <typename _Tp, typename _Allocator>
inline bool
operator<(const Vector<_Tp, _Allocator>& x, const Vector<_Tp, _Allocator>& y) {
  return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template <typename _Tp, typename _Allocator>
inline bool
operator!=(const Vector<_Tp, _Allocator>& x, const Vector<_Tp, _Allocator>& y) {
  return !(x == y);
}

template <typename _Tp, typename _Allocator>
inline bool
operator>(const Vector<_Tp, _Allocator>& x, const Vector<_Tp, _Allocator>& y) {
  return y < x;
}

template <typename _Tp, typename _Allocator>
inline bool
operator<=(const Vector<_Tp, _Allocator>& x, const Vector<_Tp, _Allocator>& y) {
  return !(x > y);
}

template <typename _Tp, typename _Allocator>
inline bool
operator>=(const Vector<_Tp, _Allocator>& x, const Vector<_Tp, _Allocator>& y) {
  return !(x < y);
}

void TEST_CONSTRUCTOR_1() {
  cout << "Testing Empty Constructor with integers...\n";
  Vector<int> ad;
  if (!ad.size())
    cout << "Empty Constructor checked \n";
  else
    cout << "Empty Constructor failure\n";
  cout << "Test Empty Constructor with strings...\n";
  Vector<string> b;
  if (!b.size())
    cout << "Empty Constructor checked \n";
  else
    cout << "Empty Constructor failure\n";
}

void TEST_CONSTRUCTOR_2() {
  cout << "Testing Constructor with it's size initialized with 5 integers...\n";
  Vector<int> ad(5);
  if (ad.size() == 5)
    cout << "2nd Constructor checked \n";
  else
    cout << "2nd Constructor failure\n";
  Vector<string> b(5);
  cout << "Testing Constructor with it's size intialized with 5 strings...\n";
  if (b.size())
    cout << "2nd Constructor checked \n";
  else
    cout << "2nd Constructor failure\n";
}

void TEST_CONSTRUCTOR_3() {
  cout << "Testing constructor with it's size initiliazed with 5 and value = 583...\n";
  Vector<int> ad(5, 583);
  int f = 1;
  for (size_type i = 0; i < ad.size(); i++) {
    f &= ad[i] == 583;
  }
  if (ad.size() != 5 || !f)
    cout << "3rd Constructor failure\n";
  else
    cout << "3rd Constructor checked \n";
  cout << "Testing constructor with it's size initiliazed with 5 and value = boost...\n";
  Vector<string> b(5, "boost");
  int g = 1;
  for (size_type i = 0; i < b.size(); i++) {
    g &= b[i] == "boost";
  }
  if (b.size() != 5 || !g)
    cout << "3rd Constructor failure\n";
  else
    cout << "3rd Constructor checked  \n";
}

void TEST_CONSTRUCTOR_4() {
  cout << "Testing Constructor with initializer_list<int> { 5, 33, 12, 35 }...\n";
  Vector<int> ad{ 5, 33, 12, 35 };
  if (ad[0] != 5) {
    cout << "4th Constructors failure\n";
    return;
  }
  if (ad[1] != 33) {
    cout << "4th Constructors failure\n";
    return;
  }
  if (ad[2] != 12) {
    cout << "4th Constructors failure\n";
    return;
  }
  if (ad[3] != 35) {
    cout << "4th Constructors failure\n";
    return;
  }
  cout << "4th Constructors checked \n";
  cout << "\n Testing Constructor with initializer_list<string> { \"boost\", \"library\", \"Gsoc\'15\" }\n";
  Vector<string> b{ "boost", "library", "Gsoc\'15" };
  if (b[0] != "boost") { cout << "4th Constructors failure\n"; return; }
  if (b[1] != "library") { cout << "4th Constructor failure\n"; return; }
  if (b[2] != "Gsoc\'15") { cout << "4th Constructor failure\n"; return; }
  cout << "4th Constructor checked \n";
}

void TEST_CONSTRUCTOR_5() {
  cout << "Testing 5th Constructor with another Vector of integers\n";
  Vector <int> a(5, 33);
  Vector <int> b(a.begin(), a.end());
  int f = 1;
  for (size_type i = 0; i < a.size(); i++) {
    f &= b[i] == a[i];
  }
  if (a.size() != b.size() || !f)
    cout << "5th Constructor failure\n";
  else
    cout << "5th Constructor checked \n";
  cout << "Testing 5th Constructore with another Vector of strings\n";
  Vector<string> c(5, "boost");
  Vector<string> d(c.begin(), c.end());
  int g = 1;
  for (size_type i = 0; i < c.size(); i++) {
    g &= c[i] == d[i];
  }
  if (c.size() != d.size() || !g)
    cout << "5th Constructor failure\n";
  else
    cout << "5th Constructor checked \n";
}

void TEST_BEGIN_END() {
  cout << "Testing begin, end, front, back [] functions for integers...\n";
  Vector<int> a(5, 3129);
  cout << "Output of Vector using begin & end in a for loop\n";
  try {
    for (auto it = a.begin(); it != a.end(); it++)
      cout << *it << ' ';
    cout << "\n begin, end functions checked \n";
  } catch (...) {
    cout << "\n begin, end functions failure\n";
  }
  int x = a.front();
  int y = a.back();
  if (x == 3129 && y == 3129)
    cout << "front, back checked \n";
  else
    cout << "front, back failure\n";
  a[1] = 23;
  if (a[1] != 23)
    cout << "operator[] failure\n";
  else
    cout << "operator[] checked \n";
  cout << "Testing begin, end, front, back [] functions for strings...\n";
  Vector<string> b(5, "boost");
  cout << "Output of Vector using begin & end in a for loop\n";
  try {
    for (auto it = a.begin(); it != a.end(); it++)
      cout << *it << ' ';
    cout << "\n begin, end functions checked \n";
  } catch (...) {
    cout << "\n begin, end functions failure\n";
  }
  string xx = b.front();
  string yy = b.back();
  if (xx == "boost" && yy == "boost")
    cout << "front, back checked \n";
  else
    cout << "front, back failure\n";
  b[1] = "islam";
  if (b[1] != "islam")
    cout << "operator[] failure\n";
  else
    cout << "operator[] checked \n";
}

void TEST_ASSIGN_RESIZE() {
  cout << "Testing assign, resize, reserve functions with integers...\n";
  Vector<int> a(5, 32);
  a.assign(4, 321);
  int f = 1;
  for (size_type i = 0; i < a.size(); i++) {
    f &= a[i] == 321;
  }
  if (f && a.size() == 4)
    cout << "assign function checked \n";
  else
    cout << "assign function failure\n";
  a.resize(10, 19);
  if (a.size() != 10 || a[0] != 19)
    cout << "resize function failure\n";
  else
    cout << "resize function checked \n";
  a.reserve(1000);
  if (a.capacity() != 1000)
    cout << "reserve function failure\n";
  else
    cout << "reserve function checked \n";
  cout << "Testing assign, resize, reserve functions with strings...\n";
  Vector<string> b(5, "boost");
  b.assign(4, "islam");
  f = 1;
  for (size_type i = 0; i < b.size(); i++) {
    f &= b[i] == "islam";
  }
  if (f && b.size() == 4)
    cout << "assign function checked \n";
  else
    cout << "assign function failure\n";
  b.resize(10, "isl");
  if (b.size() != 10 || b[0] != "isl")
    cout << "resize function failure\n";
  else
    cout << "resize function checked \n";
  b.reserve(1000);
  if (b.capacity() != 1000)
    cout << "reserve function failure\n";
  else
    cout << "reserve function checked \n";
  cout << "\n\n";
  cout << "Testing Assignment operators for integers..." << endl;
  Vector<int> ax(3, 212);
  Vector<int> bx(53, 20);
  bx = ax;
  if (!(bx == ax))
    cout << "Assignment operator failure!!!\n";
  else
    cout << "Assignment operator checked\n";
  bx = { 32, 33, 34, 312 };
  if (bx[0] != 32 && bx[1] != 33 && bx.size() != 4)
    cout << "Assignment operator failure!!!\n";
  else
    cout << "Assignemtn operator checked\n";
  cout << "Testing Assignment operators for strings..." << endl;
  Vector<string> aa(3, "boost");
  Vector<string> bb(32, "islam");
  bb = aa;
  if (!(aa == bb))
    cout << "Assignement operator failure!!!\n";
  else
    cout << "Assignment operator checked\n";
  bb = { "aasdfasdf", "library", "google" };
  if (bb[0] != "aasdfasdf" || bb[1] != "library" || bb.size() != 3)
    cout << "Assignement operator failure!!!\n";
  else
    cout << "Assignment operator checked\n";
}

void TEST_PUSH_POP() {
  cout << "push_back, pop_back, clear functions with integers\n";
  Vector<int> ad;
  cout << "pushing_back million integer..." << endl;
  for (int i = 0; i < 1000000; i++) {
    ad.push_back((int)ad.max_size());
  }
  if (ad[0] != (int)ad.max_size())
    cout << "push_back failure\n";
  else
    cout << "push_back checked \n";
  cout << "poping_back thousand integers..." << endl;
  for (int i = 0; i < 1000; i++) {
    ad.pop_back();
  }
  if (ad.size() != 1000000 - 1000)
    cout << "pop_back failure\n";
  else
    cout << "pop_back checked \n";
  ad.clear();
  if (!ad.empty())
    cout << "clear function failure\n";
  else
    cout << "clear function checked \n";
  
  cout << "Testing push_back, pop_back, clear with strings...\n";
  Vector<string> b;
  cout << "pusing million string..." << endl;
  string x = "abcdefghijklmmnobqrstalkcvxznadsfoiqweriyajhdsfadf";
  for (int i = 0; i < 1000000; i++) {
    b.push_back(x + (char)('a' + i));
  }
  if (b.size() != 1000000 && b[0] != x + 'a')
    cout << "push_back failure\n";
  else
    cout << "push_back checked \n";
  cout << "poping_back thousand string..." << endl;
  for (int i = 0; i < 1000; i++) {
    b.pop_back();
  }
  if (b.size() != 1000000 - 1000)
    cout << "pop_back failure" << endl;
  else
    cout << "pop_back checked " << endl;
  b.clear();
  if (!b.empty())
    cout << "clear function failure" << endl;
  else
    cout << "clear function checked " << endl;
  cout << "\n\nTesting emplace_back for integers\n";
  Vector <pair<int, int>> em;
  for (int i = 0; i < 1000000; i++) {
    em.emplace_back(i, i + 1);
  }
  if (em.size() != 1000000 || em[0] != make_pair(0, 1))
    cout << "emplace_back function failure" << endl;
  else
    cout << "emplace_back function checked" << endl;
  cout << "Testing emplace_back with strings...\n";
  Vector<pair<string, string>> es;
  for (int i = 0; i < 1000000; i++) {
    es.emplace_back(x, x);
  }
  if (es.size() != 1000000 || es[0] != make_pair(x, x))
    cout << "emplace_back function failure" << endl;
  else
    cout << "emplace_back function checked" << endl;
}

int main(void) {
  //Testing contructors
  TEST_CONSTRUCTOR_1();
  cout << endl << endl;
  TEST_CONSTRUCTOR_2();
  cout << endl << endl;
  TEST_CONSTRUCTOR_3();
  cout << endl << endl;
  TEST_CONSTRUCTOR_4();
  cout << endl << endl;
  TEST_CONSTRUCTOR_5();
  cout << endl << endl;
  //checked
  
  //Testing member methods
  TEST_BEGIN_END();
  cout << endl << endl;
  TEST_ASSIGN_RESIZE();
  cout << endl << endl;
  TEST_PUSH_POP();
  cout << endl << endl;
  //checked
  return 0;
}
