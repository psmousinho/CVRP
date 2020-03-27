#include "include/ILS.hpp"
#include "include/LocalSearch.hpp"


Pool ILS(Data* data, Solution &sol, int Iils) {
  Pool pool;
  
  Solution newSol = sol;
  for(int j = 0; j < Iils; j++) {
    //Intra
    for(int i = 0; i < data->vehicles; i++) {
      VND_intra(data, pool, newSol.routes[i]);
    }
    //Inter
    VND_inter(data, pool, newSol);
    //Update best sol
    if(newSol.cost() < sol.cost()) {
      sol = newSol;
      j = 0;
      std::cout << "ils reset" << std::endl;
    }
    //Disturb
    newSol = injectionChain(data,sol);
  }

  return pool;
}