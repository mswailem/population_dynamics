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
    QLineEdit *initialDensityInput = new QLineEdit(&window);
    QLineEdit *deathRateInput = new QLineEdit(&window);
    QLineEdit *birthRateInput = new QLineEdit(&window);
    QLineEdit *predRateInput = new QLineEdit(&window);
    QPushButton *runButton = new QPushButton("Run Simulation", &window);
    QCustomPlot *plotArea = new QCustomPlot(&window);
	
	// Set default values
	latticeSizeInput->setText("50");
	initialDensityInput->setText("0.5");
	deathRateInput->setText("0.5");
	birthRateInput->setText("0.5");
	predRateInput->setText("1");

    // Add widgets to layout
    layout->addWidget(new QLabel("Lattice Size:"),0,0);
    layout->addWidget(latticeSizeInput,0,1);
    layout->addWidget(new QLabel("Initial Density (n0):"),1,0);
    layout->addWidget(initialDensityInput,1,1);
    layout->addWidget(new QLabel("Death Rate:"),2,0);
    layout->addWidget(deathRateInput,2,1);
    layout->addWidget(new QLabel("Birth Rate:"),3,0);
    layout->addWidget(birthRateInput,3,1);
    layout->addWidget(new QLabel("Predation Rate:"),4,0);
    layout->addWidget(predRateInput,4,1);
    layout->addWidget(runButton,5,0,1,2);
    layout->addWidget(plotArea,6,0,1,2);
	bool RunningSimulation = false; //This flag hanldes whether the simulation is running or not

	LatticeSimulator* worker;
	// Connect button signal to slot
	QObject::connect(runButton, &QPushButton::clicked, [&](){
			if (!RunningSimulation) {
				RunningSimulation = true;
				runButton->setText("Stop Simulation");
				int lattice_size = latticeSizeInput->text().toInt();
				double n0 = initialDensityInput->text().toDouble();
				double death_rate = deathRateInput->text().toDouble();
				double birth_rate = birthRateInput->text().toDouble();
				double pred_rate = predRateInput->text().toDouble();

				worker = new LatticeSimulator();
				QThread* thread = new QThread();
				worker->moveToThread(thread);
				QObject::connect(thread, &QThread::started, worker, [worker, lattice_size, n0, death_rate, birth_rate, pred_rate]() {
						worker->run_simulation(lattice_size,death_rate,birth_rate,pred_rate,n0);
						});
				QObject::connect(worker, &LatticeSimulator::time_step_complete, plotArea, [plotArea](const std::string& result) {
						plotData(plotArea, result);
						}, Qt::QueuedConnection);

				thread->start();
				std::cout << "Thread started" << std::endl;
				QObject::connect(worker, &LatticeSimulator::simulation_complete, thread, &QThread::quit);
				QObject::connect(worker, &LatticeSimulator::simulation_complete, [&]() {
						RunningSimulation = false;
						runButton->setText("Run Simulation");
						});
				QObject::connect(worker, &LatticeSimulator::simulation_complete, worker, &LatticeSimulator::deleteLater);
				QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);
			} else {
				RunningSimulation = false;
				worker->StopSimulation();
				runButton->setText("Run Simulation");
			}
	});

    window.show();
    return app.exec();
}

void plotData(QCustomPlot *customPlot, const std::string &simulationOutput) {
    QVector<double> t, x, y;
    std::istringstream iss(simulationOutput);
    std::string line;
	int time = 0;
    while (std::getline(iss, line)) {
        double value1, value2;
        if (!(std::istringstream(line) >> value1 >> value2)) break;
		t.append(time);
        x.append(value1);
        y.append(value2);
		time++;
    }

	customPlot->xAxis->setLabel("Time");
	customPlot->yAxis->setLabel("Density");
	customPlot->yAxis->setRange(0, 1);
	customPlot->xAxis->setRange(time-100, time+100);
	/* customPlot->setInteractions(QCP::iRangeZoom | QCP::iRangeDrag); */
	/* customPlot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical); */
	/* customPlot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical); */
    customPlot->addGraph();
    customPlot->addGraph();
	/* customPlot->setInteraction(QCP::iRangeDrag, true); */
	/* customPlot->setInteraction(QCP::iSelectPlottables, true); */
    customPlot->graph(0)->setData(t, x);
    customPlot->graph(1)->setData(t, y);
	customPlot->graph(0)->setPen(QPen(Qt::red));
	customPlot->graph(1)->setPen(QPen(Qt::blue));
    customPlot->replot();
}

