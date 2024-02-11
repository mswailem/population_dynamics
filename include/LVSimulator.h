#ifndef LV_H
#define LV_H

#include <vector>
#include <random>
#include <array>
#include <SFML/Graphics.hpp>
#include <QObject>

class LatticeSimulator : public QObject {
	Q_OBJECT
public:
    LatticeSimulator();
	void run_simulation(int size, double death, double birth, double pred, double n0);
	void run_simulation_SFML(int size, double death, double birth, double pred, double n0, bool frame_by_frame = false);
	void StopSimulation(); //This stops the simulation if the user clicks the stop simulation button

signals:
    void time_step_complete(const std::string& result); // Signal to emit when a time step is complete													
	void simulation_complete(); // Signal to emit when the simulation is complete

private:
    std::random_device rd{};
    std::mt19937 rng{rd()};
    std::uniform_real_distribution<double> dist{0.0, 1.0};

    int lattice_size;
    double death_rate, birth_rate, pred_rate;
    std::vector<std::vector<std::vector<int>>> Lattice;
    std::vector<int> N;
	double pred_prob, death_prob, birth_prob;
	bool stop_simulation;

    void initialize_lattice(int N);
    std::array<int, 2> pick_neighbor_random(int x, int y);
    std::array<int, 2> pick_neighbor_prey(int x, int y);
    void update_lattice();
	void draw_lattice(sf::RenderWindow& window, int window_size_x, int window_size_y);
};


#endif // LV_H

