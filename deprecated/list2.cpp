#include <cstdio>
// #include <deque>
#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <stack>

using namespace std;

// #define Container deque
#define NegSymbol "¬"
#define AndSymbol "∧"
#define OrSymbol "∨"
#define WideSpace "　"

typedef long long ll;
typedef unsigned long long llu;
typedef map<string,ll> StringDict;

struct literal;
struct CNF;

// typedef list<literal> clause; // clause of CNF (DNF) 合取范式的子句（文字或文字否定的析取式）

// StringDict dict;
// Container<string> atoms;
// ll atomN=0;

template<typename T>struct node;
template<typename T>struct slist;

template<typename T>
struct node{
  // bool rm=false;
  stack<node<T>*>prvPS,nxtPS;
  slist<T>* L;
  T* X=nullptr;

  node(slist<T>* l,T* x=nullptr,node<T>* _prv=nullptr,node<T>* _nxt=nullptr){
    this->L=l;
    if(x!=nullptr)this->X=new T(*x);
    this->init_upd(_prv,_nxt);
  }

  void __upd(node<T>* _prv,node<T>* _nxt){
    if(_prv!=nullptr)this->prvPS.push(_prv);
    if(_nxt!=nullptr)this->nxtPS.push(_nxt);
  }

  void init_upd(node<T>* _prv,node<T>* _nxt){
    if(_prv!=nullptr)while(!this->prvPS.empty())this->prvPS.pop();
    if(_nxt!=nullptr)while(!this->nxtPS.empty())this->nxtPS.pop();
    this->__upd(_prv,_nxt);
  }

  void upd(node<T>* _prv,node<T>* _nxt){
    if(_prv!=nullptr)this->L->ch.top().push_back(&(this->prvPS));
    if(_nxt!=nullptr)this->L->ch.top().push_back(&(this->nxtPS));
    this->__upd(_prv,_nxt);
  }

  bool isHead(){return this->L->begin==this;}
  bool isTail(){return this->L->end==this;}
  node<T>* prev(){return this->prvPS.top();}
  node<T>* next(){return this->nxtPS.top();}
};

template<typename T>
struct slist{
  node<T> *begin=nullptr,*end=nullptr;
  llu /*size=0,*/layer=0;

  stack<vector<stack<node<T>*>*>>ch;

  slist(){
    this->begin=new node<T>(this);
    this->end=this->begin;
    this->nextLayer();
  }

  bool empty(){
    return this->begin==this->end;
  }

  void add(T x){
    if(this->empty()){
      this->begin=new node<T>(this,&x,nullptr,this->end);
      this->end->init_upd(this->begin,nullptr);
      // cout<<"begin:"<<(llu)(this->begin)<<" "<<*((ll*)(this->begin->X))<<endl;
      // cout<<"end:"<<(llu)(this->end)<<endl;
    }else{
      // cout<<"_prv:"<<(llu)(this->end->prv())<<" "<<*((ll*)(this->end->prv()->X))<<endl;
      node<T>* NewNode=new node<T>(this,&x,this->end->prev(),this->end);
      // cout<<"_prv:"<<(llu)(this->end->prv())<<" "<<*((ll*)(this->end->prv()->X))<<endl;
      // cout<<"_end:"<<(llu)(this->end)<<endl;
      this->end->prev()->init_upd(nullptr,NewNode);
      this->end->init_upd(NewNode,nullptr);
    }
    // this->size++;
  }

  void nextLayer(){
    this->ch.push(vector<stack<node<T>*>*>());
  }

  void backtrack(){
    for(auto it=this->ch.top().begin();it!=this->ch.top().end();it++){
      (*it)->pop();
    }
  }

  void rm(node<T>* nd){
    // if(nd->rm)return;
    if(nd->L!=this)return;
    if(nd==this->end)return;
    if(nd==this->begin){
      this->begin=this->begin->next();
    }else{
      nd->prev()->upd(nullptr,nd->next());
      nd->next()->upd(nd->prev(),nullptr);
    }
    // nd->rm=true;
    // this->size--;
  }
};

signed main(){
  slist<ll>te;
  // te.add(14);
  // te.add(1000);
  // cout<<*(te.begin->X)<<endl;
  node<ll>* ptr=te.begin;
  for(ll i=0;i<10;i++){
    te.add(i);
    if(i==5)ptr=te.end->prev();
  }
  // cout<<"size:"<<te.size<<endl;
  for(node<ll>* it=te.begin;it!=te.end;it=it->next()){
    cout<<*(it->X)<<endl;
  }
  cout<<"ptr:"<<*(ptr->X)<<endl;

  te.nextLayer();
  te.rm(ptr);

  // cout<<"size:"<<te.size<<endl;
  for(node<ll>* it=te.begin;it!=te.end;it=it->next()){
    cout<<*(it->X)<<endl;
  }

  te.backtrack();

  // cout<<"size:"<<te.size<<endl;
  for(node<ll>* it=te.begin;it!=te.end;it=it->next()){
    cout<<*(it->X)<<endl;
  }
}