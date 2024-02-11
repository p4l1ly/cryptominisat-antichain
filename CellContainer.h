#include <deque>
#include <set>
#include <vector>

#include <cryptominisat5/cryptominisat.h>

using std::set;
using std::deque;
using std::vector;
using namespace CMSat;

struct LeastSizeCompare
{
    bool operator()(const vector<Lit>& lhs, const vector<Lit>& rhs) const
    {
        if (lhs.size() != rhs.size()) return lhs.size() < rhs.size();

        // We're sure that the inserted elements are unequal and if the sizes are equal, we don't
        // care about the order.
        return true;
    }
};


struct CellContainer {
    virtual void add(vector<Lit> &x) = 0;
    virtual int size() const = 0;
    virtual vector<Lit> pop() = 0;
    virtual ~CellContainer() {};
};

class CellContainerSet : public CellContainer {
    set<vector<Lit>, LeastSizeCompare> data;
public:
    CellContainerSet() {}
    void add(vector<Lit> &x) { data.emplace(std::move(x)); }
    int size() const { return data.size(); }
    vector<Lit> pop() {
        auto it = data.begin();
        vector<Lit> result(std::move(*it));
        data.erase(it);
        return std::move(result);
    }
};

class CellContainerBfs : public CellContainer {
    deque<vector<Lit>> data;
public:
    CellContainerBfs() {}
    void add(vector<Lit>& x) { data.emplace_back(std::move(x)); }
    int size() const { return data.size(); }
    vector<Lit> pop() {
      vector<Lit> result(std::move(data.front()));
      data.pop_front();
      return std::move(result);
    }
};

class CellContainerDfs : public CellContainer {
    vector<vector<Lit>> data;
public:
    CellContainerDfs() {}
    void add(vector<Lit>& x) { data.emplace_back(std::move(x)); }
    int size() const { return data.size(); }
    vector<Lit> pop() {
      vector<Lit> result(std::move(data.back()));
      data.pop_back();
      return std::move(result);
    }
};

