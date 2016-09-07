//
// Created by Gilbert Francois on 05-09-16.
//

#ifndef GAMEOFLIFE_GAMEOFLIFEKERNEL_H
#define GAMEOFLIFE_GAMEOFLIFEKERNEL_H


#include <thread>

class GameOfLifeKernel {
public:
    GameOfLifeKernel(int rows, int cols, bool widthThreads);

    virtual ~GameOfLifeKernel();

    void printDomain();

    void timeStep();
    
    const int getXtAt(int row, int col);

    int **getXt0() const;

private:
    const int rows;
    const int cols;
    const bool withThreads;
    std::thread *threads;
    int **Xt0;
    int **Xt1;
    int *Xt2;
    int numberOfThreads;
    int slice;

    void initialConditions();

    void initialConditionsDomainSlice(const int minRow, const int maxRow);

    void timeStepInnerDomainSlice(const int minRow, const int maxRow);

    void timestepBoundaries();

    void fx(const int i, const int j, const int sum);

    void startThreads(void (GameOfLifeKernel::*fn)(int, int), GameOfLifeKernel *gameOfLifeKernel);

    int randMinMax(int min, int max);

    void zeros(int **X, const int rows, const int cols);
};


#endif //GAMEOFLIFE_GAMEOFLIFEKERNEL_H
