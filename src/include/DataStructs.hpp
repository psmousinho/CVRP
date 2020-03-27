#ifndef DataStructs_hpp
#define DataStructs_hpp

#include <vector>
#include <iostream>

typedef struct rou {
  std::vector<int> clients;
  int demand;
  int cost;
} Route;

typedef struct sol {
  std::vector<Route> routes;

  int cost() {
    int aux = 0;
    for(Route r : routes) {
        aux += r.cost;
    }
    return aux;
  }
  
} Solution;

typedef struct po {
  Solution sol;
  int total = 0;
  int repeat = 0;

  void addToPool(Route &route) {
    total++;
    int size = route.clients.size();
    for(Route r : sol.routes) {
      if(r.cost == route.cost) {
        if(r.clients.size() == size) {
          if(r.clients[1] == route.clients[1] && r.clients[size/2] == route.clients[size/2] && r.clients[size - 2] == route.clients[size - 2]) {
            repeat++;
            return;
          }
        }
      }
    }
    sol.routes.push_back(route);
  }
} Pool;

#endif /* DataStructs_hpp */