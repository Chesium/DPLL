#include "dpll.hpp"

signed main() {
  CNF test;
  test.read();
  cout << test.occurStr() << endl;
  cout << test.DPLL() << endl;
  cout << test.schemeStr();
}

/*

5
2 a b
2 ^a c
4 b ^t a c
2 ^c d
1 a

8
3 ^a b c
3 a c d
3 a c ^d
3 a ^c d
3 a ^c ^d
3 ^b ^c d
3 ^a b ^c
3 ^a ^b c

*/