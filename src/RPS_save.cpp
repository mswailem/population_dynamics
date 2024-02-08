#include "RPSSimulator.h"
#include <iostream>

int main (int argc, char *argv[]) {
	
	//Checking user input
	if (argc < 5) {
		std::cerr << "Usage: " << argv[0] << " lattice_size pred_rate n0 t_max [-e] [-f] [-ef]" << std::endl;
		return -1;
	}

	//Reading input
	const int lattice_size = std::stoi(argv[1]);
	const double pred_rate = std::stod(argv[2]);
	const double n0 = std::stod(argv[3]);
	const int t_max = std::stoi(argv[4]);
	
    bool exchange_on = false; // Default value is false
    bool frame_by_frame = false; // Default value is false

	//Checking for optional arugments
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-e") {
            exchange_on = true;
        } else if (std::string(argv[i]) == "-f") {
			frame_by_frame = true;
		} else if (std::string(argv[i]) == "-ef") {
			exchange_on = true;
			frame_by_frame = true;
		}
    }

	LatticeSimulator sim;
	sim.run_simulation_save(100, 1, 1, t_max, frame_by_frame, exchange_on);

	return 0;
}

