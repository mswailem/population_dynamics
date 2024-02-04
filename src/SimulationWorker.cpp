#include "SimulationWorker.h"

SimulationWorker::SimulationWorker(int lattice_size, int num_of_timesteps, double n0, double death_rate, double birth_rate, double pred_rate, int K)
    : lattice_size(lattice_size), num_of_timesteps(num_of_timesteps), n0(n0),
      death_rate(death_rate), birth_rate(birth_rate), pred_rate(pred_rate), K(K) {}

void SimulationWorker::runSimulation() {
    std::string result = runLVSimulation(lattice_size, num_of_timesteps, n0, death_rate, birth_rate, pred_rate, K);
    emit simulationComplete(result);
}
