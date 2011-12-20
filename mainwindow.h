#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

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
	void generate();
	void save();
private:
	Ui::MainWindow *ui;
	QString dimmersPath;
	QString lightningsPath;
	QString clipartPath;
	QSettings settings;
	QPixmap currentImage;
	QString lastSelectedDir;
	QString lastSaveDir;
	QList<QImage> dimmers;
	QList<QImage> lightnings;
	QList<QImage> cliparts;
};

#endif // MAINWINDOW_H
