#include <iostream>
#include <cmath>
#include <memory>
#include <vector>
#include <sstream>
#include "LV.h"

using namespace std;


void LatticeSimulator::initialize_lattice(int N, int K) {
	for (int s = 0; s < 2; s++) {
		for (int i = 0; i < N; i++) {
			int x = static_cast<int>(dist(rng) * lattice_size);
			int y = static_cast<int>(dist(rng) * lattice_size);
			if (Lattice[0][x][y] + Lattice[1][x][y] < K) {
				Lattice[s][x][y] += 1;
			} else {
				i--;
			}
		}
	}
}

//Pick a random neighbor of a site
array<int, 2> LatticeSimulator::pick_neighbor_random(int x, int y) {
	array<int, 2> site;
	int dice_dir = static_cast<int>(dist(rng) * 4);

	//This picks a random neighbor of a site and checks boudnaries based on the dice roll
	site[0] = (x + ((dice_dir == 1) - (dice_dir == 2)) + lattice_size) % lattice_size;
	site[1] = (y + ((dice_dir == 3) - (dice_dir == 0)) + lattice_size) % lattice_size;
	return site;
}

//Pick a neighbor of a site weighted by the number of prey
array<int, 2> LatticeSimulator::pick_neighbor_prey(int x, int y) {
	array<int, 2> site;

	//Define the neighbors
	int right = (x + 1) % lattice_size;
	int left = (x - 1 + lattice_size) % lattice_size;
	int up = (y + 1) % lattice_size;
	int down = (y - 1 + lattice_size) % lattice_size;

	//Calculate the total number of prey, and check if there are any
	int total_prey = Lattice[1][right][y] + Lattice[1][left][y] + Lattice[1][x][up] + Lattice[1][x][down];
	if (total_prey == 0) {
		return { -1, -1 };
	}

	//Pick a neighbor based on the number of prey
	int dice_dir = static_cast<int>(dist(rng) * total_prey);
	if (dice_dir < Lattice[1][right][y]) {
		return {right, y};
	} else if (dice_dir < Lattice[1][right][y] + Lattice[1][left][y]) {
		return {left, y};
	} else if (dice_dir < Lattice[1][right][y] + Lattice[1][left][y] + Lattice[1][x][up]) {
		return {x, up};
	} else {
		return {x, down};
	}
}

//Pick a site weighted by the propensities of the reactions at each site
array<int, 2> LatticeSimulator::pick_site() {
	static array<int, 2> site;
	double dice_site = dist(rng) * ((double)N[0] * (1 + death_rate + pred_rate) + (double)N[1] * (1 + birth_rate));

	//Finding the picked site
	double site_prob = 0;
	for (int x = 0; x < lattice_size; x++) {
		for (int y = 0; y < lattice_size; y++) {
			site_prob += (double)Lattice[0][x][y] * (1 + death_rate + pred_rate) + (double)Lattice[1][x][y] * (1 + birth_rate);
			if (dice_site <= site_prob) {
				site[0] = x;
				site[1] = y;
				return site;
			}
		}
	}
	return site; // Return last site if none picked (unlikely in proper use)
}

void LatticeSimulator::update_lattice() {
	int hits = N[0] + N[1];

	//Calculate the probabilities of each reaction
	double death_prob = death_rate / (1 + death_rate + pred_rate);
	double pred_prob = pred_rate / (1 + death_rate + pred_rate);
	double birth_prob = birth_rate / (1 + birth_rate);

	while (hits > 0) {

		//If there are no particles, return
		if (N[0] + N[1] == 0) {
			return;
		}

		auto site = pick_site();
		hits--;

		//Picking a reaction
		double dice_particle = dist(rng) * ((double)Lattice[0][site[0]][site[1]] * (1 + death_rate + pred_rate) + (double)Lattice[1][site[0]][site[1]] * (1 + birth_rate));

		//Predator
		if (dice_particle <= (double)Lattice[0][site[0]][site[1]] * (1 + death_rate + pred_rate)) {
			double dice_reaction = dist(rng);

			//Predation
			if (dice_reaction <= pred_prob) {
				auto neighbor = pick_neighbor_prey(site[0], site[1]);

				//If there are no prey, continue
				if (neighbor[0] == -1) continue;

				Lattice[1][neighbor[0]][neighbor[1]] -= 1;
				Lattice[0][neighbor[0]][neighbor[1]] += 1;
				N[0] += 1;
				N[1] -= 1;

				//Death
			} else if (dice_reaction <= pred_prob + death_prob) {
				Lattice[0][site[0]][site[1]] -= 1;
				N[0] -= 1;

				//Movement
			} else {
				auto neighbor = pick_neighbor_random(site[0], site[1]);
				if (Lattice[0][neighbor[0]][neighbor[1]] + Lattice[1][neighbor[0]][neighbor[1]] < K) {
					Lattice[0][neighbor[0]][neighbor[1]] += 1;
					Lattice[0][site[0]][site[1]] -= 1;
				}
			}

			//Prey
		} else {
			auto neighbor = pick_neighbor_random(site[0], site[1]);

			//Birth
			if (Lattice[1][neighbor[0]][neighbor[1]] + Lattice[0][neighbor[0]][neighbor[1]] < K) {
				double dice_reaction = dist(rng);
				if (dice_reaction < birth_prob) {
					Lattice[1][neighbor[0]][neighbor[1]] += 1;
					N[1] += 1;

					//Movement
				} else {
					Lattice[1][neighbor[0]][neighbor[1]] += 1;
					Lattice[1][site[0]][site[1]] -= 1;
				}
			}
		}
	}
}

LatticeSimulator::LatticeSimulator(int size, double death, double birth, double pred, int capacity)
	: lattice_size(size), death_rate(death), birth_rate(birth), pred_rate(pred), K(capacity), Lattice(2, vector<vector<int>>(size, vector<int>(size, 0))), N(2, 0) {}

	ostringstream oss; // String stream to build the output string

	string LatticeSimulator::run_simulation(int num_of_timesteps, double n0) {
		oss.str("");
		oss.clear();
		N[0] = N[1] = static_cast<int>(n0 * lattice_size * lattice_size);
		initialize_lattice(N[0], K);

		for (int t = 0; t < num_of_timesteps; t++) {
			oss << static_cast<double>(N[0]) / (lattice_size * lattice_size) << " "
				<< static_cast<double>(N[1]) / (lattice_size * lattice_size) << endl;
			LatticeSimulator::update_lattice();
		}

		return oss.str();

	}

string runLVSimulation(int lattice_size, int num_of_timesteps, double n0, double death_rate, double birth_rate, double pred_rate, int K) {
	LatticeSimulator simulator(lattice_size, death_rate, birth_rate, pred_rate, K);
	return simulator.run_simulation(num_of_timesteps, n0);
}
