#include <SFML/Graphics.hpp>
#include <vector>
#include <sstream>
#include "LVSimulator.h"


void LatticeSimulator::initialize_lattice(int N) {
	for (int s = 0; s < 2; s++) {
		for (int i = 0; i < N; i++) {
			int x = static_cast<int>(dist(rng) * lattice_size);
			int y = static_cast<int>(dist(rng) * lattice_size);
			if (Lattice[0][x][y] + Lattice[1][x][y] < 1) {
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

void LatticeSimulator::draw_lattice(sf::RenderWindow& window, int window_size_x, int window_size_y) {
	sf::RectangleShape rectangle(sf::Vector2f(window_size_x / lattice_size, window_size_y / lattice_size));
	for (int i = 0; i < lattice_size; i++) {
		for (int j = 0; j < lattice_size; j++) {
			rectangle.setPosition(i * window_size_x / lattice_size, j * window_size_y / lattice_size);
			if (Lattice[0][i][j] > 0) {
				rectangle.setFillColor(sf::Color::Red);
			} else if (Lattice[1][i][j] > 0) {
				rectangle.setFillColor(sf::Color::Blue);
			} else {
				rectangle.setFillColor(sf::Color::Black);
			}
			window.draw(rectangle);
		}
	}
	window.display();
}

void LatticeSimulator::update_lattice() {
	int hits = N[0] + N[1];

	while (hits > 0) {

		//If there are no particles, return
		if (N[0] + N[1] == 0) {
			return;
		}

		//Picking a species
		int s;
		double dice_particle = dist(rng) * (static_cast<double>(N[0]) * (1 + death_rate + pred_rate) + static_cast<double>(N[1]) * (1 + birth_rate));
		s = (dice_particle < static_cast<double>(N[0]) * (1 + death_rate + pred_rate)) ? 0 : 1; // 0 is predator, 1 is prey

		//Picking a site by randomly picking sites until one is found with the species picked to react
		double site_found = 0;
		int x;
		int y;
		while (site_found == 0) {
			int dice_site = static_cast<int>(dist(rng) * lattice_size * lattice_size);
			x = dice_site / lattice_size;
			y = dice_site % lattice_size;
			site_found = Lattice[s][x][y];
		}

		hits--;


		//Predator
		if (s == 0) {
			double dice_reaction = dist(rng);

			//Predation
			if (dice_reaction <= pred_prob) {
				auto neighbor = pick_neighbor_prey(x, y);

				//If there are no prey, continue
				if (neighbor[0] == -1) continue;

				Lattice[1][neighbor[0]][neighbor[1]] -= 1;
				Lattice[0][neighbor[0]][neighbor[1]] += 1;
				N[0] += 1;
				N[1] -= 1;

				//Death
			} else if (dice_reaction <= pred_prob + death_prob) {
				Lattice[0][x][y] -= 1;
				N[0] -= 1;

				//Movement
			} else {
				auto neighbor = pick_neighbor_random(x, y);
				if (Lattice[0][neighbor[0]][neighbor[1]] + Lattice[1][neighbor[0]][neighbor[1]] < 1) {
					Lattice[0][neighbor[0]][neighbor[1]] += 1;
					Lattice[0][x][y] -= 1;
				}
			}

		//Prey
		} else {
			auto neighbor = pick_neighbor_random(x, y);

			//Birth
			if (Lattice[1][neighbor[0]][neighbor[1]] + Lattice[0][neighbor[0]][neighbor[1]] < 1) {
				double dice_reaction = dist(rng);
				if (dice_reaction < birth_prob) {
					Lattice[1][neighbor[0]][neighbor[1]] += 1;
					N[1] += 1;

					//Movement
				} else {
					Lattice[1][neighbor[0]][neighbor[1]] += 1;
					Lattice[1][x][y] -= 1;
				}
			}
		}
	}
}

LatticeSimulator::LatticeSimulator()
	: N(2, 0) {}

void LatticeSimulator::run_simulation(int size, double death, double birth, double pred,int num_of_timesteps, double n0) {

	death_rate = death;
	birth_rate = birth;
	pred_rate = pred;
	death_prob = death_rate / (1 + death_rate + pred_rate);
	pred_prob = pred_rate / (1 + death_rate + pred_rate);
	birth_prob = birth_rate / (1 + birth_rate);
	lattice_size = size;

	N[0] = N[1] = static_cast<int>(n0 * lattice_size * lattice_size);
	Lattice = std::vector<std::vector<std::vector<int>>>(2, std::vector<std::vector<int>>(lattice_size, std::vector<int>(lattice_size, 0)));
	initialize_lattice(N[0]);

	std::ostringstream oss; // String stream to build the output string
	oss.str("");
	oss.clear();


	//Run the transients simulation
	for (int t = 0; t < num_of_timesteps ; t++) {
		oss << static_cast<double>(N[0]) / (lattice_size * lattice_size) << " "
			<< static_cast<double>(N[1]) / (lattice_size * lattice_size) << std::endl;
		LatticeSimulator::update_lattice();
	}
	
	emit simulationComplete(oss.str());

}

void LatticeSimulator::run_simulation_SFML(int size, double death, double birth, double pred, double n0, bool frame_by_frame) {
	death_rate = death;
	birth_rate = birth;
	pred_rate = pred;
	death_prob = death_rate / (1 + death_rate + pred_rate);
	pred_prob = pred_rate / (1 + death_rate + pred_rate);
	birth_prob = birth_rate / (1 + birth_rate);
	lattice_size = size;

	N[0] = N[1] = static_cast<int>(n0 * lattice_size * lattice_size);
	Lattice = vector<vector<vector<int>>>(2, vector<vector<int>>(lattice_size, vector<int>(lattice_size, 0)));
	initialize_lattice(N[0]);

	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

	sf::RenderWindow window(desktop, "Lattice", sf::Style::Fullscreen);

	//Run the transients simulation
	while (window.isOpen()) {
		if (frame_by_frame) {
			LatticeSimulator::draw_lattice(window, desktop.width, desktop.height);
			sf::Event event;
			while (window.pollEvent(event)) {
				if (event.type == sf::Event::Closed)
					window.close();

				// Check if 'q' is pressed
				if (event.type == sf::Event::KeyPressed) {
					if (event.key.code == sf::Keyboard::Q) {
						window.close();  // Close the window to end the simulation
					} else if (event.key.code == sf::Keyboard::Right) {
						LatticeSimulator::update_lattice();
						window.clear();
					}
				}
			}
		} else {
			LatticeSimulator::draw_lattice(window, desktop.width, desktop.height);
			sf::Event event;
			while (window.pollEvent(event)) {
				if (event.type == sf::Event::Closed)
					window.close();

				// Check if 'q' is pressed
				if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Q) {
					window.close();  // Close the window to end the simulation
				}
			}
			LatticeSimulator::update_lattice();
			window.clear();
		}
	}
}
