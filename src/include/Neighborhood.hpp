#ifndef Neighborhood_hpp
#define Neighborhood_hpp

#define MAX_INT 2147483647

#include "DataStructs.hpp"
#include "Data.hpp"

#include <algorithm>

// Movimentos de vizinhanca (intrarota)
bool swap(Data* data, std::vector<int> &sol, int &cost);
bool two_opt(Data* data, std::vector<int> &sol, int &cost);
bool reinsertion(Data* data, std::vector<int> &sol, int &cost, int tam);
// Movimentos de vizinhanca (interrota)
std::pair<int,int> swapInter(Data* data, Solution &sol);
std::pair<int,int> reinsertionInter(Data* data, Solution &sol, int tam);

#endif /* Neighborhood_hpp */
