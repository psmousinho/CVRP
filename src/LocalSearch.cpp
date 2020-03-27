#include "include/LocalSearch.hpp"

void VND_intra(Data* data, Pool &pool, Route &route){
  std::vector<int> neighbors = {1,2,3,4,5};
  bool improvemment = false;
 
  while(!neighbors.empty()){
    int index = rand()% neighbors.size();
    switch (neighbors[index]) {
      case 1:
          improvemment = swap(data, route.clients, route.cost);
          break;
      case 2:
          improvemment = two_opt(data, route.clients, route.cost);
          break;
      case 3:
          improvemment = reinsertion(data, route.clients, route.cost, 1);
          break;
      case 4:
          improvemment = reinsertion(data, route.clients, route.cost, 2);
          break;
      case 5:
          improvemment = reinsertion(data, route.clients, route.cost, 3);
          break;
    }
    
    if(improvemment) {
      neighbors = {1,2,3,4,5};
    } else {
      neighbors.erase(neighbors.begin()+index);
    }
    improvemment = false;
  }
    
  pool.addToPool(route);

}

void VND_inter(Data* data,Pool &pool, Solution &sol) {
  std::vector<int> neighbors = {1,2,3,4};
  std::pair<int,int> pair;
 
  while(!neighbors.empty()){
    int index = rand()% neighbors.size();
    switch (neighbors[index]) {
      case 1:
          pair = swapInter(data, sol);
          break;
      case 2:
          pair = reinsertionInter(data, sol, 1);
          break;
      case 3:
          pair = reinsertionInter(data, sol, 2);
          break;
      case 4:
          pair = reinsertionInter(data, sol, 3);
          break;
      
    }
    
    if(pair.first != pair.second) {
      VND_intra(data, pool, sol.routes[pair.first]);
      VND_intra(data, pool, sol.routes[pair.second]);
      neighbors = {1,2,3,4};
    } else {
      neighbors.erase(neighbors.begin()+index);
    }
  }
}