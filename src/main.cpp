#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include "qcustomplot.h" // Include QCustomPlot

void setupPlot(QCustomPlot *customPlot) {
    // Generate data for x^2 plot
    QVector<double> x(101), y(101);
    for (int i = 0; i < 101; ++i) {
        x[i] = i/50.0 - 1; // x goes from -1 to 1
        y[i] = x[i] * x[i]; // let's plot x^2
    }

    // Create graph and assign data to it
    customPlot->addGraph();
    customPlot->graph(0)->setData(x, y);

    // Give the axes labels and set axes ranges
    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");
    customPlot->xAxis->setRange(-1, 1);
    customPlot->yAxis->setRange(0, 1);
	customPlot->setInteractions(QCP::iRangeZoom | QCP::iRangeDrag);
	customPlot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
	customPlot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
	customPlot->setInteraction(QCP::iRangeDrag, true);



    // Replot to update the view
    customPlot->replot();
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Create main widget
    QWidget window;
    window.setWindowTitle("Qt Application with QCustomPlot");

    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(&window);

    // Create button
    QPushButton *button = new QPushButton("Plot x^2");
    layout->addWidget(button);

    // Create QCustomPlot widget
    QCustomPlot *customPlot = new QCustomPlot();
    layout->addWidget(customPlot);

    // Connect button signal to plot setup function
    QObject::connect(button, &QPushButton::clicked, [=](){
        setupPlot(customPlot);
    });

    // Show window
    window.show();

    return app.exec();
}

