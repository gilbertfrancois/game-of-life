//
// Created by Gilbert Francois on 05-09-16.
//

#include "GameOfLifeKernel.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <random>


#pragma mark - Public

GameOfLifeKernel::GameOfLifeKernel(int rows, int cols, bool with_threads) : rows(rows), cols(cols) {
    
    // Setup concurrency
    int n_cpus = std::thread::hardware_concurrency();
    int mod = 0;
    if (with_threads) {
        slice = (rows-2) / n_cpus;
        mod = (rows-2) % n_cpus;
        n_threads = n_cpus;
    } else {
        slice = rows-2;
        n_threads = 1;
    }
    threads = new std::thread[n_threads];
    std::cout << "--- Availabe CPU cores: " << n_cpus << ", using " << n_threads << " cores" << std::endl;
    std::cout << "--- Domain rows: " << rows << ", slice per thread: " << slice << ", Modulo: " << mod << std::endl;
    int t, r1, r2;
    r1 = 1;
    for (t = 0; t < n_threads; t++) {
        r2 = (r1 + slice < rows) ? r1 + slice : rows-1;
        int rt = r2 - r1;
        std::cout << "Slices:    " << std::setw(4) << r1 << " - " << std::setw(4) << r2 << ", " << std::setw(4) << rt << " rows." << std::endl;
        r1 += slice;
    }
    int remaining = ((rows-2) % n_threads);
    if (remaining > 0) {
        r1 = rows - 1 - remaining;
        r2 = rows - 1;
        int rt = r2 - r1;
        std::cout << "Remaining: " << std::setw(4) << r1 << " - " << std::setw(4) << r2 << ", " << std::setw(4) << rt << " rows." << std::endl;
    }

    
    // Alloc - init domain
    Xt0 = new int *[rows];
    Xt1 = new int *[rows];
    for (int i = 0; i < rows; i++) {
        Xt0[i] = new int[cols];
        Xt1[i] = new int[cols];
    }
    Xt2 = new int[rows*cols];
    zeros(Xt0, rows, cols);
    zeros(Xt1, rows, cols);
    initialConditions();
}

GameOfLifeKernel::~GameOfLifeKernel() {
    for (int i = 0; i < rows; i++) {
        delete[] Xt0[i];
        delete[] Xt1[i];
    }
    delete[] Xt0;
    delete[] Xt1;
    delete[] threads;
}

void GameOfLifeKernel::printDomain() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            
            std::cout << ((Xt0[i][j] == 1) ? "O" : ".");
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void GameOfLifeKernel::timeStep() {
    
    // compute inner domain
    startThreads(&GameOfLifeKernel::timeStepInnerDomainSlice, this);
    
    // compute boundaries
    timestepBoundaries();
    
    // swap buffers
    int **tmp = Xt0;
    Xt0 = Xt1;
    Xt1 = tmp;
}

const int GameOfLifeKernel::getXtAt(int row, int col) {
    return row > 0 && row < rows && col > 0 && col < cols ? Xt0[row][col] : -1;
}

int **GameOfLifeKernel::getXt0() const {
    return Xt0;
}


#pragma mark - Private

void GameOfLifeKernel::initialConditions() {
    
    startThreads(&GameOfLifeKernel::initialConditionsDomainSlice, this);
}

void GameOfLifeKernel::initialConditionsDomainSlice(const int minRow, const int maxRow) {
    for (int i = minRow; i < maxRow; i++) {
        for (int j = 0; j < cols; j++) {
            Xt0[i][j] = randMinMax(0, 1);
        }
    }
}

void GameOfLifeKernel::timeStepInnerDomainSlice(const int minRow, const int maxRow) {
    
    // Loop over inner domain
    for (int i = minRow; i < maxRow; i++) {
        for (int j = 1; j < cols - 1; j++) {
            int sum = Xt0[i - 1][j + 1] + Xt0[i][j + 1] + Xt0[i + 1][j + 1] +
            Xt0[i - 1][j] + Xt0[i + 1][j] +
            Xt0[i - 1][j - 1] + Xt0[i][j - 1] + Xt0[i + 1][j - 1];
            fx(i, j, sum);
        }
    }
}

void GameOfLifeKernel::timestepBoundaries() {
    
    int i, j, sum;
    
    // compute edges
    for (i = 1; i < rows-1; i++) {
        j = 0;
        sum = Xt0[i - 1][j + 1] + Xt0[i][j + 1] + Xt0[i + 1][j + 1] + Xt0[i - 1][0] + Xt0[i + 1][0];
        fx(i, j, sum);
        j = cols - 1;
        sum = Xt0[i - 1][j] + Xt0[i + 1][j] + Xt0[i - 1][j - 1] + Xt0[i][j - 1] + Xt0[i + 1][j - 1];
        fx(i, j, sum);
    }
    for (j = 1; j < cols-1; j++) {
        i = 0;
        sum = Xt0[i][j + 1] + Xt0[i + 1][j + 1] + Xt0[i + 1][j] + Xt0[i][j - 1] + Xt0[i + 1][j - 1];
        fx(i, j, sum);
        i = rows - 1;
        sum = Xt0[i - 1][j + 1] + Xt0[i][j + 1] + Xt0[i - 1][j] + Xt0[i - 1][j - 1] + Xt0[i][j - 1];
        fx(i, j, sum);
    }
    // compute corners
    i = 0;
    j = 0;
    sum = Xt0[i][j + 1] + Xt0[i + 1][j + 1] + Xt0[i + 1][j];
    fx(i, j, sum);
    i = 0;
    j = cols - 1;
    sum = Xt0[i][j - 1] + Xt0[i + 1][j - 1] + Xt0[i + 1][j];
    fx(i, j, sum);
    i = rows - 1;
    j = cols - 1;
    sum = Xt0[i - 1][j] + Xt0[i - 1][j - 1] + Xt0[i][j - 1];
    fx(i, j, sum);
    i = rows - 1;
    j = 0;
    sum = Xt0[i - 1][j + 1] + Xt0[i][j + 1] + Xt0[i - 1][j];
    fx(i, j, sum);
}

void GameOfLifeKernel::fx(const int i, const int j, const int sum) {
    if (Xt0[i][j] == 1 && sum < 2) {
        Xt1[i][j] = 0;
    } else if (Xt0[i][j] == 1 && sum >= 2 && sum <= 3) {
        Xt1[i][j] = 1;
    } else if (Xt0[i][j] == 1 && sum > 3) {
        Xt1[i][j] = 0;
    } else if (Xt0[i][j] == 0 && sum == 3) {
        Xt1[i][j] = 1;
    } else {
        Xt1[i][j] = 0;
    }
}


void GameOfLifeKernel::startThreads(void (GameOfLifeKernel::*fn)(int, int), GameOfLifeKernel *gameOfLifeKernel) {
    
    int t;
    int r1 = 1;
    int r2 = r1;
    
    for (t = 0; t < n_threads; t++) {
        r2 = (r1 + slice < rows-1) ? r1 + slice : rows-1;
        threads[t] = std::thread(fn, this, r1, r2);
        r1 += slice;
    }
    for (t = 0; t < n_threads; t++) {
        threads[t].join();
    }
    int remaining = ((rows-2) % n_threads);
    if (remaining > 0) {
        r1 = rows - 1 - remaining;
        r2 = rows - 1;
        threads[0] = std::thread(fn, this, r1, r2);
        if (threads[0].joinable()) {
            threads[0].join();
        }
    }
}

int GameOfLifeKernel::randMinMax(int min, int max) {
    static std::default_random_engine e{};
    static std::uniform_int_distribution<int> d{min, max};
    return d(e);
}

void GameOfLifeKernel::zeros(int **X, const int rows, const int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            X[i][j] = 0;
        }
    }
}


