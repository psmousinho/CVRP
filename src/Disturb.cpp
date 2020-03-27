
#include "include/Disturb.hpp"

Solution injectionChain(Data* data, Solution sol) {
  int solSize = sol.routes.size();
  std::vector<int> pos1, pos2, drt1_demandDelta, drt2_demandDelta, drt1_costDelta, drt2_costDelta;
  
  for(int r = 0; r < solSize-1; r++) {
    Route rt1 = sol.routes[r];
    Route rt2 = sol.routes[(r+1)%solSize];

    int sol1Size = rt1.clients.size()-1; 
    int sol2Size = rt2.clients.size()-1;
    int cost_aux1, cost_aux2, demand_aux1, demand_aux2;

    for(int i = 1; i < sol1Size; i++) {
      cost_aux1 = - data->matrizAdj[rt1.clients[i]][rt1.clients[i-1]] - data->matrizAdj[rt1.clients[i]][rt1.clients[i+1]];
      
      for(int j = 1; j < sol2Size; j++) {
        demand_aux1 = + data->clients_demands[rt2.clients[j]].first - data->clients_demands[rt1.clients[i]].first;
        demand_aux2 = - data->clients_demands[rt2.clients[j]].first + data->clients_demands[rt1.clients[i]].first;

        if(!(data->capacity >= rt1.demand + demand_aux1 && data->capacity >= rt2.demand + demand_aux2 ))
          continue;
        
        cost_aux1 += data->matrizAdj[rt2.clients[j]][rt1.clients[i-1]] + data->matrizAdj[rt2.clients[j]][rt1.clients[i+1]];
        cost_aux2 = data->matrizAdj[rt1.clients[i]][rt2.clients[j-1]] + data->matrizAdj[rt1.clients[i]][rt2.clients[j+1]] 
                  - data->matrizAdj[rt2.clients[j]][rt2.clients[j-1]] - data->matrizAdj[rt2.clients[j]][rt2.clients[j+1]]; 
          
      
        pos1.push_back(i);
        pos2.push_back(j);
        drt1_demandDelta.push_back(demand_aux1);
        drt2_demandDelta.push_back(demand_aux2);
        drt1_costDelta.push_back(cost_aux1);
        drt2_costDelta.push_back(cost_aux2);
          
      }
    }

    if(pos1.size() != 0) {
        int index = rand() % pos1.size();
      
        sol.routes[r].cost +=  drt1_costDelta[index];
        sol.routes[r+1].cost +=  drt2_costDelta[index];
        
        sol.routes[r].demand += drt1_demandDelta[index];
        sol.routes[r+1].demand += drt2_demandDelta[index];

        int aux = sol.routes[r].clients[pos1[index]];
        sol.routes[r].clients[pos1[index]] = sol.routes[r+1].clients[pos2[index]];
        sol.routes[r+1].clients[pos2[index]] = aux;

        pos1.clear();
        pos2.clear();
        drt1_costDelta.clear();
        drt2_costDelta.clear();
        drt1_demandDelta.clear();
        drt2_demandDelta.clear();
    }

    return sol;
  }
}