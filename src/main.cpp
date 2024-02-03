#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include "qcustomplot.h"
#include <sstream>
#include <iostream>
#include "LV.h"

void plotData(QCustomPlot *customPlot, const std::string &simulationOutput);

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QWidget window;
    QVBoxLayout *layout = new QVBoxLayout(&window);

    // Creating input fields for each parameter
    QLineEdit *latticeSizeInput = new QLineEdit(&window);
    QLineEdit *numOfTimeStepsInput = new QLineEdit(&window);
    QLineEdit *initialDensityInput = new QLineEdit(&window);
    QLineEdit *deathRateInput = new QLineEdit(&window);
    QLineEdit *birthRateInput = new QLineEdit(&window);
    QLineEdit *predRateInput = new QLineEdit(&window);
    QLineEdit *capacityInput = new QLineEdit(&window);
    QPushButton *runButton = new QPushButton("Run Simulation", &window);
    QCustomPlot *plotArea = new QCustomPlot(&window);

    // Add widgets to layout
    layout->addWidget(new QLabel("Lattice Size:"));
    layout->addWidget(latticeSizeInput);
    layout->addWidget(new QLabel("Number of Time Steps:"));
    layout->addWidget(numOfTimeStepsInput);
    layout->addWidget(new QLabel("Initial Density (n0):"));
    layout->addWidget(initialDensityInput);
    layout->addWidget(new QLabel("Death Rate:"));
    layout->addWidget(deathRateInput);
    layout->addWidget(new QLabel("Birth Rate:"));
    layout->addWidget(birthRateInput);
    layout->addWidget(new QLabel("Predation Rate:"));
    layout->addWidget(predRateInput);
    layout->addWidget(new QLabel("Capacity (K):"));
    layout->addWidget(capacityInput);
    layout->addWidget(runButton);
    layout->addWidget(plotArea);

    // Connect button signal to slot
    QObject::connect(runButton, &QPushButton::clicked, [&](){
        int lattice_size = latticeSizeInput->text().toInt();
        int num_of_timesteps = numOfTimeStepsInput->text().toInt();
        double n0 = initialDensityInput->text().toDouble();
        double death_rate = deathRateInput->text().toDouble();
        double birth_rate = birthRateInput->text().toDouble();
        double pred_rate = predRateInput->text().toDouble();
        int K = capacityInput->text().toInt();

        std::string output = runLVSimulation(lattice_size, num_of_timesteps, n0, death_rate, birth_rate, pred_rate, K);
        plotData(plotArea, output); // Plot the data
    });

    window.show();
    return app.exec();
}

void plotData(QCustomPlot *customPlot, const std::string &simulationOutput) {
    QVector<double> x, y;
    std::istringstream iss(simulationOutput);
    std::string line;
    while (std::getline(iss, line)) {
        double value1, value2;
        if (!(std::istringstream(line) >> value1 >> value2)) break;
        x.append(value1);
        y.append(value2);
    }

    customPlot->addGraph();
    customPlot->graph(0)->setData(x, y);
    customPlot->replot();
}

