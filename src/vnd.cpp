#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <chrono>

//#include "Data.cpp"
#include "Model.cpp"
//#include "dataStructs.h"

#define MAX_INT 2147483647

using namespace std;

Data *data;
Solution pool;

//Declaracao das funcoes
void initRoutes(Solution &);
void VND_intra(Route&);
void VND_inter(Solution&);
void ILS(Solution&, int);

bool swap(std::vector<int>&, int&);
bool two_opt(std::vector<int>&, int&);
bool reinsertion(std::vector<int>&, int&, int);

pair<int,int> swapInter(Solution&);
pair<int,int> reinsertionInter(Solution&, int);

Solution disturb(Solution);

void addToPool(Route&);

int main(int argc, char** argv) {
  data = new Data(argv[1]);

  Solution sol;
  initRoutes(sol);
  
  for(int i = 0; i < sol.routes.size(); i++) {
    cout << endl << "rota: " << i << endl;
    cout << "demanda: " << sol.routes[i].demand << endl;
    cout << "custo: " << sol.routes[i].cost << endl;
    cout << "clientes: " << endl;
    for(int j = 0; j < sol.routes[i].clients.size(); j++) {
      cout << sol.routes[i].clients[j] << " ,";
    }
    cout << endl;
  }
  
  ILS(sol,25);

 
  for(int i = 0; i < sol.routes.size(); i++) {
    cout << endl << "rota: " << i << endl;
    cout << "demanda: " << sol.routes[i].demand << endl;
    cout << "custo: " << sol.routes[i].cost << endl;
    cout << "clientes: " << endl;
    for(int j = 0; j < sol.routes[i].clients.size(); j++) {
      cout << sol.routes[i].clients[j] << " ,";
    }
    cout << endl;
  }

  cout << "#######################";
  /*for(int i = 0; i < pool.routes.size(); i++) {
    cout << endl << "rota: " << i << endl;
    cout << "demanda: " << pool.routes[i].demand << endl;
    cout << "custo: " << pool.routes[i].cost << endl;
    cout << "clientes: " << endl;
    for(int j = 0; j < pool.routes[i].clients.size(); j++) {
      cout << pool.routes[i].clients[j] << " ,";
    }
    cout << endl;
  }*/

  //exato com pool de rotas
  Model mo(pool, data);
  Solution newSol = mo.solveModel();

  for(int i = 0; i < newSol.routes.size(); i++) {
    cout << endl << "rota: " << i << endl;
    cout << "demanda: " << newSol.routes[i].demand << endl;
    cout << "custo: " << newSol.routes[i].cost << endl;
    cout << "clientes: " << endl;
    for(int j = 0; j < newSol.routes[i].clients.size(); j++) {
      cout << newSol.routes[i].clients[j] << " ,";
    }
    cout << endl;
  }

  
  

  return 0;
}

// Funcao para criar as rotas validas iniciais
void initRoutes(Solution &sol) {
    bool *visited = new bool[data->clients_demands.size()];

    for(int i = 0; i < data->clients_demands.size(); i++) {
      visited[i] = false;
    }
    
    std::vector<pair<int,int>> sorted_demands = data->clients_demands;
    sort(sorted_demands.begin(), sorted_demands.end());
    sorted_demands.erase(sorted_demands.begin());

    for(int v = 0; v < data->vehicles; v++ ) { 
        vector<int> candidates;
        int candidates_demand = 0;
        
        for(int i = 0; i < sorted_demands.size(); i++) {
            if(!visited[sorted_demands[i].second] && sorted_demands[i].first + candidates_demand <= data->capacity) {
                candidates.push_back(sorted_demands[i].second);
                candidates_demand += sorted_demands[i].first;
                visited[sorted_demands[i].second] = true;
                cout << sorted_demands[i].second << " ";
            }
        }
        

        Route route;
        route.clients = {0,0};
        route.cost = 0;
        route.demand = candidates_demand;

        int sizeInitSubTour = (candidates.size() >= 3) ? 3 : candidates.size();
        for(int i = 0; i < sizeInitSubTour; i++) {
          int j = rand()% candidates.size();
          route.cost += data->matrizAdj[route.clients[0]][candidates[j]] + data->matrizAdj[candidates[j]][route.clients[1]] - data->matrizAdj[route.clients[0]][route.clients[1]];
          route.clients.insert(route.clients.begin() + 1, candidates[j]);
          candidates.erase(candidates.begin() + j);
        }

        while(!candidates.empty()) {
          int pos1,pos2;
          double delta = std::numeric_limits<double>::infinity();
          double newDelta;
          
          int candidatesSize = candidates.size();
          for(int i = 1; i < route.clients.size(); i++) {
            for(int j = 0; j < candidatesSize; j++){
              newDelta = data->matrizAdj[route.clients[i-1]][candidates[j]] + data->matrizAdj[route.clients[i]][candidates[j]] - data->matrizAdj[route.clients[i-1]][route.clients[i]];
              
              if(newDelta < delta) {
                delta = newDelta;
                pos1 = i;
                pos2 = j;
              }
            }
          }

          route.clients.insert(route.clients.begin() + pos1, candidates[pos2]);
          route.cost += delta;
          candidates.erase(candidates.begin() + pos2);
        }

        sol.routes.push_back(route);
    }
    

}

void ILS(Solution &sol, int Iils) {
  Solution newSol = sol;
  for(int j = 0; j < Iils; j++) {
    //Intra
    for(int i = 0; i < data->vehicles; i++) {
      VND_intra(newSol.routes[i]);
    }
    //Inter
    VND_inter(newSol);
    //Update best sol
    if(newSol.cost() < sol.cost()) {
      sol = newSol;
      j = 0;
      cout << "ils reset" << endl;
    }
    //Disturb
    newSol = disturb(sol);
    }
}

void VND_intra(Route &route){
  vector<int> neighbors = {1,2,3,4,5};
  bool improvemment = false;
 
  while(!neighbors.empty()){
    int index = rand()% neighbors.size();
    switch (neighbors[index]) {
      case 1:
          improvemment = swap(route.clients, route.cost);
          break;
      case 2:
          improvemment = two_opt(route.clients, route.cost);
          break;
      case 3:
          improvemment = reinsertion(route.clients, route.cost, 1);
          break;
      case 4:
          improvemment = reinsertion(route.clients, route.cost, 2);
          break;
      case 5:
          improvemment = reinsertion(route.clients, route.cost, 3);
          break;
    }
    
    if(improvemment) {
      neighbors = {1,2,3,4,5};
    } else {
      neighbors.erase(neighbors.begin()+index);
    }
    improvemment = false;
  }
    
  addToPool(route);

}

void VND_inter(Solution &sol) {
  vector<int> neighbors = {1,2,3,4};
  pair<int,int> pair;
 
  while(!neighbors.empty()){
    int index = rand()% neighbors.size();
    switch (neighbors[index]) {
      case 1:
          pair = swapInter(sol);
          break;
      case 2:
          pair = reinsertionInter(sol, 1);
          break;
      case 3:
          pair = reinsertionInter(sol, 2);
          break;
      case 4:
          pair = reinsertionInter(sol, 3);
          break;
      
    }
    
    if(pair.first != pair.second) {
      VND_intra(sol.routes[pair.first]);
      VND_intra(sol.routes[pair.second]);
      neighbors = {1,2,3,4};
    } else {
      neighbors.erase(neighbors.begin()+index);
    }
  }
}

bool swap(std::vector<int> &sol, int &cost) {
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

bool two_opt(std::vector<int> &sol, int &cost) {
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

bool reinsertion(std::vector<int> &sol, int &cost, int tam) {
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

pair<int,int> swapInter(Solution &sol) {
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
   
    return pair<int, int>(drt1,drt2);
  }
  return pair<int, int>(0,0);
}

pair<int,int> reinsertionInter(Solution &sol, int tam) {
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

    return pair<int,int>(drt1,drt2);
  }
  
  return pair<int,int>(0,0);
}

Solution disturb(Solution sol) {
  int solSize = sol.routes.size();
  vector<int> pos1, pos2, drt1_demandDelta, drt2_demandDelta, drt1_costDelta, drt2_costDelta;
  
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

    if(pos1.size() == 0) {
      //movimento intra
      cout << "AQUI" << endl;
    } else {
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

void addToPool(Route &route) {
  int size = route.clients.size();
  for(Route r : pool.routes) {
    if(r.cost == route.cost) {
      if(r.clients.size() == size) {
        if(r.clients[1] == route.clients[1] && r.clients[size/2] == route.clients[size/2] && r.clients[size - 1] == route.clients[size - 1]) {
          return;
        }
      }
    }
  }
  pool.routes.push_back(route);
}