#include "test.hpp" 
#include <iostream>
int main() {
  Shape::Square s{B};
  std::cout << s.getArea();
  Any<int> any{3};
  std::cout << any.getMember();
}