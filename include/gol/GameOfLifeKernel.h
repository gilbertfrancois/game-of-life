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
#ifndef GAMEOFLIFE_GAMEOFLIFEKERNEL_H
#define GAMEOFLIFE_GAMEOFLIFEKERNEL_H

#include "config.h"
#include <string>
#include <thread>
#include <tuple>
#include <vector>

#define CELL_ALIVE "O"
#define CELL_DEAD " "

enum BOUNDARY_TYPES {
    BOUNDARY_CONSTANT = 0,
    BOUNDARY_PERIODIC = 1,
    BOUNDARY_MIRROR = 2
};

class GameOfLifeKernel {
  public:
    GameOfLifeKernel(Config config);

    virtual ~GameOfLifeKernel();

    void timestep();

    int get_n_threads();

    int get_n_cpus();

    int **get_xt() const;

    const int get_xt_at(int row, int col);

    std::string to_string();

  private:
    Config config;
    /* int rows; */
    /* int cols; */
    std::thread *threads;
    int **xt0;
    int **xt1;
    int n_cpus;
    void (GameOfLifeKernel::*fpr_apply_boundary_conditions)();

    std::vector<std::tuple<int, int>> batches;

    void set_initial_conditions();

    void set_initial_conditions_in_subdomain(const int min_row,
                                             const int max_row);

    void timestep_subdomain(const int min_row, const int max_row);

    void apply_constant_boundary_conditions();

    void apply_periodic_boundary_conditions();

    void apply_mirror_boundary_conditions();

    void fx(const int i, const int j, const int sum);

    void start_no_threads(void (GameOfLifeKernel::*fn)(int, int),
                          GameOfLifeKernel *kernel);

    void start_threads(void (GameOfLifeKernel::*fn)(int, int),
                       GameOfLifeKernel *kernel);

    void zeros(int **X);

    void batch_ranges(int n_samples, int n_batches);
};

#endif