#include "slist.hpp"

#include <iostream>

using namespace std;

typedef long long ll;
typedef unsigned long long llu;

signed main() {
  rmRecorder<ll> re;
  slist<ll> te;
  te.regRec(&re);
  node<ll> *p1 = te.begin(), *p2 = te.begin(), *p3 = te.begin(),
           *p4 = te.begin();
  for (ll i = 0; i < 10; i++) {
    te.add(i);
    if (i == 0) p1 = te.end()->prev();
    if (i == 7) p2 = te.end()->prev();
    if (i == 3) p3 = te.end()->prev();
    if (i == 9) p4 = te.end()->prev();
  }
  for (node<ll> *it = te.begin(); it != te.end(); it = it->next())
    cout << *(it->X) << " ";
  cout << endl;

  re.nextLayer();
  te.rm(p1);
  te.rm(p2);

  for (node<ll> *it = te.begin(); it != te.end(); it = it->next())
    cout << *(it->X) << " ";
  cout << endl;

  re.nextLayer();
  te.rm(p3);
  te.rm(p4);

  for (node<ll> *it = te.begin(); it != te.end(); it = it->next())
    cout << *(it->X) << " ";
  cout << endl;

  re.backtrack();

  for (node<ll> *it = te.begin(); it != te.end(); it = it->next())
    cout << *(it->X) << " ";
  cout << endl;

  re.backtrack();

  for (node<ll> *it = te.begin(); it != te.end(); it = it->next())
    cout << *(it->X) << " ";
  cout << endl;
}