#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_action_Quit_triggered()
{
	 QApplication::quit();
}

void MainWindow::on_checkBoxFill_toggled(bool checked)
{
	 ui->openGLWidget->setPolygonMode(checked);
}

void MainWindow::on_action_Open_triggered()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Open PLY"), ".", tr("*.ply"));

	ui->openGLWidget->loadMesh(filename);
}

void MainWindow::on_resetButton_clicked()
{
    ui->openGLWidget->resetScene();
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    if(arg1==0){
        ui->openGLWidget->changeSolver(false);
    }else{
        ui->openGLWidget->changeSolver(true);
    }
}

void MainWindow::on_checkBox_2_stateChanged(int arg1)
{
    if(arg1==1){
        ui->openGLWidget->changeSolver(false);
    }else{
        ui->openGLWidget->changeSolver(true);
    }
}

void MainWindow::on_SPX_valueChanged(double arg1)
{
    std::cout<<"cambio valor X a "<<arg1<<std::endl;
    ui->openGLWidget->setXPosr((float)arg1);
}

void MainWindow::on_SPY_valueChanged(double arg1)
{
 ui->openGLWidget->setYPosr((float)arg1);
}

void MainWindow::on_SPZ_valueChanged(double arg1)
{
   ui->openGLWidget->setZPosr((float)arg1);
}

void MainWindow::on_rbFountain_clicked(bool checked)
{
    ui->openGLWidget->changeFountain(checked);
}

void MainWindow::on_rbWaterfall_clicked(bool checked)
{
 ui->openGLWidget->changeFountain(!checked);
}

void MainWindow::on_checkBox_clicked(bool checked)
{
    ui->openGLWidget->changeSolver(checked);
}

void MainWindow::on_checkBox_2_clicked(bool checked)
{
    ui->openGLWidget->changeSolver(!checked);
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    ui->openGLWidget->changeHomework(index);
}

void MainWindow::on_reset1_clicked()
{
    ui->openGLWidget->changeHomeworkState(0);
}

void MainWindow::on_reset2_clicked()
{
    ui->openGLWidget->changeHomeworkState(1);
}

void MainWindow::on_keSpin_valueChanged(double arg1)
{
    ui->openGLWidget->change_ke((float)arg1);
}

void MainWindow::on_doubleSpinBox_2_valueChanged(double arg1)
{
    ui->openGLWidget->change_kd((float)arg1);
}

void MainWindow::on_keBSpin_2_valueChanged(double arg1)
{
    ui->openGLWidget->change_keB((float)arg1);
}

void MainWindow::on_kdBSpin_3_valueChanged(double arg1)
{
     ui->openGLWidget->change_kdB((float)arg1);
}
