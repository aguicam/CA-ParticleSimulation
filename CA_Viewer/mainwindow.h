#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void on_action_Quit_triggered();

	void on_checkBoxFill_toggled(bool checked);

	void on_action_Open_triggered();

    void on_resetButton_clicked();

    void on_checkBox_stateChanged(int arg1);

    void on_checkBox_2_stateChanged(int arg1);

    void on_SPX_valueChanged(double arg1);

    void on_SPY_valueChanged(double arg1);

    void on_SPZ_valueChanged(double arg1);

    void on_rbFountain_clicked(bool checked);

    void on_rbWaterfall_clicked(bool checked);

    void on_checkBox_clicked(bool checked);

    void on_checkBox_2_clicked(bool checked);

    void on_tabWidget_currentChanged(int index);

    void on_reset1_clicked();

    void on_reset2_clicked();

    void on_keSpin_valueChanged(double arg1);

    void on_doubleSpinBox_2_valueChanged(double arg1);

    void on_keBSpin_2_valueChanged(double arg1);

    void on_kdBSpin_3_valueChanged(double arg1);

private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
