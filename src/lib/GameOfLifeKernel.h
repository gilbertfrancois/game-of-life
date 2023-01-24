//
// Created by Gilbert Francois on 05-09-16.
//

#ifndef GAMEOFLIFE_GAMEOFLIFEKERNEL_H
#define GAMEOFLIFE_GAMEOFLIFEKERNEL_H

#include <string>
#include <vector>
#include <tuple>
#include <thread>

#define CELL_ALIVE "O"
#define CELL_DEAD " "

class GameOfLifeKernel {
  public:
    GameOfLifeKernel(int rows, int cols, bool with_threads);

    virtual ~GameOfLifeKernel();

    int get_n_threads();

    int get_n_cpus();

    std::string to_string();

    void timestep();

    const int get_xt_at(int row, int col);

    int **get_xt() const;

  private:
    const int rows;
    const int cols;
    std::thread *threads;
    int **xt0;
    int **xt1;
    int n_cpus;

    std::vector<std::tuple<int, int>> batches;

    void set_initial_conditions();

    void set_initial_conditions_in_subdomain(const int min_row,
                                             const int max_row);

    void timestep_inner_subdomain(const int min_row, const int max_row);

    void timestep_boundaries();

    void fx(const int i, const int j, const int sum);

    void start_threads(void (GameOfLifeKernel::*fn)(int, int),
                       GameOfLifeKernel *kernel);

    int rand_minmax(int min, int max);

    void zeros(int **X, const int rows, const int cols);

    void batch_ranges(int n_samples, int n_batches);
};

#endif // GAMEOFLIFE_GAMEOFLIFEKERNEL_H
