#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include "qcustomplot.h"
#include <sstream>
#include <iostream>
#include <QThread>
#include <QMetaType>
#include "LVSimulator.h"

void plotData(QCustomPlot *customPlot, const std::string &simulationOutput);

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);
	
	//This line is necessary to use std::string in signal/slot connections
	qRegisterMetaType<std::string>("std::string");
	
	// Setting up window
    QWidget window;
	window.resize(1280, 720);
	QGridLayout *layout = new QGridLayout(&window);

    // Creating input fields for each parameter
    QLineEdit *latticeSizeInput = new QLineEdit(&window);
    QLineEdit *numOfTimeStepsInput = new QLineEdit(&window);
    QLineEdit *initialDensityInput = new QLineEdit(&window);
    QLineEdit *deathRateInput = new QLineEdit(&window);
    QLineEdit *birthRateInput = new QLineEdit(&window);
    QLineEdit *predRateInput = new QLineEdit(&window);
    QPushButton *runButton = new QPushButton("Run Simulation", &window);
    QCustomPlot *plotArea = new QCustomPlot(&window);

    // Add widgets to layout
    layout->addWidget(new QLabel("Lattice Size:"),0,0);
    layout->addWidget(latticeSizeInput,0,1);
    layout->addWidget(new QLabel("Number of Time Steps:"),1,0);
    layout->addWidget(numOfTimeStepsInput,1,1);
    layout->addWidget(new QLabel("Initial Density (n0):"),2,0);
    layout->addWidget(initialDensityInput,2,1);
    layout->addWidget(new QLabel("Death Rate:"),3,0);
    layout->addWidget(deathRateInput,3,1);
    layout->addWidget(new QLabel("Birth Rate:"),4,0);
    layout->addWidget(birthRateInput,4,1);
    layout->addWidget(new QLabel("Predation Rate:"),5,0);
    layout->addWidget(predRateInput,5,1);
    layout->addWidget(runButton,7,0,1,2);
    layout->addWidget(plotArea,8,0,1,2);

    // Connect button signal to slot
    QObject::connect(runButton, &QPushButton::clicked, [&](){
        int lattice_size = latticeSizeInput->text().toInt();
        int num_of_timesteps = numOfTimeStepsInput->text().toInt();
        double n0 = initialDensityInput->text().toDouble();
        double death_rate = deathRateInput->text().toDouble();
        double birth_rate = birthRateInput->text().toDouble();
        double pred_rate = predRateInput->text().toDouble();

		LVSimulator* worker = new LVSimulator();
		QThread* thread = new QThread();
		worker->moveToThread(thread);
		QObject::connect(thread, &QThread::started, worker, &LVSimulator::run_simulatrion);
		QObject::connect(worker, &LVSimulator::simulationComplete, plotArea, [plotArea](const std::string& result) {
				plotData(plotArea, result); // This now executes in the main thread
				}, Qt::QueuedConnection);

		thread->start();
		});

    window.show();
    return app.exec();
}

void plotData(QCustomPlot *customPlot, const std::string &simulationOutput) {
    QVector<double> x, y;
    std::istringstream iss(simulationOutput);
	std::cout << simulationOutput << std::endl;
    std::string line;
    while (std::getline(iss, line)) {
        double value1, value2;
        if (!(std::istringstream(line) >> value1 >> value2)) break;
        x.append(value1);
        y.append(value2);
    }

	customPlot->setInteractions(QCP::iRangeZoom | QCP::iRangeDrag);
	customPlot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
	customPlot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    customPlot->addGraph();
	customPlot->setInteraction(QCP::iRangeDrag, true);
	customPlot->setInteraction(QCP::iSelectPlottables, true);
    customPlot->graph(0)->setData(x, y);
    customPlot->replot();
}

