//
// Created by Gilbert Francois on 05-09-16.
//

#include "GameOfLifeKernel.h"
#include <assert.h>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>

GameOfLifeKernel::GameOfLifeKernel(int rows, int cols, bool with_threads)
    : rows(rows), cols(cols), with_threads(with_threads) {
    // Setup concurrency
    n_cpus = std::thread::hardware_concurrency();
    if (with_threads) {
        batch_ranges(rows, n_cpus);
    } else {
        batch_ranges(rows, 1);
    }
    threads = new std::thread[batches.size()];
    std::cout << "--- Availabe CPU cores: " << n_cpus << ", using "
              << batches.size() << " threads." << std::endl;
    for (int t = 0; t < batches.size(); t++) {
        auto batch = batches.at(t);
        std::cout << "batch " << std::setw(2) << t << ":    " << std::setw(4)
                  << std::get<0>(batch) << " - " << std::setw(4)
                  << std::get<1>(batch) << std::endl;
    }
    // Alloc - init domain
    xt0 = new int *[rows];
    xt1 = new int *[rows];
    for (int i = 0; i < rows; i++) {
        xt0[i] = new int[cols];
        xt1[i] = new int[cols];
    }
    zeros(xt0, rows, cols);
    zeros(xt1, rows, cols);
    set_initial_conditions();
}

GameOfLifeKernel::~GameOfLifeKernel() {
    for (int i = 0; i < rows; i++) {
        delete[] xt0[i];
        delete[] xt1[i];
    }
    delete[] xt0;
    delete[] xt1;
    delete[] threads;
}

int GameOfLifeKernel::get_n_cpus() { return n_cpus; }

int GameOfLifeKernel::get_n_threads() { return batches.size(); }

std::string GameOfLifeKernel::to_string() {
    std::stringstream ss;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            ss << ((xt0[i][j] == 1) ? CELL_ALIVE : CELL_DEAD);
        }
        ss << std::endl;
    }
    return ss.str();
}

void GameOfLifeKernel::timestep() {
    // compute inner domain
    if (with_threads) {
        start_threads(&GameOfLifeKernel::timestep_inner_subdomain, this);
    } else {
        start_no_threads(&GameOfLifeKernel::timestep_inner_subdomain, this);
    }
    // compute boundaries
    timestep_boundaries_circular();
    // swap buffers
    int **tmp = xt0;
    xt0 = xt1;
    xt1 = tmp;
}

const int GameOfLifeKernel::get_xt_at(int row, int col) {
    return xt0[row][col];
}

int **GameOfLifeKernel::get_xt() const { return xt0; }

void GameOfLifeKernel::set_initial_conditions() {
    set_initial_conditions_in_subdomain(0, rows);
}

void GameOfLifeKernel::set_initial_conditions_in_subdomain(const int min_row,
                                                           const int max_row) {
    int sum = 0;
    for (int i = min_row; i < max_row; i++) {
        for (int j = 0; j < cols; j++) {
            xt0[i][j] = rand_minmax(0, 1);
            sum += xt0[i][j];
        }
    }
    float fraction = (float) sum / (rows*cols);
    std::cout << "Initial distribution: " << fraction << std::endl;
}

void GameOfLifeKernel::timestep_inner_subdomain(const int min_row,
                                                const int max_row) {
    // Loop over inner domain
    int min_col = 0;
    int max_col = cols - 1;
    for (int i = min_row; i < max_row; i++) {
        if (i == 0 || i >= rows - 1)
            continue;
        for (int j = min_col; j < max_col; j++) {
            if (j == 0 || j >= cols - 1)
                continue;
            int sum = xt0[i-1][j-1] + xt0[i-1][j] + xt0[i-1][j+1] + 
                      xt0[i  ][j-1] +               xt0[i  ][j+1] +
                      xt0[i+1][j-1] + xt0[i+1][j] + xt0[i+1][j+1];
            fx(i, j, sum);
        }
    }
}
void GameOfLifeKernel::timestep_boundaries_circular() {
    int i, j, sum = 0;
    // compute edges
    for (i = 1; i < rows - 1; i++) {
        j = 0;
        sum = xt0[i-1][cols-1] + xt0[i-1][j] + xt0[i-1][j+1] + 
              xt0[i  ][cols-1] +               xt0[i  ][j+1] +
              xt0[i+1][cols-1] + xt0[i+1][j] + xt0[i+1][j+1];
        fx(i, j, sum);
        j = cols - 1;
        sum = xt0[i-1][j-1] + xt0[i-1][j] + xt0[i-1][0] + 
              xt0[i  ][j-1] +               xt0[i  ][0] +
              xt0[i+1][j-1] + xt0[i+1][j] + xt0[i+1][0];
        fx(i, j, sum);
    }
    for (j = 1; j < cols - 1; j++) {
        i = 0;
        sum = xt0[rows-1][j-1] + xt0[rows-1][j] + xt0[rows-1][j+1] + 
              xt0[i  ][j-1] +               xt0[i  ][j+1] +
              xt0[i+1][j-1] + xt0[i+1][j] + xt0[i+1][j+1];
        fx(i, j, sum);
        i = rows - 1;
        sum = xt0[i-1][j-1] + xt0[i-1][j] + xt0[i-1][j+1] + 
              xt0[i  ][j-1] +               xt0[i  ][j+1] +
              xt0[0][j-1] + xt0[0][j] + xt0[0][j+1];
        fx(i, j, sum);
    }
    // compute corners
    i = 0;
    j = 0;
    sum = xt0[rows-1][cols-1] + xt0[rows-1][j] + xt0[rows-1][j+1] + 
          xt0[i  ][cols-1] +               xt0[i  ][j+1] +
          xt0[i+1][cols-1] + xt0[i+1][j] + xt0[i+1][j+1];
    fx(i, j, sum);
    i = 0;
    j = cols - 1;
    sum = xt0[rows-1][j-1] + xt0[rows-1][j] + xt0[rows-1][0] + 
          xt0[i  ][j-1] +               xt0[i  ][0] +
          xt0[i+1][j-1] + xt0[i+1][j] + xt0[i+1][0];
    fx(i, j, sum);
    i = rows - 1;
    j = 0;
    sum = xt0[i-1][cols-1] + xt0[i-1][j] + xt0[i-1][j+1] + 
          xt0[i  ][cols-1] +               xt0[i  ][j+1] +
          xt0[0][cols-1] + xt0[0][j] + xt0[0][j+1];
    fx(i, j, sum);
    i = rows - 1;
    j = cols - 1;
    sum = xt0[i-1][j-1] + xt0[i-1][j] + xt0[i-1][0] + 
          xt0[i  ][j-1] +               xt0[i  ][0] +
          xt0[0][j-1] + xt0[0][j] + xt0[0][0];
    fx(i, j, sum);
}

void GameOfLifeKernel::timestep_boundaries() {
    int i, j, sum = 0;
    // compute edges
    for (i = 1; i < rows - 1; i++) {
        j = 0;
        sum = xt0[i-1][j] + xt0[i-1][j+1] + 
                            xt0[i  ][j+1] +
              xt0[i+1][j] + xt0[i+1][j+1];
        fx(i, j, sum);
        j = cols - 1;
        sum = xt0[i-1][j-1] + xt0[i-1][j] + 
              xt0[i  ][j-1] +               
              xt0[i+1][j-1] + xt0[i+1][j];
        fx(i, j, sum);
    }
    for (j = 1; j < cols - 1; j++) {
        i = 0;
        sum = xt0[i  ][j-1] +               xt0[i  ][j+1] +
              xt0[i+1][j-1] + xt0[i+1][j] + xt0[i+1][j+1];
        fx(i, j, sum);
        i = rows - 1;
        sum = xt0[i-1][j-1] + xt0[i-1][j] + xt0[i-1][j+1] + 
              xt0[i  ][j-1] +               xt0[i  ][j+1];
                  
        fx(i, j, sum);
    }
    // compute corners
    i = 0;
    j = 0;
    sum =               xt0[i  ][j+1] +
          xt0[i+1][j] + xt0[i+1][j+1];
    fx(i, j, sum);
    i = 0;
    j = cols - 1;
    sum = xt0[i  ][j-1] + 
          xt0[i+1][j-1] + xt0[i+1][j];
    fx(i, j, sum);
    i = rows - 1;
    j = 0;
    sum = xt0[i-1][j] + xt0[i-1][j+1] + 
                        xt0[i  ][j+1];
    fx(i, j, sum);
    i = rows - 1;
    j = cols - 1;
    sum = xt0[i-1][j-1] + xt0[i-1][j] +
          xt0[i  ][j-1];             
    fx(i, j, sum);
}

void GameOfLifeKernel::fx(const int i, const int j, const int sum) {
    int value = xt0[i][j];
    int new_value = -1;
    if (value == 0) {
        if (sum == 3)
            new_value = 1;
        else
            new_value = 0;
    }
    if (value == 1) {
        if (sum < 2)
            new_value = 0;
        else if (sum >= 2 && sum <= 3)
            new_value = 1;
        else if (sum > 3)
            new_value = 0;
    }
    assert(new_value != -1);
    xt1[i][j] = new_value;
}

void GameOfLifeKernel::start_no_threads(void (GameOfLifeKernel::*fn)(int, int),
                                        GameOfLifeKernel *gameOfLifeKernel) {

    std::tuple<int, int> batch = batches.at(0);
    (gameOfLifeKernel->*fn)(std::get<0>(batch), std::get<1>(batch));
}

void GameOfLifeKernel::start_threads(void (GameOfLifeKernel::*fn)(int, int),
                                     GameOfLifeKernel *gameOfLifeKernel) {

    for (int i = 0; i < batches.size(); i++) {
        int r1 = std::get<0>(batches[i]);
        int r2 = std::get<1>(batches[i]);
        threads[i] = std::thread(fn, this, r1, r2);
    }
    for (int i = 0; i < batches.size(); i++) {
        threads[i].join();
    }
}

int GameOfLifeKernel::rand_minmax(int min, int max) {
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

void GameOfLifeKernel::batch_ranges(int n_samples, int n_batches) {
    // Don't try to make more batches than the total number of samples.
    if (n_batches >= n_samples - 2) {
        n_batches = n_samples - 2;
    }
    int batch_size = n_samples / n_batches;
    for (int i = 0; i < n_batches; i++) {
        batches.push_back(
            std::tuple<int, int>{i * batch_size, (i + 1) * batch_size});
    }
    if (batch_size * n_batches - n_samples != 0) {
        batches.push_back(
            std::tuple<int, int>{(batch_size * n_batches), n_samples});
    }
}
