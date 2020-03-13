#include <vector>

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
