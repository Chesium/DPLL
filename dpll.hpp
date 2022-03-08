#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <vector>

#include "slist.hpp"

using namespace std;

#define NegSymbol "¬"
#define AndSymbol "∧"
#define OrSymbol "∨"
#define WideSpace "　"

typedef long long ll;
typedef unsigned long long llu;

struct Literal;
struct Clause;
struct CNF;
struct Occur;
struct AvAtom;

/**
 * 结构体：文字
 * 包括命题变号及其否定（如 a 或 ¬a ）
 */
struct Literal {
  llu index;  //
  bool neg;
  CNF *cnf;
  node<Clause> *cl;
  node<Occur> *oc;
  Literal(CNF *_cnf, string s, bool _neg);
  string str();
  void RemoveOccurrence();
};

/**
 * 结构体：子句（析取式）
 * 形如 a∨b∨c∨...∨z
 * 包含一个文字链表
 */
struct Clause {
  slist<Literal> *lt;
  CNF *cnf;
  Clause(CNF *_cnf);
  string str();
};

/**
 * 结构体：文字的出现
 * 为可用变量列表中的元素
 * 指向对应的 Literal
 */
struct Occur {
  node<Literal> *lit;
  Occur(node<Literal> *_lit) { this->lit = _lit; }
};

/**
 * 结构体：可用变量（命题变号）
 * 包含该命题变号的出现（Occurence）链表
 */
struct AvAtom {
  llu index;
  slist<Occur> *oc;
  CNF *cnf;
  AvAtom(CNF *_cnf, llu i);
};

/**
 * 结构体：公式集（析取式的合取式）
 * 形如 (a∨b∨...∨z)∧(d∨e∨...∨f)∧...∧(g∨...∨i)
 * 包含一个子句链表
 */
struct CNF {
  map<string, llu> Dict;
  vector<string> Atoms;
  vector<ll> scheme;
  llu AtomN = 0;
  slist<Clause> CL;
  slist<AvAtom> AVA;
  vector<node<AvAtom> *> avAtoms;
  rmRecorder<Literal> Rec_Literal;
  rmRecorder<Clause> Rec_Clause;
  rmRecorder<Occur> Rec_Occur;
  rmRecorder<AvAtom> Rec_AvAtom;
  stack<list<ll>> Rec_assign;
  CNF() {
    this->CL.regRec(&this->Rec_Clause);
    this->AVA.regRec(&this->Rec_AvAtom);
  }
  void read();
  string str();
  string occurStr();
  string schemeStr();
  void removeLiteral(node<Clause> *cl, node<Literal> *lit);
  void removeClause(node<Clause> *cl);
  ll AssignLiteralIn(node<Clause> *cl, node<Literal> *unit);
  bool PureLiteralAssign();
  bool UnitPropagate();
  void nextLayer();
  void backtrack();
  bool containEmptyClause = false;
  bool DPLL(bool disableSimp);
};

/**
 * 子句构造函数
 * @param _cnf 所在公式，用于绑定删除记录器 Rec_Literal
 */
Clause::Clause(CNF *_cnf) {
  this->lt = new slist<Literal>();
  this->cnf = _cnf;
  this->lt->regRec(&this->cnf->Rec_Literal);
}

/**
 * 可用变量构造函数
 * @param _cnf 所在公式，用于绑定删除记录器 Rec_Occur
 * @param i 变量序号
 */
AvAtom::AvAtom(CNF *_cnf, llu i) {
  this->oc = new slist<Occur>();
  this->cnf = _cnf;
  this->oc->regRec(&this->cnf->Rec_Occur);
  this->index = i;
}

/**
 * 文字构造函数
 * @param _cnf 所在公式
 * @param s 文字标识符
 * @param _neg 是否为否定文字
 */
Literal::Literal(CNF *_cnf, string s, bool _neg) {
  this->cnf = _cnf;
  this->neg = _neg;
  auto it = this->cnf->Dict.find(s);
  if (it == this->cnf->Dict.end()) {
    this->cnf->Dict.insert(make_pair(s, this->cnf->AtomN));
    this->cnf->Atoms.push_back(s);
    this->cnf->scheme.push_back(0);
    this->cnf->AVA.add(AvAtom(this->cnf, this->cnf->AtomN));
    this->cnf->avAtoms.push_back(this->cnf->AVA.end()->prev());
    this->index = this->cnf->AtomN;
    this->cnf->AtomN++;
  } else
    this->index = it->second;
}

/**
 * 生成文字的字符串
 * 若是否定文字，则会在标识符前加上否定符号“¬”
 */
string Literal::str() {
  if (this->neg)
    return NegSymbol + this->cnf->Atoms[this->index];
  else
    return this->cnf->Atoms[this->index];
}

/**
 * 在出现列表中移除该文字的出现
 * 一般来说，用于删除该文字之前
 */
void Literal::RemoveOccurrence() {
  this->cnf->avAtoms[this->index]->X->oc->rm(this->oc);
  if (this->cnf->avAtoms[this->index]->X->oc->empty())
    this->cnf->AVA.rm(this->cnf->avAtoms[this->index]);
}

/**
 * 生成字句的字符串
 * 每个文字之间用“ ∨ ”分隔
 */
string Clause::str() {
  stringstream res;
  for (auto it = this->lt->begin(); it != this->lt->end(); it = it->next()) {
    if (it != this->lt->begin()) res << " " << OrSymbol << " ";
    res << it->X->str();
  }
  return res.str();
}

/**
 * 从cin中读入公式
 * 格式：
 * 第一行为一个整数 n，表示有 n 个子句
 * 接下来 n 行：
 * 每行开头为一个数 k_i，表示该子句有 k_i 个文字
 * 接下来 k_i 个字符串，表示该子句中的文字
 * 如果字符串的第一位为'^'则该字符串表示对应的否定文字
 * 例：
 * 4
 * 2 a b
 * 2 ^a c
 * 4 b ^t a c
 */
void CNF::read() {
  llu n;
  cin >> n;
  for (llu i = 0; i < n; i++) {
    this->CL.add(Clause(this));
    llu tn;
    cin >> tn;
    for (llu j = 0; j < tn; j++) {
      string s;
      cin >> s;
      if (s[0] == '^')
        this->CL.back()->lt->add(Literal(this, s.substr(1), true));
      else
        this->CL.back()->lt->add(Literal(this, s, false));
      node<Literal> *tmp1 = this->CL.back()->lt->end()->prev();
      this->avAtoms[this->CL.back()->lt->back()->index]->X->oc->add(
          Occur(tmp1));
      this->CL.back()->lt->back()->cl = this->CL.end()->prev();
      this->CL.back()->lt->back()->oc =
          this->avAtoms[this->CL.back()->lt->back()->index]
              ->X->oc->end()
              ->prev();
    }
  }
}

/**
 * 生成公式的字符串
 * 样式：
 * {
 * | 　 ( a ∨ b )
 * | ∧ ( ¬a ∨ c )
 * | ∧ ( b ∨ ¬t ∨ a ∨ c )
 * | ∧ ( ¬c ∨ d )
 * | ∧ a
 * }
 */
string CNF::str() {
  stringstream res;
  res << "{\n";
  for (auto it = this->CL.begin(); it != this->CL.end(); it = it->next()) {
    res << "| ";
    if (it != this->CL.begin())
      res << AndSymbol;
    else
      res << WideSpace;
    if (it->X->lt->single())
      res << " " << it->X->str() << "\n";
    else
      res << " ( " << it->X->str() << " )\n";
  }
  res << "}\n";
  return res.str();
}

/**
 * 生成公式出现列表的字符串
 * 样式：
 * [0]a
 * a ¬
 * [1]b
 * b b
 * [2]c
 * c c ¬
 * [3]t
 * ¬
 * [4]d
 * d
 */
string CNF::occurStr() {
  stringstream res;
  ll i = 0;
  for (auto ava = this->AVA.begin(); ava != this->AVA.end();
       ava = ava->next(), i++) {
    res << "[" << to_string(i) << "]" << this->Atoms[ava->X->index] << "\n";
    for (auto oc = ava->X->oc->begin(); oc != ava->X->oc->end();
         oc = oc->next())
      res << oc->X->lit->X->str() << " ";
    res << "\n";
  }
  return res.str();
}

/**
 * 生成当前公式赋值情况的字符串
 * 样式：
 * "a" -> True
 * "b" -> False
 * "c" -> _
 * "d" -> True
 * 其中下划线表示该变量取 True 或 False 均可
 */
string CNF::schemeStr() {
  stringstream res;
  for (llu i = 0; i < this->AtomN; i++) {
    res << "\"" << this->Atoms[i] << "\" -> ";
    if (this->scheme[i] == 0)
      res << "_";
    else if (this->scheme[i] == 1)
      res << "True";
    else
      res << "False";
    res << endl;
  }
  return res.str();
}

/**
 * 移除文字
 * @param cl 文字所在的子句
 * @param lit 要移除的文字
 */
void CNF::removeLiteral(node<Clause> *cl, node<Literal> *lit) {
  cout << "DEL literal \"" << lit->X->str() << "\" in \"" << cl->X->str()
       << "\"" << endl;
  lit->X->RemoveOccurrence();
  cl->X->lt->rm(lit);
}

/**
 * 移除子句
 * @param cl 要移除的子句
 */
void CNF::removeClause(node<Clause> *cl) {
  cout << "DEL Clause \"" << cl->X->str() << "\"" << endl;
  for (auto lit = cl->X->lt->begin(); lit != cl->X->lt->end();
       lit = lit->next())
    lit->X->RemoveOccurrence();
  this->CL.rm(cl);
}

/**
 * 在指定子句中处理赋值
 * 用于单位子句传播（Unit Propagation）中
 * 若子句 cl 包含 unit 的肯定形式，则删除该子句
 * 若子句 cl 包含 unit 的否定形式，则删除该否定文字
 *
 * @param cl 要处理的子句
 * @param unit 公式中一个单位子句中的文字
 */
ll CNF::AssignLiteralIn(node<Clause> *cl, node<Literal> *unit) {
  this->scheme[unit->X->index] = unit->X->neg ? 2 : 1;
  this->Rec_assign.top().push_back(unit->X->index);
  bool changed = false;
  for (auto it = cl->X->lt->begin(); it != cl->X->lt->end(); it = it->next())
    if (it->X->index == unit->X->index) {
      if (it->X->neg == unit->X->neg) {
        /* 移除子句 */
        this->removeClause(cl);
        return 1;
      } else {
        /**
         * cl 包含 unit 的否定出现（cl[]）
         * 这次出现肯定取不到 true，即 cl 的真值取决于 cl\{unit}，删除该 unit
         */
        this->removeLiteral(cl, it);
        if (cl->X->lt->empty()) {
          this->containEmptyClause = true;
          return 2;
        }
        changed = true;
      }
    }
  return changed ? 1 : 0;
}

/**
 * 对公式进行一次单位子句传播（Unit Propagation）
 * @return 是否更改了公式
 */
bool CNF::UnitPropagate() {
  bool ok = false;
  for (auto it1 = this->CL.begin(); it1 != this->CL.end(); it1 = it1->next())
    if (it1->X->lt->single()) {
      /* A=Phi[i] 是一个单位子句（unit clause）*/
      node<Literal> *A = it1->X->lt->begin();
      for (auto it2 = this->CL.begin(); it2 != this->CL.end();
           it2 = it2->next()) {
        if (it1 == it2) continue;
        ll res = this->AssignLiteralIn(it2, A);
        if (res == 2) return false;
        if (res) ok = true;
      }
      if (ok) return true;
    }
  return false;
}

/**
 * 对公式进行一次孤立文字消去（Pure Literal Assign）
 * @return 是否更改了公式
 */
bool CNF::PureLiteralAssign() {
  for (llu i = 0; i < this->AtomN; i++)
    if (this->avAtoms[i]->X->oc->single()) {
      this->scheme[this->avAtoms[i]->X->index] =
          this->avAtoms[i]->X->oc->begin()->X->lit->X->neg ? 2 : 1;
      this->Rec_assign.top().push_back(this->avAtoms[i]->X->index);
      this->removeClause(this->avAtoms[i]->X->oc->begin()->X->lit->X->cl);
      return true;
    }
  return false;
}

/**
 * 让所有修改记录器进入新的修改层
 */
void CNF::nextLayer() {
  this->Rec_Literal.nextLayer();
  this->Rec_Clause.nextLayer();
  this->Rec_Occur.nextLayer();
  this->Rec_AvAtom.nextLayer();
  this->Rec_assign.push(list<ll>());
}

/**
 * 让所有修改记录器回溯至上个修改层
 */
void CNF::backtrack() {
  this->Rec_Literal.backtrack();
  this->Rec_Clause.backtrack();
  this->Rec_Occur.backtrack();
  this->Rec_AvAtom.backtrack();
  for (auto it = this->Rec_assign.top().begin();
       it != this->Rec_assign.top().end(); it++)
    this->scheme[*it] = 0;
  this->Rec_assign.pop();
}

/**
 * 对公式进行DPLL算法（非递归实现）
 * @param disableSimp 禁用两个化简步骤
 */
bool CNF::DPLL(bool disableSimp = false) {
  stack<ll> STACK;
  AvAtom *x;
  ll layerNow = -1, Status;
  STACK.push(0);
  /* 0 => 不进行任何赋值 */
  while (!STACK.empty()) {
    /*Status 的绝对值表示*/
    Status = STACK.top();
    STACK.pop();
    /***/ cout << "=== NEW STATUS : " << Status << " ===" << endl;

    /* -------------------------------------------------------------------------- */
    /*                                    回溯部分                                    */
    /* -------------------------------------------------------------------------- */

    /* 回溯至Status层之前，再建立新的修改层 */
    while (layerNow >= abs(Status)) {
      layerNow--;
      /***/ cout << "BACKTRACK: -> " << layerNow << endl;
      this->backtrack();
    }
    layerNow = abs(Status);
    /***/ cout << "FORMULA: begin processing(layer=" << layerNow
               << "):" << endl;
    /***/ cout << this->str();
    this->nextLayer();

    /* 第一遍循环时不赋值 */
    if (Status == 0) {
      /***/ cout << "INIT: skip assignments" << endl;
      goto SIMPLIFICATION;
    }

    /* -------------------------------------------------------------------------- */
    /*                             选取变量：其实就是选第一个可用的变量                             */
    /* -------------------------------------------------------------------------- */

    x = this->AVA.begin()->X;
    /***/ cout << "ASSIGN: \"" << Atoms[x->index] << "\" -> "
               << (Status > 0 ? "True" : "False") << endl;
    this->scheme[x->index] = Status > 0 ? 1 : 2;
    this->Rec_assign.top().push_back(x->index);

    /* -------------------------------------------------------------------------- */
    /*                                    赋值部分                                    */
    /* -------------------------------------------------------------------------- */

    /* ! 需要保证每个子句中一种命题变号至多出现一次（不存在形如 a∨a、a∨¬a
     * 的子句）*/
    for (auto it = x->oc->begin(); it != x->oc->end(); it = it->next()) {
      if ((Status < 0) == it->X->lit->X->neg)
        /* 该出现与赋值类型相同，即赋值后该文字为 true，使得其所在子句为
         * true，不对公式产生约束 */
        /* 将其所在子句删除 */
        this->removeClause(it->X->lit->X->cl);
      else {
        /* 该出现与赋值类型相反，即赋值后该文字为 false，不对所在子句产生约束 */
        /* 将该出现（文字）删除 */
        this->removeLiteral(it->X->lit->X->cl, it->X->lit);
        if (it->X->lit->X->cl->X->lt->empty()) {
          this->containEmptyClause = true;
          break;
        }
      }
    }
    /***/ cout << "FORMULA: finish assignments:" << endl;
    /***/ cout << this->str();

    /* -------------------------------------------------------------------------- */
    /*                                    化简部分                                    */
    /* -------------------------------------------------------------------------- */
  SIMPLIFICATION:
    /* 进行两个化简操作 */
    /* 1. 传播单位子句（Unit Propagatating）*/
    if (!disableSimp) {
      while (this->UnitPropagate()) {
      }
      /***/ cout << "FORMULA: Unit-propagatated:" << endl;
      /***/ cout << this->str();
      /* 2. 消去孤立文字（Pure Literal Assignment）*/
      while (this->PureLiteralAssign()) {
      }
      /***/ cout << "FORMULA: Pure-literal-assigned:" << endl;
      /***/ cout << this->str();
    }

    
    /* -------------------------------------------------------------------------- */
    /*                                    检查部分                                    */
    /* -------------------------------------------------------------------------- */
    
    /* CNF公式集为空，必能满足 */
    if (this->CL.empty()) {
      /***/ cout << "***FORMULA IS EMPTY: It can be satisfied." << endl;
      /***/ cout << "***ALGORITHM FINISHED." << endl;
      return true;
    }
    /* CNF公式集包含空子句，不可能满足 */
    if (this->containEmptyClause) {
      /***/ cout << "***FORMULA CONTAIN EMPTY CLAUSES: backtrack." << endl
                 << endl;
      this->containEmptyClause = false;
      continue;
    }

    
    /* -------------------------------------------------------------------------- */
    /*                                 将进一步搜索步骤推入栈                                */
    /* -------------------------------------------------------------------------- */
    
    /* 处理完成，该分支并未被证实或证伪，将下一层的赋值选择推入栈 */
    STACK.push(abs(Status) + 1);
    STACK.push(-abs(Status) - 1);
    /***/ cout << endl;
  }
  /***/ cout << "***The formula cannot be satisfied." << endl;
  /***/ cout << "***ALGORITHM FINISHED." << endl;
  return false;
}