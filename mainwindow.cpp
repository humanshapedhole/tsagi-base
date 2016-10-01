#include "mainwindow.h"
#include "ui_mainwindow.h"

// *****************************************************

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ***************** SETTING MAIN WINDOW: **************

    setWindowIcon(QIcon(":/images/icon.png"));
    setWindowTitle("GwQ :: version 2.0");

    ui->centralWidget->setMinimumSize(1024, 600);

    // Соединение кнопок со слотами (тестирование)
    QObject::connect(ui->createMenuButton_mf, SIGNAL(clicked(bool)),
                     this, SLOT(createTreeWidgetItems()));
    QObject::connect(ui->closeButton_mf, SIGNAL(clicked(bool)),
                     this, SLOT(close()));

    // ************** SETTING MAIN MENU: ********************
    // createActions();                           createMenus();

    // ************** SETTING TREE WIDGET (UI): *************
    ui->treeWidget->setColumnCount(1);
    ui->treeWidget->setHeaderHidden(true);

    // ************ SETTING STACKED WIDGET (UI): ************
    QObject::connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
                     this, SLOT(switchStackedWidgetPage()));

    // ************* НАСТРОЙКА SERIAL PORT: *****************
    arduino_is_avaliable = false;
    arduino_port_name = "N/A";

    arduino = new QSerialPort;

    getSerialPortInfo();
    checkArduinoState();                  setArduinoPort();

    QObject::connect(arduino, SIGNAL(readyRead()),
                     this, SLOT(readSensorData()));

    QObject::connect(arduino, SIGNAL(readyRead()),
                     this, SIGNAL(dataReceived()));

    QObject::connect(ui->pushButton7_pg6, SIGNAL(clicked(bool)),
                     this, SLOT(turnSerialPlotOn()));

    QObject::connect(ui->pushButton8_pg6, SIGNAL(clicked(bool)),
                     this, SLOT(turnSerialPlotOff()));

    /*  ПОДКЛЮЧЕНО В СЛОТЫ ЧЕРЕЗ СИГНАЛЫ КНОПОК 7 И 8
    // ВКЛ ВЫВОДА ДАННЫХ С ДД НА ГРАФИК
    QObject::connect(this, SIGNAL(dataReceived()),
                     this, SLOT(addCurvePoint()));
    // ВЫКЛ ВЫВОДА ДАННЫХ С ДД НА ГРАФИК
    QObject::disconnect(this, SIGNAL(dataReceived()),
                        this, SLOT(addCurvePoint()));
    */

    // ************ ОТОБРАЖЕНИЕ ГРАФИКА С ДАННЫМИ: **********
    addPlot();                                 addPlotGrid();
    addCurve();
    setCounter(0);

    // ЭТО ВСЕ ВРЕМЕННО:
    c1Exist = false; c2Exist = false; c3Exist = false; c4Exist = false;
    connect(ui->pushButton1_pg6, SIGNAL(clicked(bool)),
            this, SLOT(addCurve1()));
    connect(ui->pushButton2_pg6, SIGNAL(clicked(bool)),
            this, SLOT(addCurve2()));
    connect(ui->pushButton3_pg6, SIGNAL(clicked(bool)),
            this, SLOT(addCurve3()));
    connect(ui->pushButton4_pg6, SIGNAL(clicked(bool)),
            this, SLOT(addCurve4()));
    connect(ui->pushButton5_pg6, SIGNAL(clicked(bool)),
            this, SLOT(deleteCurves()));
    // ******************************************************


}

// *****************************************************


// ****************** MENUS & ACTIONS: *****************

void MainWindow::createMenus()
{
    menuFile = menuBar()->addMenu(tr("Файл"));
        menuFile->addAction(actionNew);
        menuFile->addAction(actionExit);

    menuHelp = menuBar()->addMenu(tr("?"));
    menuHelp->setDisabled(true);
}

void MainWindow::createActions()
{
    // **************** НОВЫЙ ЭКСПЕРИМЕНТ ******************
    actionNew = new QAction(tr("Новый эксперимент"), this);
    //actionNew->setIcon(QIcon(":/icons/menu/new.png"));
    actionNew->setShortcut(QKeySequence::New);
    actionNew->setStatusTip(tr("Создать файл нового эксперимента"));
    //connect(actionNew, SIGNAL(triggered(bool)), this, SLOT(createNewFileDialog())); // нужно проработать сначала диалоговое окно

    // **************** ВЫЙТИ ИЗ ПРОГРАММЫ *****************
    actionExit = new QAction(tr("Выход"), this);
    //actionNew->setIcon(QIcon(":/icons/menu/exit.png"));
    actionExit->setShortcut(tr("Ctrl+A"));
    actionExit->setStatusTip(tr("Выйти из программы"));
    connect(actionExit, SIGNAL(triggered(bool)), this, SLOT(close()));
}

// *************** СОЗДАТЬ ДРЕВО МЕНЮ: *****************

void MainWindow::createTreeWidgetItems()
{
    ui->treeWidget->clear();
    // Построение древа меню:
    // 1. Установки
    itemSetup = new QTreeWidgetItem(ui->treeWidget);
    itemSetup->setText(0, tr("Установки"));
        // 1.1 Исходные данные
        itemInitialData = new QTreeWidgetItem(itemSetup);
        itemInitialData->setText(0, tr("Исходные данные"));

    // 2. Эксперимент
    itemExperiment = new QTreeWidgetItem(ui->treeWidget);
    itemExperiment->setText(0, tr("Эксперимент"));
        // 2.1 [TEMPORARY] Проверка управления шаговым двигателем
        itemStepMotor = new QTreeWidgetItem(itemExperiment);
        itemStepMotor->setText(0, tr("Управление шаговым двигателем"));
        // 2.2 [TEMPORARY] Проверка датчика давления
        itemSensor = new QTreeWidgetItem(itemExperiment);
        itemSensor->setText(0, tr("Чтение данных с датчика давления"));

    // 3. Результаты
    itemResults = new QTreeWidgetItem(ui->treeWidget);
    itemResults->setText(0, tr("Результаты"));
        // 3.1 Обработка данных
        itemDataHandling = new QTreeWidgetItem(itemResults);
        itemDataHandling->setText(0, tr("Обработка данных"));
}

// *************** ПЕРЕКЛЮЧАТЕЛЬ ФРЕЙМОВ: **************

void MainWindow::switchStackedWidgetPage()
{
    // Отображение выбранной страницы:
    // 1. Установки
    if (itemSetup->isSelected())
    {
        ui->stackedWidget->setCurrentIndex(TWI_SETUP);
    }
        // 1.1 Исходные данные
        else if (itemInitialData->isSelected())
        {
            ui->stackedWidget->setCurrentIndex(TWI_INITIAL_DATA);
        }
    // 2. Эксперимент
    else if (itemExperiment->isSelected())
    {
        ui->stackedWidget->setCurrentIndex(TWI_EXPERIMENT);
    }
        // 2.1 [TEMPORARY] Проверка управления шаговым двигателем
        else if (itemStepMotor->isSelected())
        {
            ui->stackedWidget->setCurrentIndex(TWI_STEP_MOTOR);
        }
        // 2.2 [TEMPORARY] Проверка датчика давления
        else if (itemSensor->isSelected())
        {
            ui->stackedWidget->setCurrentIndex(TWI_SENSOR);
        }
    // 3. Результаты
    else if (itemResults->isSelected())
    {
        ui->stackedWidget->setCurrentIndex(TWI_RESULTS);
    }
        // 3.1 Обработка данных
        else if (itemDataHandling->isSelected())
        {
            ui->stackedWidget->setCurrentIndex(TWI_DATA_HANDLING);
        }
}

// ****************** SERIAL PORT: *********************

void MainWindow::getSerialPortInfo()
{
    /* Getting port info (qDebug):*/
    qDebug() << "Number of avaliable ports: " << QSerialPortInfo::availablePorts().length();
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()) {
        qDebug() << "Port name: " << serialPortInfo.portName();
        qDebug() << "Has vendor ID: " << serialPortInfo.hasVendorIdentifier();
        if (serialPortInfo.hasVendorIdentifier()) {
            qDebug() << "vendor ID: " << serialPortInfo.vendorIdentifier();
        }
        qDebug() << "Has product ID: " << serialPortInfo.hasProductIdentifier();
        if (serialPortInfo.hasProductIdentifier()) {
            qDebug() << "product ID: " << serialPortInfo.productIdentifier();
            qDebug() << "Serial number: " << serialPortInfo.serialNumber();
        }
        qDebug() << "\n";
    }
}

void MainWindow::checkArduinoState()
{
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts())
    {
        if(serialPortInfo.hasProductIdentifier() && serialPortInfo.hasVendorIdentifier())
        {
            if(serialPortInfo.vendorIdentifier() == arduino_nano_vendor_id)
            {
                if(serialPortInfo.productIdentifier() == arduino_nano_product_id)
                {
                    arduino_port_name = serialPortInfo.portName();
                    arduino_is_avaliable = true;
                }
            }
        }
    }
}

void MainWindow::setArduinoPort()
{
    if(arduino_is_avaliable)
    {
        // Everything is good
        arduino->setPortName(arduino_port_name);
        arduino->open(QSerialPort::ReadWrite);
        arduino->setBaudRate(QSerialPort::Baud115200);
        arduino->setDataBits(QSerialPort::Data8);
        arduino->setParity(QSerialPort::NoParity);
        arduino->setStopBits(QSerialPort::OneStop);
        arduino->setFlowControl(QSerialPort::NoFlowControl);
    }
    else
    {
        // Error message
        QMessageBox::warning(this, "Ошибка подключения", "Устройство не найдено!");
    }
}

void MainWindow::writeToArduino()
{
    //arduino->write(packet, packet.length()); // only for QByteArray
    arduino->write((const char*)packet, 4); // for unsigned char[]
}

void MainWindow::readSensorData()
{
    arduino->read((char*)packet, 4);
    if (packet[0] == 0xB2 && packet[3] == 0xB2)
    {
        int val = packet[1]*256 + packet[2];
        //ui->lineEdit_receiveSensorData_pg7->setText(QString::number(val));
        capturedData = val;
        // emit dataReceived(); ВЫЗЫВАЕТСЯ НАПРЯМУЮ ЧЕРЕЗ СИГНАЛ READYREAD();
    }
}

/*  УСТАНОВКА НАСАДКА В НУЖНОE ПОЛОЖЕНИЕ
void MainWindow::NO_NAME()
{
    if (arduino->isWritable())
    {
        // aggregate data:
        float prev_val = ui->doubleSpinBox_rotAngle_pg6->value();                   //qDebug() << "prev_val: " << prev_val;
        int val = prev_val * 8;                                                     //qDebug() << "val: " << val;
        prev_val = val / 8.;                                                        //qDebug() << "prev_val: " << prev_val;
        ui->doubleSpinBox_rotAngle_pg6->setValue(prev_val);

        int stepMotorNumber = ui->comboBox_stepMotor_pg6->currentIndex();           //qDebug() << "step_motor_N: " << stepMotorNumber;
        int direction;
        if (ui->radioButton_forward_pg6->isChecked())       direction = 1;
        else                                                direction = 3;

        // form package:
        packet[0] = 0x20 + stepMotorNumber + direction; // set arduino -> getPacket = true:
        packet[1] = static_cast<int>(val/256);
        packet[2] = val % 256;
        packet[3] = packet[0]; // control byte

        qDebug() << "Debug:";
        qDebug() << "packet[0]: " << (void *)packet[0];
        qDebug() << "packet[1]: " << (void *)packet[1];
        qDebug() << "packet[2]: " << (void *)packet[2];
        qDebug() << "packet[3]: " << (void *)packet[3];
        qDebug() << "value: " << val;

        writeToArduino();
    }
}
*/

// ****************** PLOTTING DATA: *******************

void MainWindow::addPlot()
{
    // Font:
    QFont *font12 = new QFont();
    font12->setPixelSize(12);
    font12->setBold(false);

    QFont *font24 = new QFont();
    font24->setPixelSize(24);
    font24->setBold(false);
    // Title:
    QwtText *plotTitle = new QwtText();
    plotTitle->setFont(*font12);
    plotTitle->setColor(Qt::black);
    plotTitle->setText("Заголовок графика");
    ui->plot->setTitle(*plotTitle); // о, работает!

    ui->plot->setCanvasBackground(Qt::white);
    //ui->plot->canvas()->setCursor(Qt::ArrowCursor);

    ui->plot->setFont(*font12);
    ui->plot->setFooter("Подпись по графиком");

    ui->plot->setToolTip("Всплывающая подсказка");
    //ui->plot->setToolTipDuration(250);

    ui->plot->setAccessibleDescription("Accessible description");
    //ui->plot->axisTitle(QwtPlot::xBottom)->setFont(*font24);
    //ui->plot->axisWidget(QwtPlot::yLeft)->setFont(*font24);
    // Параметры осей координат
    //ui->plot->setAxisFont(QwtPlot::yLeft, *font12);
    //ui->plot->setAxisFont(QwtPlot::xBottom, *font12);

    ui->plot->setAxisTitle(QwtPlot::yLeft, "Показания датчика, Па");
    ui->plot->setAxisTitle(QwtPlot::xBottom, "Время, с");
    setPlotMargins(0.0, 5.0, -10.0, 10.0);

    QwtLegend *legend = new QwtLegend();
    ui->plot->insertLegend(legend);

    painter = new QwtPlotDirectPainter(ui->plot);

    ui->plot->setAutoReplot(true);
}

void MainWindow::addPlotGrid()
{
    grid = new QwtPlotGrid();
    grid->setMajorPen(QPen(Qt::gray, 1)); // цвет линий и толщина
    grid->attach(ui->plot);
}

void MainWindow::addCurve()
{
    curve = new QwtPlotCurve("y(x)");
    curve->setTitle("Показания датчика давления");
    curve->setStyle(QwtPlotCurve::Lines);
    curve->setPen(QPen(Qt::magenta, 2));
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    curve->attach(ui->plot);
}

void MainWindow::setPlotMargins(double _x1, double _x2, double _y1, double _y2)
{
    bottomMargin = _y1;                         topMargin = _y2;
    leftMargin = _x1;                           rightMargin = _x2;

    ui->plot->setAxisScale(QwtPlot::xBottom, leftMargin, rightMargin);
    ui->plot->setAxisScale(QwtPlot::yLeft, bottomMargin, topMargin);
}

void MainWindow::addCurvePoint()
{
    // REALTIMEPLOT:

    counter++;
    float x = counter/2.0;          float y = (capturedData - 510)/0.3956;
    point = QPointF(x, y);
    ui->plainTextEdit->setPlainText(QString::number(y));

    CurveData *data = static_cast<CurveData*>(curve->data());
    data->appendDataPoint(point);
    painter->drawSeries(curve, 0, data->size() - 1);

    if (x > 4.0/5.0*rightMargin)
        rightMargin = x*5.0/4.0;
    if (y > 4.0/5.0*topMargin)
        topMargin = 5.0/4.0*y;
    if (y < 4.0/5.0*bottomMargin)
        bottomMargin = 5.0/4.0*y;

    setPlotMargins(0.0, rightMargin, bottomMargin, topMargin);
    // #
}

// ВРЕМЕННЫЕ МЕТОДЫ РИСОВАНИЯ ГРАФИКОВ:

void MainWindow::addCurve1()
{
    if (c1Exist)    delete curve1;

    curve1 = new QwtPlotCurve(QString("y = sin(x)"));
    curve1->setRenderHint(QwtPlotItem::RenderAntialiased);
    curve1->setPen(QPen(Qt::green, 2));

    const int n = 314;
    double x[2*n], y[2*n];

    for (int i=0; i<2*n; i++)
    {
        x[i] = i/100.0;
        y[i] = sin(x[i]);
    }

    curve1->setSamples(x, y, 2*n);
    curve1->attach(ui->plot);
    c1Exist = true;
    ui->plot->replot();
}

void MainWindow::addCurve2()
{
    if (c2Exist)    delete curve2;

    curve2 = new QwtPlotCurve(QString("y = cos(x)"));
    curve2->setRenderHint(QwtPlotItem::RenderAntialiased);
    curve2->setPen(QPen(Qt::blue, 2));

    const int n = 314;
    double x[2*n], y[2*n];

    for (int i=0; i<2*n; i++)
    {
        x[i] = i/100.0;
        y[i] = cos(x[i]);
    }

    curve2->setSamples(x, y, 2*n);
    curve2->attach(ui->plot);
    c2Exist = true;
    ui->plot->replot();
}

void MainWindow::addCurve3()
{
    if (c3Exist)    delete curve3;

    curve3 = new QwtPlotCurve(QString("y = sin(2x)"));
    curve3->setRenderHint(QwtPlotItem::RenderAntialiased);
    curve3->setPen(QPen(Qt::red, 2));

    const int n = 314;
    double x[2*n], y[2*n];

    for (int i=0; i<2*n; i++)
    {
        x[i] = i/100.0;
        y[i] = sin(2*x[i]);
    }

    curve3->setSamples(x, y, 2*n);
    curve3->attach(ui->plot);
    c3Exist = true;
    ui->plot->replot();
}

void MainWindow::addCurve4()
{
    if (c4Exist)    delete curve4;

    curve4 = new QwtPlotCurve(QString("y = sin(3x)"));
    curve4->setRenderHint(QwtPlotItem::RenderAntialiased);
    curve4->setPen(QPen(Qt::magenta, 2));

    const int n = 314;
    double x[2*n], y[2*n];

    for (int i=0; i<2*n; i++)
    {
        x[i] = i/100.0;
        y[i] = sin(3*x[i]);
    }

    curve4->setSamples(x, y, 2*n);
    curve4->attach(ui->plot);
    c4Exist = true;
    ui->plot->replot();
}

void MainWindow::deleteCurves()
{
    if (c1Exist)
    {
        delete curve1;
        c1Exist = false;
    }
    if (c2Exist)
    {
        delete curve2;
        c2Exist = false;
    }
    if (c3Exist)
    {
        delete curve3;
        c3Exist = false;
    }
    if (c4Exist)
    {
        delete curve4;
        c4Exist = false;
    }
}

void MainWindow::turnSerialPlotOff()
{
    // ВЫКЛ ВЫВОДА ДАННЫХ С ДД НА ГРАФИК
    QObject::disconnect(this, SIGNAL(dataReceived()),
                        this, SLOT(addCurvePoint()));

    ui->plainTextEdit->setPlainText("OFF");
}

void MainWindow::turnSerialPlotOn()
{
    // ВКЛ ВЫВОДА ДАННЫХ С ДД НА ГРАФИК
    QObject::connect(this, SIGNAL(dataReceived()),
                     this, SLOT(addCurvePoint()));
}

// *****************************************************

// ******************** DESTRUCTOR: ********************

MainWindow::~MainWindow()
{
    delete ui;
    delete grid;
    // delete curve;
}
