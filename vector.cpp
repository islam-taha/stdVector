#include <iostream>
#include <cstdio>
#include <type_traits>
#include <initializer_list>
#include <cstring>
#include <string>
#include <algorithm>
#include <cassert>
#include <vector>
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
		__size(0), __capacity(0), alloc(__a) { buffer = alloc.allocate(0); }
  
  Vector(size_type __n, const _Allocator& __a = _Allocator()) :
		__size(__n), __capacity(__n), alloc(__a) {
      buffer = alloc.allocate(__n);
      uninitialized_fill_n(buffer, __n, _Tp());
    }
  
  Vector(size_type __n, const _Tp& value, const _Allocator& __a = _Allocator()) :
		__size(__n), __capacity(__n), alloc(__a) {
      buffer = alloc.allocate(__n);
      uninitialized_fill_n(buffer, __n, value);
    }
  
  template<class InputIterator, class = typename enable_if<is_same<InputIterator, const _Tp*>::value>::type>
  Vector(InputIterator __first, InputIterator __last, const _Allocator& __a = _Allocator()) :
  __size(0), __capacity(0), alloc(__a)
  {
    buffer = alloc.allocate(0);
    for (; __first != __last; ++__first) {
      push_back(*__first);
    }
  }
  
  Vector(initializer_list<_Tp> __li, const _Allocator& __a = _Allocator()) :
  __size(0), __capacity(0), alloc(__a)
  {
    buffer = alloc.allocate(__li.size());
    for (auto _list : __li)
      buffer[__size++] = _list;
    __capacity = __size;
  }
  
  ~Vector();
  
  Vector<_Tp, _Allocator> operator=(const Vector<_Tp, _Allocator>& __o) {
    __size = __o.size();
    __capacity = __o.capacity();
    return Vector(__o.begin(), __o.end());
  }
  
  Vector<_Tp, _Allocator> operator=(const initializer_list<_Tp> __li) {
    return Vector<_Tp, _Allocator>(__li);
  }
  
  void reserve(const size_type& size);
  void pop_back();
  
  void assign(size_type __n, const _Tp& value);
  void resize(size_type __n, const _Tp& value);
  
  void clear() {
    if (!__capacity)
      return;
    while (__size > 0)
      pop_back();
    alloc.deallocate(this->buffer, __capacity);
    __size = __capacity = 0;
  }
  
  const _Tp* begin() const {
    return buffer;
  }
  
  const _Tp* end() const {
    return buffer + __size;
  }
  
  const _Tp& front() const {
    assert(!empty());
    return buffer[0];
  }
  
  const _Tp& back() const {
    assert(!empty());
    return buffer[__size - 1];
  }
  
  const size_type size() const {
    return this->__size;
  }
  
  const size_type capacity() const {
    return __capacity;
  }
  
  const bool empty() const {
    return (__size == 0);
  }
  
  const _Tp& operator[](const size_type& index) const {
    assert(index >= 0 && index < __capacity);
    return buffer[index];
  }
  
  _Tp& operator[](const size_type& index) {
    assert(index >= 0 && index < __capacity);
    return buffer[index];
  }
  
  void push_back(const _Tp& value) {
    if (__size >= __capacity) {
      reserve(__capacity > 0 ? __capacity * 2 : 1);
    }
    alloc.construct(this->buffer + __size, value);
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

int main(void) {
  
  return 0;
}
