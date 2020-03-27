#ifndef Model_hpp
#define Model_hpp


#include "Data.hpp"
#include "DataStructs.hpp"

#include <iostream>
#include <vector>
#include <ilcplex/ilocplex.h>

class Model {
    public:
        Data* data;
        Pool pool;
        
        Model(Pool p, Data* d);

        Solution solveModel();
};        

#endif /* Model_hpp */ 