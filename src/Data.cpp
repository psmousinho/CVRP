
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

    Data(std::string file_path) {
        std::ifstream file(file_path);
        if(!file.is_open()) {
            std::cout << "Error while opening file: " << file_path << std::endl;
            exit(1);
        }

        std::string aux;

        file >> aux; // NAME:
        file >> instance_name;

        file >> aux; // DIMENSION:
        file >> dimension;

        file >> aux; // VEHICLES:
        file >> vehicles;

        file >> aux; // CAPACITY:
        file >> capacity;

        // Aloca os vetores de demanda e adjacencia
        clients_demands.resize(dimension);
        matrizAdj = new int*[dimension];
        for(int i = 0; i < dimension; i++) {
            matrizAdj[i] = new int[dimension];
        }

        file >> aux; // DEMAND_SECTION:
        for(int i = 0; i < dimension; i++) {
            file >> clients_demands[i].second; // index
            file >> clients_demands[i].first; // demanda
        }

        file >> aux; // EDGE_WEIGHT_SECTION
        for(int i = 0; i < dimension; i++) {
            for(int j = 0; j < dimension; j++) {
                file >> matrizAdj[i][j];
            }
        }

        file.close();
    }   
};