#include "include/Data.hpp"
#include "include/DataStructs.hpp"
#include "include/BuildHeuristic.hpp"
#include "include/ILS.hpp"
#include "include/Model.hpp"

using namespace std;

int main(int argc, char** argv) {
  Data* data = new Data(argv[1]);
  
  Solution sol;
  buildRoutes(data, sol);

  Pool pool = ILS(data, sol, 25);

  Model mod(pool, data);
  sol = mod.solveModel();

  cout << endl << "Rotas Encontradas:" << endl;
  cout << "##########################" << endl; 
  int custoTotal = 0;
  for(int i = 0; i < sol.routes.size(); i++) {
    cout << endl << "rota: " << i << endl;
    cout << "demanda: " << sol.routes[i].demand << endl;
    cout << "custo: " << sol.routes[i].cost << endl;
    cout << "clientes: " << endl;
    for(int j = 0; j < sol.routes[i].clients.size(); j++) {
      cout << sol.routes[i].clients[j] << " ,";
    }
    cout << endl;
    custoTotal +=  sol.routes[i].cost;
  }
  cout << "##########################" << endl;   
  cout << "Custo Total: " << custoTotal << endl;

  cout << endl << "Tamanho do pool: " << pool.sol.routes.size() << endl;
  cout << "Taxa de Repeticao no pool: " << (float)pool.repeat/pool.total << endl;

  return 0;
}