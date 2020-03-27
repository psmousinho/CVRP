#include "include/Model.hpp"
        
Model::Model(Pool p, Data* d) : pool(p), data(d){};

Solution Model::solveModel() {
    int poolSize = pool.sol.routes.size();
    int present[poolSize][data->dimension] = {0};

    for(int i = 0; i < poolSize; i++) {
        for(int j = 0; j < data->dimension -1; j++) {
            present[i][j] = 0;
        }
    }

    IloEnv env;
    IloModel modelo(env);
    
    for(int i = 0; i < pool.sol.routes.size(); i++) {
        for(int j = 1; j < pool.sol.routes[i].clients.size() -1; j++) {
            present[i][pool.sol.routes[i].clients[j]] = 1;
        }
    }
    ///////////////////////////////////////////////////////
    //Variaveis

    //variavel Ri: Ri = 1, se a rota esta sendo utilizada
    IloBoolVarArray r(env, poolSize);

    //adiciona as variaveis R ao modelo
    for(int i = 0; i <  poolSize; i++) {
        modelo.add(r[i]);
    }

    //fim das variaveis
    ///////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////
    //Criando a Função Objetivo (FO) 
    IloExpr sumX(env);
    for(int i = 0; i < poolSize; i++) {
        sumX += pool.sol.routes[i].cost * r[i];
    }
    
    // Adicionando a FO
    modelo.add(IloMinimize(env, sumX));
    //////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////
    //Restricoes
    
    //Restricoes do numero de veiculos
    IloExpr sumR(env);
    for(int i = 0; i < poolSize; i++) {
        sumR += r[i];
    }
    IloRange ran = (sumR <= data->vehicles);
    ran.setName("NV");
    modelo.add(ran);

    
        //Restrição de exclusividade do cliente
    for (int i = 1; i < data->dimension; i++) {
        IloExpr temp(env);
        for (int j = 0; j < poolSize; j++) {
            temp += present[j][i]*r[j];
        }
        modelo.add(temp == 1);
        temp.end();
    }

    //fim das restricoes
    ////////////////////////////////////////////////////////

    IloCplex cvrp(modelo);
    cvrp.setParam(IloCplex::TiLim, 2*60*60);
    cvrp.setParam(IloCplex::Threads, 1);
    cvrp.exportModel("modelo.lp");

    try {
        cvrp.solve();
        std::cout << std::endl << "status: " << cvrp.getStatus() << std::endl;
        std::cout << "custo: " << cvrp.getObjValue() << std::endl;
        
        Solution newSol;
        
        for(int i = 0; i < poolSize; i++) {
            if(cvrp.getValue(r[i]) > 0.9) {
                newSol.routes.push_back(pool.sol.routes[i]);
            }
        }

        return newSol;

    }
    catch(IloException& e) {
        std::cout << e;
    }
}     