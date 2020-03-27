#include "include/BuildHeuristic.hpp"

Route cheapestInsertion(Data* data, std::vector<int> candidates, int candidates_demand) {
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

    return route;
}



void buildRoutes(Data* data,Solution &sol) {
    bool *visited = new bool[data->clients_demands.size()];

    for(int i = 0; i < data->clients_demands.size(); i++) {
      visited[i] = false;
    }
    
    std::vector<std::pair<int,int>> sorted_demands = data->clients_demands;
    sort(sorted_demands.begin(), sorted_demands.end());
    sorted_demands.erase(sorted_demands.begin());
    
    for(int v = 0; v < data->vehicles; v++ ) { 
        std::vector<int> candidates;
        int candidates_demand = 0;
        
        for(int i = sorted_demands.size()-1; i >= 0; i--) {
            if(!visited[sorted_demands[i].second] && sorted_demands[i].first + candidates_demand <= data->capacity) {
                candidates.push_back(sorted_demands[i].second);
                candidates_demand += sorted_demands[i].first;
                visited[sorted_demands[i].second] = true;
                std::cout << sorted_demands[i].second << " ";
            }
        }
        
        Route route = cheapestInsertion(data, candidates, candidates_demand);
        sol.routes.push_back(route);
    }
    
}