#include "include/Neighborhood.hpp"

bool swap(Data* data, std::vector<int> &sol, int &cost) {
  int pos1, pos2;
  int delta = MAX_INT;

  int solSize = sol.size();
  for(int i = 1; i < solSize-1; i++) {
    int aux1 = - data->matrizAdj[sol[i]][sol[i-1]] - data->matrizAdj[sol[i]][sol[i+1]];
    for(int j = i+1; j < solSize-1; j++) {
      int newDelta;
      if(i != j-1){
        newDelta = aux1 + data->matrizAdj[sol[i]][sol[j-1]] + data->matrizAdj[sol[i]][sol[j+1]] + data->matrizAdj[sol[j]][sol[i-1]] + data->matrizAdj[sol[j]][sol[i+1]]
                  - data->matrizAdj[sol[j]][sol[j-1]] - data->matrizAdj[sol[j]][sol[j+1]] ;
      } else {
        newDelta = data->matrizAdj[sol[i-1]][sol[j]] + data->matrizAdj[sol[j+1]][sol[i]] - data->matrizAdj[sol[i-1]][sol[i]] - data->matrizAdj[sol[j+1]][sol[j]];
      }
      if(newDelta < delta){
        pos1 = i;
        pos2 = j;
        delta = newDelta;
      }
    }
  }

  if(delta < 0) {
    int aux = sol[pos1];
    sol[pos1] = sol[pos2];
    sol[pos2] = aux;
    cost += delta;
    return true;
  }
  return false;
}

bool two_opt(Data* data, std::vector<int> &sol, int &cost) {
  int pos1, pos2;
  int delta = MAX_INT;

  int solSize = sol.size();
  for(int i = 1; i < solSize-1; i++) {
    int aux = - data->matrizAdj[sol[i-1]][sol[i]];
    for(int j = i+1; j < solSize-1; j++) {
      int newDelta =  aux + data->matrizAdj[sol[i-1]][sol[j]] + data->matrizAdj[sol[j+1]][sol[i]] - data->matrizAdj[sol[j+1]][sol[j]];
      if(newDelta < delta){
        pos1 = i;
        pos2 = j;
        delta = newDelta;
      }
    }
  }

  if(delta < 0) {
    reverse(sol.begin() + pos1, sol.begin() + pos2+1);
    cost += delta;
    return true;
  }
  return false;
}

bool reinsertion(Data* data, std::vector<int> &sol, int &cost, int tam) {
  int pos1, pos2;
  int delta = MAX_INT;

  int solSize = sol.size(); 
  for(int i = 1; i < solSize-tam; i++) {
    int aux = + data->matrizAdj[sol[i-1]][sol[i+tam]] - data->matrizAdj[sol[i-1]][sol[i]];
    for(int j = 1; j < solSize; j++) {
      if(j >= i && j <= i+tam)
        continue;
      
      int newDelta = aux + data->matrizAdj[sol[j-1]][sol[i]] + data->matrizAdj[sol[i+tam-1]][sol[j]] - data->matrizAdj[sol[i+tam-1]][sol[i+tam]] - data->matrizAdj[sol[j-1]][sol[j]];
      if(newDelta < delta) {
        pos1 = i;
        pos2 = j;
        delta = newDelta;
      }
    }
  }

  if(delta < 0) {
    std::vector<int> vec;
    if(pos1 < pos2) {
      vec.insert(vec.end(),sol.begin()+pos1,sol.begin()+pos1+tam);
      sol.erase(sol.begin()+pos1,sol.begin()+pos1+tam);
      sol.insert(sol.begin()+pos2-tam,vec.begin(),vec.end());
    } else {
      vec.insert(vec.end(),sol.begin()+pos1,sol.begin()+pos1+tam);
      sol.erase(sol.begin()+pos1,sol.begin()+pos1+tam);
      sol.insert(sol.begin()+pos2,vec.begin(),vec.end());
    }

    cost += delta;
    return true;
  }
  return false;
}

std::pair<int,int> swapInter(Data* data, Solution &sol) {
  int pos1, pos2;
  int drt1, drt2;
  int drt1_demandDelta, drt2_demandDelta;
  int drt1_costDelta, drt2_costDelta;
  int delta = MAX_INT;

  for(int r = 0 ; r < sol.routes.size(); r++) {
    for(int s = r+1; s < sol.routes.size(); s++) {
      
      Route rt1 = sol.routes[r];
      Route rt2 = sol.routes[s];

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
            
          if(cost_aux1 + cost_aux2 < delta){
            pos1 = i;
            pos2 = j;
            drt1 = r;
            drt2 = s;
            drt1_demandDelta = demand_aux1;
            drt2_demandDelta = demand_aux2;
            drt1_costDelta = cost_aux1;
            drt2_costDelta = cost_aux2;
            delta = cost_aux1 + cost_aux2;
          }
        }
      }
    }
  }

  if(delta < 0) {
    sol.routes[drt1].cost +=  drt1_costDelta;
    sol.routes[drt2].cost +=  drt2_costDelta;
    
    sol.routes[drt1].demand += drt1_demandDelta;
    sol.routes[drt2].demand += drt2_demandDelta;

    int aux = sol.routes[drt1].clients[pos1];
    sol.routes[drt1].clients[pos1] = sol.routes[drt2].clients[pos2];
    sol.routes[drt2].clients[pos2] = aux;
   
    return std::pair<int, int>(drt1,drt2);
  }
  return std::pair<int, int>(0,0);
}

std::pair<int,int> reinsertionInter(Data* data,Solution &sol, int tam) {
  int pos1, pos2;
  int drt1, drt2;
  int demandDelta;
  int drt1_costDelta, drt2_costDelta;
  int delta = MAX_INT;

  for(int r = 0 ; r < sol.routes.size(); r++) {
    for(int s = 0; s < sol.routes.size(); s++) {
      if(r == s) continue;

      Route rt1 = sol.routes[r];
      Route rt2 = sol.routes[s];

      int sol1Size = rt1.clients.size();
      int sol2Size = rt2.clients.size(); 
      int cost_aux1, cost_aux2;
      int subTour_demand, subTour_cost;
      
      for(int i = 1; i < sol1Size-tam; i++) {
        cost_aux1 = + data->matrizAdj[rt1.clients[i-1]][rt1.clients[i+tam]] - data->matrizAdj[rt1.clients[i-1]][rt1.clients[i]] - data->matrizAdj[rt1.clients[i+tam-1]][rt1.clients[i+tam]];
                

        subTour_demand = 0;
        subTour_cost = 0;
        for(int j = i; j < i+tam; j++) {
          subTour_demand += data->clients_demands[rt1.clients[j]].first;
          if(j+1 != i+tam)
            subTour_cost += data->matrizAdj[rt1.clients[j]][rt1.clients[j+1]];
        }

        for(int j = 1; j < sol2Size; j++) {
          if(!(data->capacity >= rt2.demand + subTour_demand))
            continue;
          
          cost_aux2 = subTour_cost + data->matrizAdj[rt2.clients[j-1]][rt1.clients[i]] + data->matrizAdj[rt1.clients[i+tam-1]][rt2.clients[j]]  - data->matrizAdj[rt2.clients[j-1]][rt2.clients[j]];
          
          if(cost_aux1 + cost_aux2 < delta) {
            pos1 = i;
            pos2 = j;
            drt1 = r;
            drt2 = s;
            demandDelta = subTour_demand;
            drt1_costDelta = cost_aux1;
            drt2_costDelta = cost_aux2;
            delta = cost_aux1 + cost_aux2;
          }
        }
      }
    }
  }

  if(delta < 0) {
    sol.routes[drt1].cost += drt1_costDelta;
    sol.routes[drt2].cost += drt2_costDelta;
    
    sol.routes[drt1].demand -= demandDelta;
    sol.routes[drt2].demand += demandDelta;
    
    std::vector<int> vec;
    vec.insert(vec.end(), sol.routes[drt1].clients.begin()+pos1,sol.routes[drt1].clients.begin()+pos1+tam);
    sol.routes[drt1].clients.erase(sol.routes[drt1].clients.begin()+pos1,sol.routes[drt1].clients.begin()+pos1+tam);
    sol.routes[drt2].clients.insert(sol.routes[drt2].clients.begin()+pos2,vec.begin(),vec.end());

    return std::pair<int,int>(drt1,drt2);
  }
  
  return std::pair<int,int>(0,0);
}