#include <array>
#include <vector>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "RPSSimulator.h"

using namespace std;


void LatticeSimulator::initialize_lattice_hopping() {
	for (int s = 0; s < 3; s++) {
		for (int i = 0; i < N[0]; i++) {
			int x = static_cast<int>(dist(rng) * lattice_size);
			int y = static_cast<int>(dist(rng) * lattice_size);
			Lattice[s][x][y] += 1;
		}
	}
}

void LatticeSimulator::initialize_lattice_exchange(int n0) {
	int N_max[3];
	N_max[0] = N[0];
	N_max[1] = N[1];
	N_max[2] = N[2];
	for (int x=0; x < lattice_size; x++) {
		for (int y=0; y < lattice_size; y++) {
			for (int p=0; p < 3*n0; p++) {
				int s = floor(dist(rng) * 3);
				if (N_max[s] > 0) {
					Lattice[s][x][y] += 1;
					N_max[s] -= 1;
				} else {
					p--;
				}
			}
		}
	}
}	

//Pick a random neighbor of a site
array<int, 2> LatticeSimulator::pick_neighbor(int x, int y) {
	array<int, 2> site;
	int dice_dir = static_cast<int>(dist(rng) * 4);

	//This picks a random neighbor of a site and checks boudnaries based on the dice roll
	site[0] = (x + ((dice_dir == 1) - (dice_dir == 2)) + lattice_size) % lattice_size;
	site[1] = (y + ((dice_dir == 3) - (dice_dir == 0)) + lattice_size) % lattice_size;
	return site;
}

array<int, 2> LatticeSimulator::pick_site() {
	array<int, 2> site;
	int dice_site = ceil(dist(rng) * (N[0]+N[1]+N[2]));
	int site_prob = 0;
	for(int x = 0; x < lattice_size; x++) {
		for(int y = 0; y < lattice_size; y++) {
			site_prob += Lattice[0][x][y] + Lattice[1][x][y] + Lattice[2][x][y];
			if (dice_site <= site_prob) {
				site[0] = x;
				site[1] = y;
				return site;
			}
		}
	}
	return site;
}

//Draw the lattice
void LatticeSimulator::draw_lattice(sf::RenderWindow& window, int window_size_x, int window_size_y, double n0) {
	sf::RectangleShape rectangle(sf::Vector2f(window_size_x / lattice_size, window_size_y / lattice_size));
	for (int i = 0; i < lattice_size; i++) {
		for (int j = 0; j < lattice_size; j++) {

			int red = std::min(255, (Lattice[0][i][j] * 255) / 3*static_cast<int>(n0));
			int green = std::min(255, (Lattice[1][i][j] * 255) / 3*static_cast<int>(n0));
			int blue = std::min(255, (Lattice[2][i][j] * 255) / 3*static_cast<int>(n0));
			sf::RectangleShape rectangle(sf::Vector2f(window_size_x / lattice_size, window_size_y / lattice_size));
			rectangle.setPosition(i * window_size_x / lattice_size, j * window_size_y / lattice_size);
			rectangle.setFillColor(sf::Color(red, green, blue));
			window.draw(rectangle);
		}
	}
	window.display();
}

void LatticeSimulator::save_lattice(int window_size_x, int window_size_y, double n0, string filename) {
	sf::RenderTexture renderTexture;
	if (!renderTexture.create(window_size_x, window_size_y)) {
		cerr << "Failed to create render texture!" << endl;
		return;
	}

	sf::RectangleShape rectangle(sf::Vector2f(window_size_x / lattice_size, window_size_y / lattice_size));
	for (int i = 0; i < lattice_size; i++) {
		for (int j = 0; j < lattice_size; j++) {
			int red = std::min(255, (Lattice[0][i][j] * 255) / 3*static_cast<int>(n0));
			int green = std::min(255, (Lattice[1][i][j] * 255) / 3*static_cast<int>(n0));
			int blue = std::min(255, (Lattice[2][i][j] * 255) / 3*static_cast<int>(n0));
			sf::RectangleShape rectangle(sf::Vector2f(window_size_x / lattice_size, window_size_y / lattice_size));
			rectangle.setPosition(i * window_size_x / lattice_size, j * window_size_y / lattice_size);
			rectangle.setFillColor(sf::Color(red, green, blue));
			renderTexture.draw(rectangle);
		}
	}

	renderTexture.display();
	sf::Texture texture = renderTexture.getTexture();
	sf::Image image = texture.copyToImage();
	if (!image.saveToFile(filename)) {
		std::cerr << "Failed to save image to file: " << filename << std::endl;
	}
}

void LatticeSimulator::update_lattice(bool exchange_on) {
	int hits = N[0] + N[1] + N[2];

	array<int, 2> site;
	array<int, 2> neighbor;
	int x, y,x_n,y_n, s, s_next, dice_particle, iterations;
	double dice_reaction;

	while (hits > 0) {

		//If there are no particles, return
		if (N[0] + N[1] + N[2] == 0) {
			return;
		}
		
		//Picking a site
		site = pick_site();
		x = site[0];
		y = site[1];
		hits -= 1;

		//Picking a species
		dice_particle = ceil(dist(rng) * (Lattice[0][x][y] + Lattice[1][x][y] + Lattice[2][x][y]));
		if (dice_particle <= Lattice[0][x][y]) {
			s = 0;
			s_next = 1;
		} else if (dice_particle <= Lattice[0][x][y] + Lattice[1][x][y]) {
			s = 1;
			s_next = 2;
		} else {
			s = 2;
			s_next = 0;
		}

		//Picking a reaction
		dice_reaction = dist(rng);	
		neighbor = pick_neighbor(x, y);
		x_n = neighbor[0];
		y_n = neighbor[1];

		if (dice_reaction <= pred_prob) {
			if (Lattice[s_next][x_n][y_n] != 0) {
				iterations = Lattice[s_next][x_n][y_n];
				Lattice[s_next][x_n][y_n] -= 1;
				Lattice[s][x_n][y_n] += 1;
				N[s_next] -= 1;
				N[s] += 1;
				for (int i=1; i<iterations; i++){
					double dice_pred = dist(rng);
					if (dice_pred <= pred_prob) {
						Lattice[s_next][x_n][y_n] -= 1;
						Lattice[s][x_n][y_n] += 1;
						N[s_next] -= 1;
						N[s] += 1;
					}
				}
			}
		} else if (exchange_on) {
			dice_particle = ceil(dist(rng) * (Lattice[0][x_n][y_n] + Lattice[1][x_n][y_n] + Lattice[2][x_n][y_n]));
			if (dice_particle <= Lattice[0][x_n][y_n]) {
				s_next = 0;
			} else if (dice_particle <= Lattice[0][x_n][y_n] + Lattice[1][x_n][y_n]) {
				s_next = 1;
			} else {
				s_next = 2;
			}
			Lattice[s][x_n][y_n] += 1;
			Lattice[s][x][y] -= 1;
			Lattice[s_next][x_n][y_n] -= 1;
			Lattice[s_next][x][y] += 1;
		} else {
			Lattice[s][x_n][y_n] += 1;
			Lattice[s][x][y] -= 1;
		}
	}
}

LatticeSimulator::LatticeSimulator() : N(3, 0) {}

void LatticeSimulator::run_simulation(int size,double pred, double n0, bool frame_by_frame, bool exchange_on) {
	pred_prob = pred / (1 + pred);
	lattice_size = size;

	N[0] = N[1] = N[2] = static_cast<int>(n0 * lattice_size * lattice_size);
	Lattice = vector<vector<vector<int>>>(3, vector<vector<int>>(lattice_size, vector<int>(lattice_size, 0)));
	if (exchange_on) {
		initialize_lattice_exchange(n0);
	} else {
		initialize_lattice_hopping();
	}

	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

	sf::RenderWindow window(desktop, "Lattice", sf::Style::Fullscreen);

	while (window.isOpen()) {
		if (frame_by_frame) {
			LatticeSimulator::draw_lattice(window, desktop.width, desktop.height, n0);
			sf::Event event;
			while (window.pollEvent(event)) {
				if (event.type == sf::Event::Closed)
					window.close();

				// Check if 'q' is pressed
				if (event.type == sf::Event::KeyPressed) {
					if (event.key.code == sf::Keyboard::Q) {
						window.close();  // Close the window to end the simulation
					} else if (event.key.code == sf::Keyboard::Right) {
						LatticeSimulator::update_lattice(exchange_on);
						window.clear();
					}
				}
			}
		} else {
			LatticeSimulator::draw_lattice(window, desktop.width, desktop.height, n0);
			sf::Event event;
			while (window.pollEvent(event)) {
				if (event.type == sf::Event::Closed)
					window.close();

				// Check if 'q' is pressed
				if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Q) {
					window.close();  // Close the window to end the simulation
				}
			}
			LatticeSimulator::update_lattice(exchange_on);
			window.clear();
		}
	}

}

void LatticeSimulator::run_simulation_save(int size,double pred, double n0, int num_of_timesteps, bool frame_by_frame, bool exchange_on) {

	pred_prob = pred / (1 + pred);
	lattice_size = size;
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

	N[0] = N[1] = N[2] = static_cast<int>(n0 * lattice_size * lattice_size);
	Lattice = vector<vector<vector<int>>>(3, vector<vector<int>>(lattice_size, vector<int>(lattice_size, 0)));
	if (exchange_on) {
		initialize_lattice_exchange(n0);
	} else {
		initialize_lattice_hopping();
	}

	LatticeSimulator::save_lattice(desktop.width, desktop.height, n0, "../images/0.png");
	for (int t = 1; t < num_of_timesteps; t++) {
		cout << "Timestep: " << t << endl;
		LatticeSimulator::update_lattice(exchange_on);
		LatticeSimulator::save_lattice(desktop.width, desktop.height, n0, "../images/" + to_string(t) + ".png");
	}

}


