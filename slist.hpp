#include <set>
#include <stack>
#include <string>

using namespace std;

template <typename T>
struct node;
template <typename T>
struct slist;
template <typename T>
struct rmRecorder;

template <typename T>
struct node {
  stack<node<T> *> prvPS, nxtPS;
  slist<T> *L;
  T *X = nullptr;

  node(slist<T> *l, T *x = nullptr, node<T> *_prv = nullptr,
       node<T> *_nxt = nullptr) {
    this->L = l;
    if (x != nullptr) this->X = new T(*x);
    this->init_upd(_prv, _nxt);
  }

  void __upd(node<T> *_prv, node<T> *_nxt) {
    if (_prv != nullptr) this->prvPS.push(_prv);
    if (_nxt != nullptr) this->nxtPS.push(_nxt);
  }

  void init_upd(node<T> *_prv, node<T> *_nxt) {
    if (_prv != nullptr)
      while (!this->prvPS.empty()) this->prvPS.pop();
    if (_nxt != nullptr)
      while (!this->nxtPS.empty()) this->nxtPS.pop();
    this->__upd(_prv, _nxt);
  }

  void upd(node<T> *_prv, node<T> *_nxt) {
    if (_prv != nullptr) {
      auto it = this->L->Recorder->ch.top().find(&(this->prvPS));
      if (it == this->L->Recorder->ch.top().end())
        this->L->Recorder->ch.top().insert(&(this->prvPS));
      else
        this->prvPS.pop();
      this->prvPS.push(_prv);
    }
    if (_nxt != nullptr) {
      auto it = this->L->Recorder->ch.top().find(&(this->nxtPS));
      if (it == this->L->Recorder->ch.top().end())
        this->L->Recorder->ch.top().insert(&(this->nxtPS));
      else
        this->nxtPS.pop();
      this->nxtPS.push(_nxt);
    }
  }

  bool isHead() { return this->L->begin() == this; }
  bool isTail() { return this->L->end() == this; }
  node<T> *prev() { return this->prvPS.top(); }
  node<T> *next() { return this->nxtPS.top(); }
};

template <typename T>
struct slist {
  stack<node<T> *> beginPS, endPS;
  rmRecorder<T> *Recorder = nullptr;

  slist() {
    auto primNode = new node<T>(this);
    this->beginPS.push(primNode);
    this->endPS.push(primNode);
  }

  node<T> *begin() { return this->beginPS.top(); }
  node<T> *end() { return this->endPS.top(); }

  void regRec(rmRecorder<T> *rec) { this->Recorder = rec; }

  bool empty() { return this->begin() == this->end(); }

  bool single() {
    if (this->empty()) return false;
    return this->begin()->next() == this->end();
  }

  void add(T x) {
    if (this->empty()) {
      while (!this->beginPS.empty()) this->beginPS.pop();
      this->beginPS.push(new node<T>(this, &x, nullptr, this->end()));
      this->end()->init_upd(this->begin(), nullptr);
    } else {
      auto NewNode = new node<T>(this, &x, this->end()->prev(), this->end());
      this->end()->prev()->init_upd(nullptr, NewNode);
      this->end()->init_upd(NewNode, nullptr);
    }
  }

  void rm(node<T> *nd) {
    if (nd->L != this) return;
    if (nd == this->end()) return;
    if (nd == this->begin()) {
      auto it = this->Recorder->ch.top().find(&this->beginPS);
      if (it == this->Recorder->ch.top().end())
        this->Recorder->ch.top().insert(&this->beginPS);
      else
        this->beginPS.pop();
      this->beginPS.push(nd->next());
    } else {
      nd->prev()->upd(nullptr, nd->next());
      nd->next()->upd(nd->prev(), nullptr);
    }
  }

  T *front() { return this->begin()->X; }

  T *back() { return this->end()->prev()->X; }
};

template <typename T>
struct rmRecorder {
  stack<set<stack<node<T> *> *>> ch;
  int layer = 0;

  rmRecorder() { this->nextLayer(); }

  void nextLayer() {
    this->ch.push(set<stack<node<T> *> *>());
    this->layer++;
  }

  void backtrack() {
    for (auto it = this->ch.top().begin(); it != this->ch.top().end(); it++)
      (*it)->pop();
    this->layer--;
    ch.pop();
  }
};