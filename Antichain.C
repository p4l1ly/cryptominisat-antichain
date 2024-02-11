#include <cryptominisat5/cryptominisat.h>
#include <assert.h>
#include <vector>
#include <sstream>
#include <iterator>
#include <unordered_set>

#include "CellContainer.h"

using std::vector;
using std::unordered_set;
using namespace CMSat;


std::pair<bool, std::vector<int>> getIntLine(bool required = true) {
  std::string line;
  if (getline(std::cin, line)) {
    std::istringstream is(line);
    return std::pair(
      true,
      std::move(std::vector<int> {std::istream_iterator<int>(is), std::istream_iterator<int>()})
    );
  }
  if (required) {
    std::cerr << "Too few lines" << std::endl;
    exit(1);
  }
  return std::pair(false, std::vector<int>());
}


void parse_cnfafa(
  SATSolver &solver,
  int& acnt,
  std::vector<Lit> &all_outputs,
  std::unordered_set<unsigned> &finals_set
) {
  acnt = getIntLine().second[0];
  std::vector<int> outputs(std::move(getIntLine().second));
  std::vector<int> finals(std::move(getIntLine().second));
  std::vector<int> pureVars(std::move(getIntLine().second));
  std::vector<int> upwardClauses(std::move(getIntLine().second));
  std::vector<int> posqOutputs(std::move(getIntLine().second));

  int nVars = acnt + outputs.size();

  std::vector<std::vector<int>> clauses;
  while (true) {
    auto intLine = getIntLine(false);
    if (!intLine.first) break;
    clauses.push_back(std::move(intLine.second));
  }

  for (int output: outputs) {
    int var = abs(output) - 1;
    Lit lit = output > 0 ? Lit(var, false) : Lit(var, true);
    all_outputs.push_back(lit);
  }

  for (auto clause: clauses) {
    for (auto lit: clause) {
      int var = abs(lit) - 1;
      if (var + 1 > nVars) {
        nVars = var + 1;
      }
    }
  }

  for (int final_: finals) finals_set.insert(final_);

  solver.new_vars(nVars);

  {
    vector<Lit> lits;
    int i = 0;
    for (auto clause: clauses) {
      lits.clear();

      bool optional = false;
      if (clause.back() == 0) {
        optional = true;
        clause.pop_back();
      }

#ifdef OPTIONAL_CLAUSES
      if (!optional)
#endif
      {
        for (int lit: clause) {
          int var = abs(lit) - 1;
          lits.push_back(lit > 0 ? Lit(var, false) : Lit(var, true));
        }
        solver.add_clause(lits);
      }
      ++i;
    }
  }
}


bool run()
{
    SATSolver solver;
    int acnt;
    vector<Lit> all_outputs;
    unordered_set<unsigned> finals;

    // solver.set_num_threads(4);

    parse_cnfafa(solver, acnt, all_outputs, finals);

    if (finals.contains(0)) return true;
    vector<Lit> outputs;
    vector<Lit> inputs;
    vector<Lit> new_inputs;

    for (unsigned i = 0; i < all_outputs.size(); ++i) {
      if (!finals.contains(i)) {
        outputs.push_back(all_outputs[i]);
        inputs.push_back(Lit(i, true));
      }
    }

    solver.add_clause(outputs);

    unsigned solveCnt = 0;
    unsigned satCnt = 0;
    unsigned unsatCnt = 0;

#ifdef CELL_CONTAINER_SET
    CellContainerSet cell_container;
#elif CELL_CONTAINER_DFS
    CellContainerDfs cell_container;
#elif CELL_CONTAINER_BFS
    CellContainerBfs cell_container;
#else
    ERROR
#endif

    bool result;
    while (true) {
      ++solveCnt;
      lbool lresult = solver.solve(&inputs);
      if (lresult == l_Undef) {
        printf("UNDEF_SOLVE_RESULT!\n");
        exit(1);
      }
      result = lresult == l_True;

      while (result) {
        ++satCnt;

        const std::vector<lbool>& model = solver.get_model();

        outputs.clear();
        new_inputs.clear();

        for (int i = 0; i < all_outputs.size(); ++i) {
          Lit out = all_outputs[i];

          if ((model[out.var()] == l_True) == out.sign()) {
            outputs.push_back(out);
            new_inputs.push_back(Lit(i, true));
          }
          else if (i == 0) return true;
        }

        cell_container.add(new_inputs);

        solver.add_clause(outputs);

        ++solveCnt;
        lresult = solver.solve(&inputs);
        result = lresult == l_True;
      }

      ++unsatCnt;

      if (!cell_container.size()) return false;
      inputs = std::move(cell_container.pop());
    }
}

int main(int argc, char** argv) {
    srand(1345719);
    char mode = '3';
    if (argc >= 2) mode = argv[1][0];

    if (run()) {
        std::cout << "NOT_EMPTY" << std::endl;
    } else {
        std::cout << "EMPTY" << std::endl;
    }
}
