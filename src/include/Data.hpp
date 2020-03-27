#ifndef Data_hpp
#define Data_hpp

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class Data {
    public:
        std::string instance_name;
        int dimension;
        int vehicles;
        int capacity;
        std::vector<std::pair<int,int>> clients_demands;
        int** matrizAdj;

    Data(std::string file_path);
};

#endif /* Data_hpp */ 