#include <cstdio>
#include <deque>
#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <list>
#include <vector>
#include <stack>

using namespace std;

#define Container deque
#define NegSymbol "¬"
#define AndSymbol "∧"
#define OrSymbol "∨"
#define WideSpace "　"

typedef long long ll;
typedef unsigned long long llu;
typedef map<string,ll> StringDict;

struct literal;
struct CNF;

typedef list<literal> clause; // clause of CNF (DNF) 合取范式的子句（文字或文字否定的析取式）

StringDict dict;
Container<string> atoms;
ll atomN=0;

struct ListNode;

struct ClauseNode;
struct CNFnode;
struct OccurNode;
struct AtomNode;

struct ClauseNode{
  bool IsHead,IsTail;
  stack<ClauseNode*>prv,nxt;
  CNFnode *clHead;

  ClauseNode(){}
  ClauseNode(CNFnode *clH,string s,bool _n);

  ll atomI;
  bool neg;
};

struct OccurNode{
  bool IsHead,IsTail;
  stack<OccurNode*>prv,nxt;
  AtomNode *atom;

  ClauseNode *lit;
};

struct CNFnode{
  bool IsHead,IsTail;
  stack<CNFnode*>prv,nxt;
  CNFnode *head;
  ClauseNode *cl;

  void read();
};

struct AtomNode{
  bool IsHead,IsTail;
  stack<AtomNode*>prv,nxt;
  OccurNode *oc;

  ll atomI;
  string atomS;
}AvAtom,*AvAtomEnd;

ClauseNode::ClauseNode(CNFnode *clH,string s,bool _neg){
  this->clHead=clH;
  this->neg=_neg;
  auto it=dict.find(s);

  // this->cnf=_cnf;
  // this->neg=_neg;
  // StringDict::iterator it;
  // it=dict.find(s);
  if(it==dict.end()){
    dict.insert(make_pair(s,atomN));
    atoms.push_back(s);
    AvAtomEnd->IsTail=false;
    AvAtomEnd->nxt.push(new AtomNode());
    
    // this->cnf->occur.push_back(Container<clause::iterator>());
    this->atomI=atomN;
    atomN++;
  }else{
    this->atomI=it->second;
  }
}

void CNFnode::read(){
  ll n;
  cin>>n;
  this->IsHead=true;
  for(ll i=0;i<n;i++){
    this->nxt.push(new CNFnode());
    this->nxt.top()->prv.push(this);
    // this->push_back(clause());
    ll tn;
    cin>>tn;
    for(ll j=0;j<tn;j++){
      string s;
      cin>>s;
      // if(s[0]=='^') 
    //   if(s[0]=='^') this->back().push_back(literal(this,s.substr(1),true));
    //   else this->back().push_back(literal(this,s,false));
    //   auto tmp1=this->back().end();tmp1--;
    //   this->occur[this->back().back().i].push_back(tmp1);
    //   auto tmp2=this->end();tmp2--;
    //   this->back().back().cl=tmp2;
    //   this->back().back().oc=this->occur[this->back().back().i].end()-1;
    }
  }
}

// struct CNF{

//   bool containEmptyClause=false;
//   // Container<Container<clause::iterator>>occur;
//   list<clause>cl;

//   /***/ clause& front()                   {return this->cl.front();      }
//   /***/ clause& back()                    {return this->cl.back();       }
//   /***/ list<clause>::iterator begin()    {return this->cl.begin();      }
//   /***/ list<clause>::iterator end()      {return this->cl.end();        }
//   /***/ bool empty()                      {return this->cl.empty();      }
//   /***/ void erase(list<clause>::iterator it) {   this->cl.erase(it);    }
//   /***/ void push_back(clause& x)             {   this->cl.push_back(x); }
//   /***/ void push_back(clause x)              {   this->cl.push_back(x); }

//   void read();
//   string str();

//   void removeLiteral(list<clause>::iterator it1,clause::iterator& it2);
//   void removeClause(list<clause>::iterator& it);

//   ll AssignLiteralIn(list<clause>::iterator& cl,literal& unit);
//   bool UnitPropagate();
//   bool PureLiteralAssign();
  
//   bool DPLL();
// };

// struct literal{
//   // 命题变号索引（编号）
//   ll i;
//   // 是否为否定文字s
//   bool neg;
//   // 所在子句的迭代器
//   // list<clause>::iterator cl;
//   // 在复现列表 occur 中的迭代器
//   // Container<clause::iterator>::iterator oc;
//   // CNF* cnf;
//   // 从命题变号名创建
//   literal(CNF* _cnf,string s,bool _neg=true);
//   // 转换为字符串
//   string str(){
//     if(this->neg)
//       return NegSymbol+atoms[this->i];
//     else
//       return atoms[this->i];
//   }
//   // 在复现列表 occur 中删除自己的出现
//   void RemoveOccurrence();
// };