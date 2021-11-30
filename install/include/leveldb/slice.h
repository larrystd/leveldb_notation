// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// Slice is a simple structure containing a pointer into some external
// storage and a size.  The user of a Slice must ensure that the slice
// is not used after the corresponding external storage has been
// deallocated.
//
// Multiple threads can invoke const methods on a Slice without
// external synchronization, but if any of the threads may call a
// non-const method, all threads accessing the same Slice must use
// external synchronization. Slice的const方法可以直接进行多线程访问, 但非const方法必须外界施加同步

#ifndef STORAGE_LEVELDB_INCLUDE_SLICE_H_
#define STORAGE_LEVELDB_INCLUDE_SLICE_H_

#include <assert.h>
#include <stddef.h> // 定义了一些宏变量, 例如size_t, NULL
#include <string.h>
#include <string>
#include "leveldb/export.h"

namespace leveldb {

/// Slice, 常常作为key使用
class LEVELDB_EXPORT Slice {
 public:
  // Create an empty slice. 默认构造
  Slice() : data_(""), size_(0) { }
  
  /// 三种Slice初始化方法, const char*, std::string, char*, 实际内部都是字符指针
  // Create a slice that refers to d[0,n-1].
  Slice(const char* d, size_t n) : data_(d), size_(n) { }

  // Create a slice that refers to the contents of "s"
  Slice(const std::string& s) : data_(s.data()), size_(s.size()) { }

  // Create a slice that refers to s[0,strlen(s)-1]
  Slice(const char* s) : data_(s), size_(strlen(s)) { }

  // Intentionally copyable. Slice可以拷贝和赋值
  Slice(const Slice&) = default;
  Slice& operator=(const Slice&) = default;

  // Return a pointer to the beginning of the referenced data 返回一个内部维护的const char* data
  const char* data() const { return data_; }

  // Return the length (in bytes) of the referenced data 返回字符串大小
  size_t size() const { return size_; }

  // Return true iff the length of the referenced data is zero 是否为空
  bool empty() const { return size_ == 0; }

  // Return the ith byte in the referenced data.
  // REQUIRES: n < size()
  char operator[](size_t n) const { // 使用[]访问元素
    assert(n < size());
    return data_[n];
  }

  // Change this slice to refer to an empty array 清空字符串
  void clear() { data_ = ""; size_ = 0; }

  // Drop the first "n" bytes from this slice.移除前n个char字节
  void remove_prefix(size_t n) {
    assert(n <= size());
    data_ += n;
    size_ -= n;
  }

  // Return a string that contains the copy of the referenced data. 转换成string类型
  std::string ToString() const { return std::string(data_, size_); }

  // Three-way comparison.  Returns value:
  //   <  0 iff "*this" <  "b",
  //   == 0 iff "*this" == "b",
  //   >  0 iff "*this" >  "b"
  int compare(const Slice& b) const;  // 字符串比较函数

  // Return true iff "x" is a prefix of "*this" Slice是否有x前缀
  bool starts_with(const Slice& x) const {
    return ((size_ >= x.size_) &&
            (memcmp(data_, x.data_, x.size_) == 0));
  }

 private:
  const char* data_;
  size_t size_;
};

// 比较两个Slice是否相等
inline bool operator==(const Slice& x, const Slice& y) {
  return ((x.size() == y.size()) &&
          (memcmp(x.data(), y.data(), x.size()) == 0));
}

inline bool operator!=(const Slice& x, const Slice& y) {
  return !(x == y);
}

// 实现比较函数, return 1表示this > b
inline int Slice::compare(const Slice& b) const {
  const size_t min_len = (size_ < b.size_) ? size_ : b.size_;
  int r = memcmp(data_, b.data_, min_len);  // 比较相同长度下的slice
  if (r == 0) {   // 若相等, 则比较长度, 长的大
    if (size_ < b.size_) r = -1;
    else if (size_ > b.size_) r = +1;
  }
  return r;
}

}  // namespace leveldb


#endif  // STORAGE_LEVELDB_INCLUDE_SLICE_H_
