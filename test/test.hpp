#pragma once

int pass(int a);
template<class T> T max(T a, T b);
enum age {
  A, B, C, D
};
namespace Shape {
  class Quadrilateral {
  protected:
    int side1{};
    int side2{};
    int side3{};
    int side4{};
  };
  class Square : Quadrilateral {
  public:
    Square(int sidelength);
    int getArea();
  };
}
template<class T>
class Any {
  T member; 
public:
  Any(T member);
  T getMember();
};

