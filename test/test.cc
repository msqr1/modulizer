#include "test.hpp"
int pass(int a) {
  return a;
}
Shape::Square::Square(int sidelength) {
  side1 = side2 = side3 = side4 = sidelength;
}
int Shape::Square::getArea() {
  return side1 * side1;
}
template <class T> T max(T a, T b) {
  return a > b ? a : b;
}
template <class T> Any<T>::Any(T member) {
  member = member;
}
template <class T> T Any<T>::getMember() {
  return member;
}
