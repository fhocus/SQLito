#ifndef TRIPLET_H
#define TRIPLET_H

#include <iostream>

template <typename T, typename O, typename V>
class Triplet
{
public:
  Triplet(T first, O second, V third)
      : first_(first), second_(second), third_(third) {}

  ~Triplet() {}

  T getFirst() const { return first_; }
  O getSecond() const { return second_; }
  V getThird() const { return third_; }

private:
  T first_;
  O second_;
  V third_;
};

#endif
