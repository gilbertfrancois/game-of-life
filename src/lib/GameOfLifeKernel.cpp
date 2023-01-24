//
// Created by Gilbert Francois on 05-09-16.
//

#include "GameOfLifeKernel.h"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>

GameOfLifeKernel::GameOfLifeKernel(int rows, int cols, bool with_threads)
    : rows(rows), cols(cols) {
    // Setup concurrency
    n_cpus = std::thread::hardware_concurrency();
    if (with_threads) {
        batch_ranges(rows, n_cpus);
    } else {
        batch_ranges(rows, 1);
    }
    threads = new std::thread[batches.size()];
    std::cout << "--- Availabe CPU cores: " << n_cpus << ", using " << batches.size()
              << " threads." << std::endl;
    for (int t = 0; t < batches.size(); t++) {
        auto batch = batches.at(t);
        std::cout << "Slices:    " << std::setw(4) << std::get<0>(batch) << " - "
                  << std::setw(4) << std::get<1>(batch) << std::endl;
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
    start_threads(&GameOfLifeKernel::timestep_inner_subdomain, this);
    // compute boundaries
    timestep_boundaries();
    // swap buffers
    int **tmp = xt0;
    xt0 = xt1;
    xt1 = tmp;
}

const int GameOfLifeKernel::get_xt_at(int row, int col) {
    return row > 0 && row < rows && col > 0 && col < cols ? xt0[row][col] : -1;
}

int **GameOfLifeKernel::get_xt() const { return xt0; }

void GameOfLifeKernel::set_initial_conditions() {
    /* start_threads(&GameOfLifeKernel::set_initial_conditions_in_subdomain, this); */
    set_initial_conditions_in_subdomain(0, rows);
}

void GameOfLifeKernel::set_initial_conditions_in_subdomain(const int min_row,
                                                           const int max_row) {
    for (int i = min_row; i < max_row; i++) {
        for (int j = 0; j < cols; j++) {
            xt0[i][j] = rand_minmax(0, 1);
        }
    }
}

void GameOfLifeKernel::timestep_inner_subdomain(const int min_row,
                                                const int max_row) {
    // Loop over inner domain
    for (int i = min_row; i < max_row; i++) {
        for (int j = 1; j < cols - 1; j++) {
            int sum = xt0[i - 1][j + 1] + xt0[i][j + 1] + xt0[i + 1][j + 1] +
                      xt0[i - 1][j] + xt0[i + 1][j] + xt0[i - 1][j - 1] +
                      xt0[i][j - 1] + xt0[i + 1][j - 1];
            fx(i, j, sum);
        }
    }
}

void GameOfLifeKernel::timestep_boundaries() {
    int i, j, sum;
    // compute edges
    for (i = 1; i < rows - 1; i++) {
        j = 0;
        sum = xt0[i - 1][j + 1] + xt0[i][j + 1] + xt0[i + 1][j + 1] +
              xt0[i - 1][0] + xt0[i + 1][0];
        fx(i, j, sum);
        j = cols - 1;
        sum = xt0[i - 1][j] + xt0[i + 1][j] + xt0[i - 1][j - 1] +
              xt0[i][j - 1] + xt0[i + 1][j - 1];
        fx(i, j, sum);
    }
    for (j = 1; j < cols - 1; j++) {
        i = 0;
        sum = xt0[i][j + 1] + xt0[i + 1][j + 1] + xt0[i + 1][j] +
              xt0[i][j - 1] + xt0[i + 1][j - 1];
        fx(i, j, sum);
        i = rows - 1;
        sum = xt0[i - 1][j + 1] + xt0[i][j + 1] + xt0[i - 1][j] +
              xt0[i - 1][j - 1] + xt0[i][j - 1];
        fx(i, j, sum);
    }
    // compute corners
    i = 0;
    j = 0;
    sum = xt0[i][j + 1] + xt0[i + 1][j + 1] + xt0[i + 1][j];
    fx(i, j, sum);
    i = 0;
    j = cols - 1;
    sum = xt0[i][j - 1] + xt0[i + 1][j - 1] + xt0[i + 1][j];
    fx(i, j, sum);
    i = rows - 1;
    j = cols - 1;
    sum = xt0[i - 1][j] + xt0[i - 1][j - 1] + xt0[i][j - 1];
    fx(i, j, sum);
    i = rows - 1;
    j = 0;
    sum = xt0[i - 1][j + 1] + xt0[i][j + 1] + xt0[i - 1][j];
    fx(i, j, sum);
}

void GameOfLifeKernel::fx(const int i, const int j, const int sum) {
    if (xt0[i][j] == 1 && sum < 2) {
        xt1[i][j] = 0;
    } else if (xt0[i][j] == 1 && sum >= 2 && sum <= 3) {
        xt1[i][j] = 1;
    } else if (xt0[i][j] == 1 && sum > 3) {
        xt1[i][j] = 0;
    } else if (xt0[i][j] == 0 && sum == 3) {
        xt1[i][j] = 1;
    } else {
        xt1[i][j] = 0;
    }
}

void GameOfLifeKernel::start_threads(void (GameOfLifeKernel::*fn)(int, int),
                                     GameOfLifeKernel *gameOfLifeKernel) {

    for (int i=0; i<batches.size(); i++) {
        int r1 = std::get<0>(batches[i]);
        int r2 = std::get<0>(batches[i]);
        threads[i] = std::thread(fn, this, r1, r2);
    }
    for (int i = 0; i < n_threads; i++) {
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
    int batch_size = n_samples / n_batches;
    for (int i=0; i<n_batches; i++) {
        batches.push_back(std::tuple<int, int>{i*batch_size, (i+1)*batch_size});
    }
    if (batch_size * n_batches - n_samples != 0) {
        batches.push_back(std::tuple<int, int>{(batch_size * n_batches), n_samples});
    }
}
