#ifndef BuildHeuristic_hpp
#define BuildHeuristic_hpp

#include "DataStructs.hpp"
#include "Disturb.hpp"
#include "Data.hpp"

#include <vector>
#include <algorithm>

Route cheapestInsertion(Data* data, std::vector<int> candidates, int candidates_demand);

void buildRoutes(Data* data,Solution &sol);

#endif /* BuildHeuristic_hpp */
