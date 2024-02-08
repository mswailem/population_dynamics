#ifndef RPS_H
#define RPS_H

#include <vector>
#include <random>
#include <array>
#include <SFML/Graphics.hpp>

class LatticeSimulator {
public:
    LatticeSimulator();
    void run_simulation(int size,double pred, double n0, bool frame_by_frame = false, bool exchange_on = false);
    void run_simulation_save(int size,double pred, double n0,int num_of_timesteps, bool frame_by_frame = false, bool exchange_on = false);

private:
    std::random_device rd{};
    std::mt19937 rng{rd()};
    std::uniform_real_distribution<double> dist{0.0, 1.0};

    int lattice_size;
    double pred_prob;
    std::vector<std::vector<std::vector<int>>> Lattice;
    std::vector<int> N;

    void initialize_lattice_hopping();
    void initialize_lattice_exchange(int n0);
    std::array<int, 2> pick_neighbor(int x, int y);
	std::array<int, 2> pick_site();
    void update_lattice(bool exchange_on);
	void draw_lattice(sf::RenderWindow& window, int window_size_x, int window_size_y, double n0);
	void save_lattice(int window_size_x, int window_size_y, double n0, std::string filename);
};


#endif // RPS_H

