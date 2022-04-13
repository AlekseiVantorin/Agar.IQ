#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingswindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    scene = new Scene;
    connect(scene, &Scene::first, this, &MainWindow::show);
    connect(this, &MainWindow::signalStartGame, scene, &Scene::startGame);
    //  scene = new Scene;
    // ui->scene->setParent(scene);
    //   scene->show();
    //   this->hide();
}

MainWindow::~MainWindow() {
    delete scene;
    delete ui;
}


void MainWindow::on_startButton_clicked()
{
    scene->showFullScreen();
    this->close();
    emit signalStartGame();
}


void MainWindow::on_settingsButton_clicked()
{
    SettingsWindow *sw = new SettingsWindow();
    sw->showFullScreen();
}



void MainWindow::on_greenButton_clicked()
{
    repaint();
}


void MainWindow::on_yellowButton_clicked()
{
    repaint();
}


void MainWindow::on_purpleButton_clicked()
{
    repaint();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::FlatCap));


    if(ui->greenButton->isChecked()){

        painter.setBrush(QBrush(Qt::green, Qt::SolidPattern));
        painter.drawEllipse(1455, 375, 200, 200);
    } else if(ui->yellowButton->isChecked()){

        painter.setBrush(QBrush(Qt::yellow, Qt::SolidPattern));
        painter.drawEllipse(1455, 375, 200, 200);
    } else if(ui->purpleButton->isChecked()){

        painter.setBrush(QBrush(Qt::magenta, Qt::SolidPattern));
        painter.drawEllipse(1455, 375, 200, 200);
    } else {

        painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));
        painter.drawEllipse(1455, 375, 200, 200);
    }
}

void MainWindow::on_closeButton_clicked()
{
    this->close();
}

