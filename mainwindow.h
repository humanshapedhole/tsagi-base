#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// ******************** EXTERNAL LIBRARIES: *******************

#include <QMainWindow>

#include <QTreeWidget>

#include <QIODevice>
#include <QFile>

#include <QSerialPort>
#include <QSerialPortInfo>

#include <QtWidgets> // для диалогов
#include <QDebug>

// Qwt:

#include <qwt_plot.h>
#include <qwt_plot_grid.h>
#include <qwt_legend.h>
#include <qwt_plot_curve.h>

#include <qwt_scale_widget.h>

// ******************** EXTERNAL HEADERS: *********************

#include "realtimeplot.h"

// **************** TREE WIDGET ITEMS INDEXES: ****************

#define TWI_SETUP                                           1
    #define TWI_INITIAL_DATA                                4

#define TWI_EXPERIMENT                                      2
    #define TWI_STEP_MOTOR                                  5
    #define TWI_SENSOR                                      6

#define TWI_RESULTS                                         3
    #define TWI_DATA_HANDLING                               7

// ************************************************************

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:

    // *************** SERIAL PORT SIGNALS: *******************

    dataReceived();

private slots:

    // **************** TREE WIDGET SLOTS: ********************
    void createTreeWidgetItems();

    // ************** STACKED WIDGET SLOTS: *******************
    void switchStackedWidgetPage();

    // **************** SERIAL PORT SLOTS: ********************
    void readSensorData();

    // *************** QWT PLOT DATA SLOTS: *******************
    void addCurvePoint();

    void addCurve1();                         void addCurve2();
    void addCurve3();                         void addCurve4();


    void turnSerialPlotOn();
    void turnSerialPlotOff();

    void deleteCurves();

private:
    Ui::MainWindow *ui;

    // *************** MAIN MENU & ACTIONS: *******************
    void createMenus();
    QMenu *menuFile, *menuHelp;
    void createActions();
    QAction *actionNew, *actionExit;

    // **************** TREE WIDGET ITEMS: ********************
    //TO SLOTS: void createTreeWidgetItems();

    QTreeWidgetItem *itemSetup,                 // Установки
                        *itemInitialData,       // Исходные данные
                    *itemExperiment,            // Эксперимент
                        *itemStepMotor,         // Тестирование ШД
                        *itemSensor,            // Тестирование датчика
                    *itemResults,               // Результаты
                        *itemDataHandling;      // Обработка данных

    // ************** STACKED WIDGET METHODS: *****************
    //TO SLOTS: void switchStackedWidgetPage();

    // *************** SERIAL PORT MANAGMENT: *****************
    QSerialPort *arduino;

    // Проверка устройства на совместимость:
    static const quint16 arduino_nano_vendor_id = 6790;
    static const quint16 arduino_nano_product_id = 29987;
    QString arduino_port_name;
    bool arduino_is_avaliable;

    unsigned char packet[4]; // пакет данных V2
    void writeToArduino();

    void getSerialPortInfo();         void checkArduinoState();
    void setArduinoPort();

    //TO SLOTS: void readSensorData();
    //TO SLOTS: void NO_NAME(); // установка насадка в положение
                                // *доработать и разместить в слотах*

    int capturedData;

    // ******************* PLOTTING DATA: *********************
    void addPlot();                         void addPlotGrid();
    void addCurve();

    int topMargin, bottomMargin, leftMargin, rightMargin;
    int xCentre, yCentre;

    void setPlotMargins(double, double, double, double);
    void setPlotMargins();

    int counter;
    void setCounter(int _counter)
    {
        counter = _counter;
    }
    QPointF point;

    QwtPlotGrid *grid;
    QwtPlotCurve *curve;
    QwtPlotDirectPainter *painter;

    // Curves (временно, для тестирования):
    QwtPlotCurve *curve1, *curve2, *curve3, *curve4;
    bool c1Exist, c2Exist, c3Exist, c4Exist;
    // Ещё надо потом будет удалить методы из слотов!

    QPolygonF points;

    // ********************************************************

};

#endif // MAINWINDOW_H
