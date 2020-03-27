#ifndef LocalSearch_hpp
#define LocalSearch_hpp

#include "DataStructs.hpp"
#include "Neighborhood.hpp"
#include "Data.hpp"

// Busca local (intrarota)
void VND_intra(Data* data, Pool &pool, Route &route);
// Busca local (interrota)
void VND_inter(Data* data, Pool &pool, Solution &sol);

#endif /* LocalSearch_hpp */
