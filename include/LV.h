#ifndef LV_H
#define LV_H

#include <string>
#include <vector>
#include <random>
#include <array>

class LatticeSimulator {
public:
    LatticeSimulator(int size, double death, double birth, double pred, int capacity);
    std::string run_simulation(int num_of_timesteps, double n0);

private:
    std::random_device rd{};
    std::mt19937 rng{rd()};
    std::uniform_real_distribution<double> dist{0.0, 1.0};

    int lattice_size, K;
    double death_rate, birth_rate, pred_rate;
    std::vector<std::vector<std::vector<int>>> Lattice;
    std::vector<int> N;

    void initialize_lattice(int N, int K);
    std::array<int, 2> pick_neighbor_random(int x, int y);
    std::array<int, 2> pick_neighbor_prey(int x, int y);
    std::array<int, 2> pick_site();
    void update_lattice();
};

std::string runLVSimulation(int lattice_size, int num_of_timesteps, double n0, double death_rate, double birth_rate, double pred_rate, int K);

#endif // LV_H

