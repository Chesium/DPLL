#include <cstdio>
#include <deque>
#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <list>

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

struct CNF{

  bool containEmptyClause=false;
  Container<Container<clause::iterator>>occur;
  list<clause>cl;

  /***/ clause& front()                   {return this->cl.front();      }
  /***/ clause& back()                    {return this->cl.back();       }
  /***/ list<clause>::iterator begin()    {return this->cl.begin();      }
  /***/ list<clause>::iterator end()      {return this->cl.end();        }
  /***/ bool empty()                      {return this->cl.empty();      }
  /***/ void erase(list<clause>::iterator it) {   this->cl.erase(it);    }
  /***/ void push_back(clause& x)             {   this->cl.push_back(x); }
  /***/ void push_back(clause x)              {   this->cl.push_back(x); }

  void read();
  string str();

  void removeLiteral(list<clause>::iterator it1,clause::iterator& it2);
  void removeClause(list<clause>::iterator& it);

  ll AssignLiteralIn(list<clause>::iterator& cl,literal& unit);
  bool UnitPropagate();
  bool PureLiteralAssign();
  
  bool DPLL();
};

struct literal{
  // 命题变号索引（编号）
  ll i;
  // 是否为否定文字s
  bool neg;
  // 所在子句的迭代器
  list<clause>::iterator cl;
  // 在复现列表 occur 中的迭代器
  Container<clause::iterator>::iterator oc;
  CNF* cnf;
  // 从命题变号名创建
  literal(CNF* _cnf,string s,bool _neg=true);
  // 转换为字符串
  string str(){
    if(this->neg)
      return NegSymbol+atoms[this->i];
    else
      return atoms[this->i];
  }
  // 在复现列表 occur 中删除自己的出现
  void RemoveOccurrence();
};

literal::literal(CNF* _cnf,string s,bool _neg){
  this->cnf=_cnf;
  this->neg=_neg;
  StringDict::iterator it;
  it=dict.find(s);
  if(it==dict.end()){
    dict.insert(make_pair(s,atomN));
    atoms.push_back(s);
    this->cnf->occur.push_back(Container<clause::iterator>());
    this->i=atomN;
    atomN++;
  }else{
    i=it->second;
  }
}

void literal::RemoveOccurrence(){
  this->cnf->occur[this->i].erase(this->oc);
}

string ClauseToStr(clause& cl){
  string res="";
  for(auto it=cl.begin();it!=cl.end();it++){
    if(it!=cl.begin())res=res+" "+OrSymbol+" ";
    res+=it->str();
  }
  return res;
}


void CNF::removeLiteral(list<clause>::iterator cl,clause::iterator& lit){
  lit->RemoveOccurrence();
  auto tmp=lit;
  lit++;
  cl->erase(tmp);
  lit--;
}

void CNF::removeClause(list<clause>::iterator& cl){
  for(auto lit=cl->begin();lit!=cl->end();lit++)
    lit->RemoveOccurrence();
  auto tmp=cl;
  cl++;
  this->erase(tmp);
  cl--;
}

void CNF::read(){
  ll n;
  cin>>n;
  for(ll i=0;i<n;i++){
    this->push_back(clause());
    ll tn;
    cin>>tn;
    for(ll j=0;j<tn;j++){
      string s;
      cin>>s;
      if(s[0]=='^') this->back().push_back(literal(this,s.substr(1),true));
      else this->back().push_back(literal(this,s,false));
      auto tmp1=this->back().end();tmp1--;
      this->occur[this->back().back().i].push_back(tmp1);
      auto tmp2=this->end();tmp2--;
      this->back().back().cl=tmp2;
      this->back().back().oc=this->occur[this->back().back().i].end()-1;
    }
  }
}

string CNF::str(){
  string res="{\n";
  for(auto it=this->begin();it!=this->end();it++){
    res+=" ";
    if(it!=this->begin())res+=AndSymbol;
    else res+=WideSpace;
    auto tmp=it->begin();tmp++;
    if(tmp==it->end())res+=" "+ClauseToStr(*it)+"\n";
    else res+=" ( "+ClauseToStr(*it)+" )\n";
  }
  res+="}\n";
  return res;
}

/**
 * 0 => 没有修改子句
 * 1 => 修改了该子句
 * 2 => 包含空子句，CNF不可满足
 */
ll CNF::AssignLiteralIn(list<clause>::iterator& cl,literal& unit){
  bool changed=false;
  for(auto it=cl->begin();it!=cl->end();it++)
    if(it->i==unit.i){
      if(it->neg==unit.neg){
        /* 移除子句 */
        /***/cout<<"* del Phi \""<<ClauseToStr(*cl)<<"\""<<endl;
        this->removeClause(cl);
        return 1;
      }else{
        /**
         * cl 包含 unit 的否定出现（cl[]）
         * 这次出现肯定取不到 true，即 cl 的真值取决于 cl\{unit}，删除该 unit
         */
        /***/cout<<"* del ¬("<<unit.str()<<") in \""<<ClauseToStr(*cl)<<"\""<<endl;
        this->removeLiteral(cl,it);
        if(cl->empty()){
          this->containEmptyClause=true;
          return 2;
        }
        changed=true;
      }
    }
  return changed?1:0;
}

bool CNF::UnitPropagate(){
  bool ok=false;
  for(auto it1=this->begin();it1!=this->end();it1++)
    if(it1->size()==1){
      /* A=Phi[i] 是一个单位子句（unit clause）*/
      literal A=(*it1).front();
      /***/cout<<"* find unit clause \""<<A.str()<<"\""<<endl;
      for(auto it2=this->begin();it2!=this->end();it2++){
        /***/cout<<"* process clause \""<<ClauseToStr(*it2)<<"\""<<endl;
        if(&(*it1)==&(*it2))continue;
        ll res=this->AssignLiteralIn(it2,A);
        if(res==2)return false;
        if(res)ok=true;
      }
      if(ok)return true;
      /***/cout<<"* nothing changed"<<endl;
    }
  return false;
}

bool CNF::PureLiteralAssign(){
  /* 遍历所有命题变号 */
  for(ll i=0;i<atomN;i++)
    if(this->occur[i].size()==1){
      /**
       * 这个命题变号在 Φ 中只出现了一次
       * 我们可以给它赋值，使其所在子句为真，而不会对其他子句产生影响
       * 也就是说，我们可以删除这个命题变号所在的子句
       * 在这之前，我们在复现列表中删除这个子句中其他命题变号的”出现“
       */
      this->removeClause(this->occur[i][0]->cl);
      return true;
    }
  return false;
}

// bool CNF::DPLL(){
//   //   while there is a unit clause {l} in Φ do
//   //       Φ ← unit-propagate(l, Φ);
//   while(this->UnitPropagate()){}
//   /***/cout<<this->str();
//   //   if Φ contains an empty clause then
//   //       return false;
//   if(this->containEmptyClause)return false;
//   //   while there is a literal l that occurs pure in Φ do
//   //       Φ ← pure-literal-assign(l, Φ);
//   while(this->PureLiteralAssign()){}
//   //   if Φ is empty then
//   //       return true;
//   if(this->empty()){
//     return true;
//   }
//   //   l ← choose-literal(Φ);
//   //   return DPLL(Φ ∧ {l}) or DPLL(Φ ∧ {not(l)});
//   /* Choose Literal: this->front().front() */
//   literal& L=this->front().front();

//   for(auto it=this->begin();it!=this->end();it++){
    
//   }

//   return DPLL(Phi)||DPLL(Phi);
//   // return true;
// }

signed main(){
  CNF test;
  test.read();
  cout<<test.str();

  for(ll i=0;i<atomN;i++){
    cout<<"["<<i<<"] "<<atoms[i]<<endl;
    llu tn=test.occur[i].size();
    for(llu j=0;j<tn;j++){
      cout<<test.occur[i][j]->str()<<" ";
    }
    cout<<endl;
  }
  // test.UnitPropagate();
  while(test.UnitPropagate()){}
  cout<<test.str();
  while(test.PureLiteralAssign()){}
  cout<<test.str();
}

/*
测试样例：

5
2 ^a b
2 b c
2 ^c d
4 ^a a c d
1 a

*/