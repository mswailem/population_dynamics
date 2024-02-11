This project is a software application written using the Qt libarary to simulate different population dynamics models.

The application uses Qt library for the UI and qcustomplot for plotting.

To build this project you need to have the Qt library and qtcustomplot installed, make a directory called build and run the following commands:

``` cmake .. ```

``` make ```

``` ./population_dynamics ```

The application will ask you for the parameters to run the simulation and then it will plot the results.

Clicking the "Run simulation" button will run the simulation and plot the densities versus time, until the "Stop simulation" button is clicked or until an abosrbing state is reached

TODO:
- Add more models
- Add another plot that shows the phase space plot
- Add a way to save the results to a file
- Use SFML to be able to also visualize the lattice along side the density plots
- More in the future
