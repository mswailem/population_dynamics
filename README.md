This project is a software application written using the Qt libarary to simulate different population dynamics models.

The application uses Qt library for the UI and qcustomplot for plotting.

To build this project you need to have the Qt library and qtcustomplot installed, make a directory called build and run the following commands:

``` cmake .. ```

``` make ```

``` ./population_dynamics ```

The application will ask you for the parameters to run the simulation and then it will plot the results.

The simulation runs on a different thread so the UI will not freeze while the simulation is running.

TODO:
- Add more models
- Make it so that it can only simulate K=1, for quicker use
- Make it so that the simulation plots each time step until the user hits the "stop simulation" button
- Add a way to save the results to a file
- Use SFML to be able to also visualize the lattice along side the density plots
- More in the future
