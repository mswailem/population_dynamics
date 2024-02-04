#ifndef SIMULATIONWORKER_H
#define SIMULATIONWORKER_H

#include <QObject>
#include "LV.h" // Include your simulation header

class SimulationWorker : public QObject {
    Q_OBJECT

public:
    SimulationWorker(int lattice_size, int num_of_timesteps, double n0, double death_rate, double birth_rate, double pred_rate, int K);
    void runSimulation(); // Function to start the simulation

signals:
    void simulationComplete(const std::string& result); // Signal to emit when simulation is done

private:
    int lattice_size, num_of_timesteps, K;
    double n0, death_rate, birth_rate, pred_rate;
};

#endif // SIMULATIONWORKER_H

