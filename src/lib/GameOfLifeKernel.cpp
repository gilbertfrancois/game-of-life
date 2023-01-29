//   Copyright 2023 Gilbert Francois Duivesteijn
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//
#include "GameOfLifeKernel.h"
#include <assert.h>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>

GameOfLifeKernel::GameOfLifeKernel(Config config_) : config(config_) {
    // Setup concurrency
    n_cpus = std::thread::hardware_concurrency();
    if (config.with_threads) {
        batch_ranges(config.rows, n_cpus);
    } else {
        batch_ranges(config.rows, 1);
    }
    threads = new std::thread[batches.size()];
    // Print info on the console.
    std::cout << "--- Availabe CPU cores: " << n_cpus << ", using "
              << batches.size() << " threads." << std::endl;
    std::cout << "--- Boundary type: " << config.boundary_type << std::endl;
    for (int t = 0; t < batches.size(); t++) {
        auto batch = batches.at(t);
        std::cout << "batch " << std::setw(2) << t << ":    " << std::setw(4)
                  << std::get<0>(batch) << " - " << std::setw(4)
                  << std::get<1>(batch) << std::endl;
    }
    // Alloc - init domain
    xt0 = new int *[config.rows];
    xt1 = new int *[config.rows];
    for (int i = 0; i < config.rows; i++) {
        xt0[i] = new int[config.cols];
        xt1[i] = new int[config.cols];
    }
    zeros(xt0);
    zeros(xt1);
    set_initial_conditions();

    // Set boundary condition function
    switch (config.boundary_type) {
    case BOUNDARY_CONSTANT:
        fpr_apply_boundary_conditions =
            &GameOfLifeKernel::apply_constant_boundary_conditions;
        break;
    case BOUNDARY_PERIODIC:
        fpr_apply_boundary_conditions =
            &GameOfLifeKernel::apply_periodic_boundary_conditions;
        break;
    case BOUNDARY_MIRROR:
        fpr_apply_boundary_conditions =
            &GameOfLifeKernel::apply_mirror_boundary_conditions;
        break;
    default:
        fpr_apply_boundary_conditions =
            &GameOfLifeKernel::apply_periodic_boundary_conditions;
    }
}

GameOfLifeKernel::~GameOfLifeKernel() {
    for (int i = 0; i < config.rows; i++) {
        delete[] xt0[i];
        delete[] xt1[i];
    }
    delete[] xt0;
    delete[] xt1;
    delete[] threads;
}

void GameOfLifeKernel::timestep() {
    // compute inner domain
    if (config.with_threads) {
        start_threads(&GameOfLifeKernel::timestep_subdomain, this);
    } else {
        start_no_threads(&GameOfLifeKernel::timestep_subdomain, this);
    }
    // compute boundaries
    (this->*fpr_apply_boundary_conditions)();
    // swap buffers
    int **tmp = xt0;
    xt0 = xt1;
    xt1 = tmp;
    zeros(xt1);
}

int GameOfLifeKernel::get_n_threads() { return batches.size(); }

int GameOfLifeKernel::get_n_cpus() { return n_cpus; }

int **GameOfLifeKernel::get_xt() const { return xt0; }

const int GameOfLifeKernel::get_xt_at(int row, int col) {
    return xt0[row][col];
}

std::string GameOfLifeKernel::to_string() {
    std::stringstream ss;
    for (int i = 0; i < config.rows; i++) {
        for (int j = 0; j < config.cols; j++) {
            ss << ((xt0[i][j] == 1) ? CELL_ALIVE : CELL_DEAD);
        }
        ss << std::endl;
    }
    return ss.str();
}

void GameOfLifeKernel::set_initial_conditions() {
    set_initial_conditions_in_subdomain(0, config.rows);
}

void GameOfLifeKernel::set_initial_conditions_in_subdomain(const int min_row,
                                                           const int max_row) {
    int sum = 0;
    // Will be used to obtain a seed for the random number engine
    std::random_device rd;
    // Standard mersenne_twister_engine seeded with rd()
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribution(0, 1);
    for (int i = min_row; i < max_row; i++) {
        for (int j = 0; j < config.cols; j++) {
            xt0[i][j] = distribution(gen);
            sum += xt0[i][j];
        }
    }
    float fraction = (float)sum / (config.rows * config.cols);
    std::cout << "Initial distribution: " << fraction << std::endl;
}

void GameOfLifeKernel::timestep_subdomain(const int min_row,
                                          const int max_row) {
    // Loop over inner domain
    int min_col = 0;
    int max_col = config.cols - 1;
    for (int i = min_row; i < max_row; i++) {
        if (i == 0 || i >= config.rows - 1)
            continue;
        for (int j = min_col; j < max_col; j++) {
            if (j == 0 || j >= config.cols - 1)
                continue;
            int sum = xt0[i - 1][j - 1] + xt0[i - 1][j] + xt0[i - 1][j + 1] +
                      xt0[i][j - 1] + xt0[i][j + 1] + xt0[i + 1][j - 1] +
                      xt0[i + 1][j] + xt0[i + 1][j + 1];
            fx(i, j, sum);
        }
    }
}

void GameOfLifeKernel::apply_constant_boundary_conditions() {
    int i, j, sum = 0;
    // compute edges
    for (i = 1; i < config.rows - 1; i++) {
        j = 0;
        sum = xt0[i - 1][j] + xt0[i - 1][j + 1] + xt0[i][j + 1] +
              xt0[i + 1][j] + xt0[i + 1][j + 1];
        fx(i, j, sum);
        j = config.cols - 1;
        sum = xt0[i - 1][j - 1] + xt0[i - 1][j] + xt0[i][j - 1] +
              xt0[i + 1][j - 1] + xt0[i + 1][j];
        fx(i, j, sum);
    }
    for (j = 1; j < config.cols - 1; j++) {
        i = 0;
        sum = xt0[i][j - 1] + xt0[i][j + 1] + xt0[i + 1][j - 1] +
              xt0[i + 1][j] + xt0[i + 1][j + 1];
        fx(i, j, sum);
        i = config.rows - 1;
        sum = xt0[i - 1][j - 1] + xt0[i - 1][j] + xt0[i - 1][j + 1] +
              xt0[i][j - 1] + xt0[i][j + 1];

        fx(i, j, sum);
    }
    // compute corners
    i = 0;
    j = 0;
    sum = xt0[i][j + 1] + xt0[i + 1][j] + xt0[i + 1][j + 1];
    fx(i, j, sum);
    i = 0;
    j = config.cols - 1;
    sum = xt0[i][j - 1] + xt0[i + 1][j - 1] + xt0[i + 1][j];
    fx(i, j, sum);
    i = config.rows - 1;
    j = 0;
    sum = xt0[i - 1][j] + xt0[i - 1][j + 1] + xt0[i][j + 1];
    fx(i, j, sum);
    i = config.rows - 1;
    j = config.cols - 1;
    sum = xt0[i - 1][j - 1] + xt0[i - 1][j] + xt0[i][j - 1];
    fx(i, j, sum);
}

void GameOfLifeKernel::apply_periodic_boundary_conditions() {
    int i, j, sum = 0;
    // compute edges
    for (i = 1; i < config.rows - 1; i++) {
        j = 0;
        sum = xt0[i - 1][config.cols - 1] + xt0[i - 1][j] + xt0[i - 1][j + 1] +
              xt0[i][config.cols - 1] + xt0[i][j + 1] +
              xt0[i + 1][config.cols - 1] + xt0[i + 1][j] + xt0[i + 1][j + 1];
        fx(i, j, sum);
        j = config.cols - 1;
        sum = xt0[i - 1][j - 1] + xt0[i - 1][j] + xt0[i - 1][0] +
              xt0[i][j - 1] + xt0[i][0] + xt0[i + 1][j - 1] + xt0[i + 1][j] +
              xt0[i + 1][0];
        fx(i, j, sum);
    }
    for (j = 1; j < config.cols - 1; j++) {
        i = 0;
        sum = xt0[config.rows - 1][j - 1] + xt0[config.rows - 1][j] +
              xt0[config.rows - 1][j + 1] + xt0[i][j - 1] + xt0[i][j + 1] +
              xt0[i + 1][j - 1] + xt0[i + 1][j] + xt0[i + 1][j + 1];
        fx(i, j, sum);
        i = config.rows - 1;
        sum = xt0[i - 1][j - 1] + xt0[i - 1][j] + xt0[i - 1][j + 1] +
              xt0[i][j - 1] + xt0[i][j + 1] + xt0[0][j - 1] + xt0[0][j] +
              xt0[0][j + 1];
        fx(i, j, sum);
    }
    // compute corners
    i = 0;
    j = 0;
    sum = xt0[config.rows - 1][config.cols - 1] + xt0[config.rows - 1][j] +
          xt0[config.rows - 1][j + 1] + xt0[i][config.cols - 1] +
          xt0[i][j + 1] + xt0[i + 1][config.cols - 1] + xt0[i + 1][j] +
          xt0[i + 1][j + 1];
    fx(i, j, sum);
    i = 0;
    j = config.cols - 1;
    sum = xt0[config.rows - 1][j - 1] + xt0[config.rows - 1][j] +
          xt0[config.rows - 1][0] + xt0[i][j - 1] + xt0[i][0] +
          xt0[i + 1][j - 1] + xt0[i + 1][j] + xt0[i + 1][0];
    fx(i, j, sum);
    i = config.rows - 1;
    j = 0;
    sum = xt0[i - 1][config.cols - 1] + xt0[i - 1][j] + xt0[i - 1][j + 1] +
          xt0[i][config.cols - 1] + xt0[i][j + 1] + xt0[0][config.cols - 1] +
          xt0[0][j] + xt0[0][j + 1];
    fx(i, j, sum);
    i = config.rows - 1;
    j = config.cols - 1;
    sum = xt0[i - 1][j - 1] + xt0[i - 1][j] + xt0[i - 1][0] + xt0[i][j - 1] +
          xt0[i][0] + xt0[0][j - 1] + xt0[0][j] + xt0[0][0];
    fx(i, j, sum);
}

void GameOfLifeKernel::apply_mirror_boundary_conditions() {
    int i, j, sum = 0;
    // compute edges
    for (i = 1; i < config.rows - 1; i++) {
        j = 0;
        sum = xt0[i - 1][j + 1] + xt0[i - 1][j] + xt0[i - 1][j + 1] +
              xt0[i][j + 1] + xt0[i][j + 1] + xt0[i + 1][j + 1] +
              xt0[i + 1][j] + xt0[i + 1][j + 1];
        fx(i, j, sum);
        j = config.cols - 1;
        sum = xt0[i - 1][j - 1] + xt0[i - 1][j] + xt0[i - 1][j - 1] +
              xt0[i][j - 1] + xt0[i][j - 1] + xt0[i + 1][j - 1] +
              xt0[i + 1][j] + xt0[i + 1][j - 1];
        fx(i, j, sum);
    }
    for (j = 1; j < config.cols - 1; j++) {
        i = 0;
        sum = xt0[i + 1][j - 1] + xt0[i + 1][j] + xt0[i + 1][j + 1] +
              xt0[i][j - 1] + xt0[i][j + 1] + xt0[i + 1][j - 1] +
              xt0[i + 1][j] + xt0[i + 1][j + 1];
        fx(i, j, sum);
        i = config.rows - 1;
        sum = xt0[i - 1][j - 1] + xt0[i - 1][j] + xt0[i - 1][j + 1] +
              xt0[i][j - 1] + xt0[i][j + 1] + xt0[i - 1][j - 1] +
              xt0[i - 1][j] + xt0[i - 1][j + 1];
        fx(i, j, sum);
    }
    // compute corners
    i = 0;
    j = 0;
    sum = xt0[i + 1][j] + xt0[i + 1][j + 1] + xt0[i][j + 1] + xt0[i][j + 1] +
          xt0[i + 1][j + 1] + xt0[i + 1][j] + xt0[i + 1][j + 1];
    fx(i, j, sum);
    i = 0;
    j = config.cols - 1;
    sum = xt0[i + 1][j - 1] + xt0[i + 1][j] + xt0[i][j - 1] + xt0[i][j - 1] +
          xt0[i + 1][j - 1] + xt0[i + 1][j] + xt0[i + 1][j - 1];
    fx(i, j, sum);
    i = config.rows - 1;
    j = 0;
    sum = xt0[i - 1][j + 1] + xt0[i - 1][j] + xt0[i - 1][j + 1] +
          xt0[i][j + 1] + xt0[i][j + 1] + +xt0[i - 1][j] + xt0[i - 1][j + 1];
    fx(i, j, sum);
    i = config.rows - 1;
    j = config.cols - 1;
    sum = xt0[i - 1][j - 1] + xt0[i - 1][j] + xt0[i - 1][j - 1] +
          xt0[i][j - 1] + xt0[i][j - 1] + xt0[i - 1][j - 1] + xt0[i - 1][j];
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

void GameOfLifeKernel::zeros(int **X) {
    for (int i = 0; i < config.rows; i++) {
        for (int j = 0; j < config.cols; j++) {
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
