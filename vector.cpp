# stdVector
simple implementation to std::vector not with all functions
#include <algorithm>
#include <memory>
#include <iostream>
#include <cstddef>
#include <initializer_list>
#include <cstring>
#include <type_traits>
#include <ctime>
using namespace std;

#define eprintf(...)  fprintf(stderr, __VA_ARGS__)
#define TIMESTAMP(x)  eprintf("[" #x "] Time = %.5lfs\n",clock()*1.0/CLOCKS_PER_SEC)

#ifdef DEBUG
struct __timestamper {
  ~__timestamper() {
    TIMESTAMP(END);
  }
} __Timestamper;
#else
struct __timestamper {};
#endif

typedef unsigned int size_type;

template <typename T, typename Allocator = allocator<T> >
class vector {
private:
  Allocator alloc;

  T* buffer;

  size_type __size;
  size_type __capacity;
  
public:
  
  explicit vector(const Allocator& = Allocator()) noexcept :
    __size(0), __capacity(0)
  {
  }
  
  explicit vector(size_type __n, const Allocator& = Allocator()) :
    __size(__n), __capacity(__n)
  {
    buffer = alloc.allocate(__n);
    uninitialized_fill_n(buffer, __n, T());
  }
  
  explicit vector(size_type __n, const T& value, const Allocator& = Allocator()) :
    __size(__n), __capacity(__n)
  {
    buffer = alloc.allocate(__n);
    uninitialized_fill_n(buffer, __n, value);
  }
  
  void reserve(const size_type& size);
  void push_back(const T& value);
  void pop_back();
  void clear();
  
  const T* begin() const {
    return buffer;
  }
  
  const T* end() const {
    return buffer + __size;
  }
  
  const T& front() const {
    try {
      return buffer[0];
    } catch(...) {
      cerr << "vector is empty!" << endl;
      exit(0);
    }
  }
  
  const T& back() const {
    try {
      return buffer[__size - 1];
    } catch(...) {
      cerr << "vector is empty!" << endl;
      exit(0);
    }
  }
  
  const size_type size() const noexcept {
    return this->__size;
  }
  
  const bool empty() const noexcept {
    return (__size == 0);
  }
  
  const T& operator[](const size_type& index) const {
    try {
      return buffer[index];
    } catch(...) {
      cerr << "index out of boundary!" << endl;
      exit(0);
    }
  }
  
  T& operator[](const size_type& index) {
    try {
      return buffer[index];
    } catch(...) {
      cerr << "index out of boundary!" << endl;
      exit(0);
    }
  }
  
};

template <class T, typename Allocator>
void vector<T, Allocator>::reserve(const size_type& size) {
  if (size <= __capacity)
    return;
  
  T* newBuffer = alloc.allocate(size);
  
  uninitialized_copy(buffer, buffer + __size, newBuffer);
  
  for (size_type i = 0; i < __size; i++) {
    alloc.destroy(&buffer[i]);
  }
  
  alloc.deallocate(buffer, __capacity);
  
  __capacity = size;
  buffer = newBuffer;
  
}

template <class T, typename Allocator>
void vector<T, Allocator>::push_back(const T& value) {
  if (__size >= __capacity) {
    reserve(__capacity * 2 + 2);
  }
  try {
    buffer[__size++] = value;
  } catch(...) {
    cerr << "A7EH" << endl;
    exit(0);
  }
}

template <class T, typename Allocator>
void vector<T, Allocator>::pop_back() {
  if (__size == 0)
    return;
  alloc.destroy(&buffer[__size - 1]);
  --__size;
}

template <class T, typename Allocator>
void vector<T, Allocator>::clear() {
  for (int i = 0; i < __size; i++) {
    alloc.destroy(&buffer[i]);
  }
  alloc.deallocate(buffer, __capacity);
  __size = __capacity = 0;
}

template <class T, typename Allocator>
inline bool
operator==(const vector<T, Allocator>& x, const vector<T, Allocator>& y) {
  return (x.size() == y.size() &&
          equal(x.begin(), x.end(), y.begin()));
}

template <class T, typename Allocator>
inline bool
operator<(const vector<T, Allocator>& x, const vector<T, Allocator>& y) {
  return lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
}

template <class T, typename Allocator>
inline bool
operator!=(const vector<T, Allocator>& x, const vector<T, Allocator>& y) {
  return !(x == y);
}

template <class T, typename Allocator>
inline bool
operator>(const vector<T, Allocator>& x, const vector<T, Allocator>& y) {
  return y < x;
}

template <class T, typename Allocator>
inline bool
operator<=(const vector<T, Allocator>& x, const vector<T, Allocator>& y) {
  return !(x > y);
}

template <class T, typename Allocator>
inline bool
operator>=(const vector<T, Allocator>& x, const vector<T, Allocator>& y) {
  return !(x < y);
}


int main(void) {
  vector <int> ad(4, 198);
  vector <int> b(4, 199);
  cout << boolalpha << (ad == b) << endl;
  ad.clear();
  b.clear();
  cout << boolalpha << ad.empty() << ' ' << b.empty() << endl;
  return 0;
}
