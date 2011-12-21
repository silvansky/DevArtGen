#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QDateTime>
#include <QPainter>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	settings("Silvansky", "DevArtGen")
{
	ui->setupUi(this);

	connect(ui->actionGen, SIGNAL(triggered()), SLOT(generate()));
	connect(ui->actionSave, SIGNAL(triggered()), SLOT(save()));
	connect(ui->actionReset, SIGNAL(triggered()), SLOT(reset()));

	dimmersPath = settings.value("dimmersPath").toString();
	lightningsPath = settings.value("lightningsPath").toString();
	clipartPath = settings.value("clipartPath").toString();

	lastSelectedDir = settings.value("lastSelectedDir").toString();
	lastSaveDir = settings.value("lastSaveDir").toString();

	qDebug() << dimmersPath << lightningsPath << lastSelectedDir;

	qsrand(QDateTime::currentMSecsSinceEpoch());

	ui->actionSave->setEnabled(false);
	ui->actionReset->setVisible(false);
	ui->result->installEventFilter(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}

bool MainWindow::eventFilter(QObject *obj, QEvent *evt)
{
	if (obj == ui->result)
	{
		if (evt->type() == QEvent::DragEnter)
		{
			qDebug() << "drag enter...";
			QDragEnterEvent * dee = (QDragEnterEvent*)(evt);
			if (dee->mimeData()->hasUrls() && (dee->mimeData()->urls().count() == 1))
			{
				dee->accept();
			}
		}
		else if (evt->type() == QEvent::Drop)
		{
			QDropEvent * drop = (QDropEvent*)evt;
			if (drop->mimeData()->hasUrls())
			{
				if (drop->mimeData()->urls().count() > 1)
				{
					qDebug() << "many files! can\'t handle!";
				}
				else
				{
					QString file = drop->mimeData()->urls().first().toLocalFile();
					qDebug() << "got file:" << file;
					QFileInfo finfo(file);
					if (finfo.isDir())
					{
						qDebug() << "can't handle dir!";
					}
					else
					{
						if (finfo.size())
						{
							// load file
							if (background.load(file))
							{
								drop->accept();
								ui->actionReset->setVisible(true);
							}
							else
								qDebug() << "wrong file format!";
						}
						else
						{
							qDebug() << "file is empty!";
						}
					}
				}
			}
		}
	}
	return QMainWindow::eventFilter(obj, evt);
}

void MainWindow::generate()
{
	if (dimmers.isEmpty())
	{
		if (dimmersPath.isEmpty())
		{
			dimmersPath = QFileDialog::getExistingDirectory(this, "Select path with dimmers", lastSelectedDir);
			if (dimmersPath.isEmpty())
				return;
		}
		settings.setValue("dimmersPath", dimmersPath);
		lastSelectedDir = dimmersPath;
		settings.setValue("lastSelectedDir", lastSelectedDir);
		QDir dir(dimmersPath);
		QStringList dimmersList = dir.entryList(QStringList() << "*.png");
		QImage img;
		qDebug() << "loading dimmers...";
		foreach (QString file,  dimmersList)
		{
			qDebug() << dir.absoluteFilePath(file);
			if (img.load(dir.absoluteFilePath(file)))
			{
				dimmers << img;
			}
		}
		if (dimmers.isEmpty())
		{
			generate();
			return;
		}
	}
	if (lightnings.isEmpty())
	{
		if (lightningsPath.isEmpty())
		{
			lightningsPath = QFileDialog::getExistingDirectory(this, "Select path with lightnings", lastSelectedDir);
			if (lightningsPath.isEmpty())
				return;
		}
		settings.setValue("lightningsPath", lightningsPath);
		lastSelectedDir = lightningsPath;
		settings.setValue("lastSelectedDir", lastSelectedDir);
		QDir dir(lightningsPath);
		QStringList lightningsList = dir.entryList(QStringList() << "*.png");
		QImage img;
		qDebug() << "loading lightnings...";
		foreach (QString file,  lightningsList)
		{
			qDebug() << dir.absoluteFilePath(file);
			if (img.load(dir.absoluteFilePath(file)))
			{
				lightnings << img;
			}
		}
		if (lightnings.isEmpty())
		{
			generate();
			return;
		}
	}
	if (cliparts.isEmpty())
	{
		if (clipartPath.isEmpty())
		{
			clipartPath = QFileDialog::getExistingDirectory(this, "Select path with cliparts", lastSelectedDir);
			if (clipartPath.isEmpty())
				return;
		}
		settings.setValue("clipartPath", clipartPath);
		lastSelectedDir = clipartPath;
		settings.setValue("lastSelectedDir", lastSelectedDir);
		QDir dir(clipartPath);
		QStringList lightningsList = dir.entryList(QStringList() << "*.png");
		QImage img;
		qDebug() << "loading cliparts...";
		foreach (QString file,  lightningsList)
		{
			qDebug() << dir.absoluteFilePath(file);
			if (img.load(dir.absoluteFilePath(file)))
			{
				cliparts << img;
			}
		}
		if (cliparts.isEmpty())
		{
			generate();
			return;
		}
	}
	// transparent canvas
	QPixmap canvas(640, 480);
	canvas.fill(QColor(0, 0, 0, 0).rgba());
	QPainter p(&canvas);
	p.setRenderHint(QPainter::Antialiasing);

	p.save();

	int parts = 1000;

	// background
	if (!background.isNull())
	{
		parts = 50;
		p.drawImage(0, 0, background);
	}

	// draw some random shapes with random colors
	QColor c;

	// let's get the common tone of the picture...
	QColor tone(qrand()%256, qrand()%256, qrand()%256);

	for (int i = 0; i < qrand()%parts+parts; i++)
	{
		// each color is tone.color +- 20
		c.setRgb(tone.red() + abs(qrand()%40-20)%256, tone.green() + abs(qrand()%40-20)%256, tone.blue() + abs(qrand()%40-20)%256, qrand()%156+100);
		p.setPen(QPen(c, qrand()%4 + 1));
		p.setBrush(c);
		p.translate(320, 240);
		p.rotate((qrand()%360) / 2.0);
		p.translate(-320, -240);
		switch (qrand()%5)
		{
		case 0:
			{
				// figure
				QPainterPath path(QPoint(qrand()%740-50, qrand()%580-50));
				for (int j = 0; j < qrand()%10; j++)
				{
					switch(rand()%2)
					{
					case 0:
						path.lineTo(qrand()%740-50, qrand()%580-50);
						break;
					case 1:
						path.cubicTo(qrand()%740-50, qrand()%580-50, qrand()%740-50, qrand()%580-50, qrand()%740-50, qrand()%580-50);
						break;
					}
				}
				p.drawPath(path);
				break;
			}
		case 1:
			{
				// big triangle
				QPolygon triangle;
				triangle.append(QPoint(qrand()%740-50, qrand()%580-50));
				triangle.append(QPoint(qrand()%740-50, qrand()%580-50));
				triangle.append(QPoint(qrand()%740-50, qrand()%580-50));
				p.drawPolygon(triangle);
				break;
			}
		case 2:
			{
				// small triangle
				QPolygon triangle;
				QPoint fp(qrand()%740-50, qrand()%580-50);
				triangle.append(fp);
				fp += QPoint(qrand()%100-50, qrand()%100-50);
				triangle.append(fp);
				fp += QPoint(qrand()%100-50, qrand()%100-50);
				triangle.append(fp);
				p.drawPolygon(triangle);
				break;
			}
		case 3:
			{
				// rect
				p.drawRect(qrand()%740-50, qrand()%580-50, qrand()%150, qrand()%150);
				break;
			}
		case 4:
			{
				// line
				p.drawLine(QPoint(qrand()%740-50, qrand()%580-50), QPoint(qrand()%740-50, qrand()%580-50));
				break;
			}
		default:
			break;
		}
		if (qrand()%10 > 7)
		{
			p.rotate(0);
			p.setOpacity((qrand()%101) / 100.0 + 0.3);
			p.drawImage(qrand()%740-50, qrand()%580-50, cliparts.at(qrand()%cliparts.count()));
		}
	}

	p.restore();

	// dimmer and lightning
	if (rand()%2)
	{
		int n = qrand() % dimmers.count();
		p.setOpacity((qrand()%101) / 100.0 + 0.3);
		p.drawImage(0, 0, dimmers.at(n));
		n = qrand() % lightnings.count();
		p.setOpacity((qrand()%101) / 100.0 + 0.3);
		p.drawImage(0, 0, lightnings.at(n));
	}
	else
	{
		int n = qrand() % lightnings.count();
		p.setOpacity((qrand()%101) / 100.0 + 0.3);
		p.drawImage(0, 0, lightnings.at(n));
		n = qrand() % dimmers.count();
		p.setOpacity((qrand()%101) / 100.0 + 0.3);
		p.drawImage(0, 0, dimmers.at(n));
	}

	p.setPen(QColor(255, 255, 255, 150));
	p.setBrush(QColor(255, 255, 255, 150));
	QFont f("Anonymous Pro", 12);
	p.setFont(f);
	QFontMetrics fm(f);
	QString copyright = QChar(0x00A9) + QString(" %1 vtualetius").arg(QDateTime::currentDateTime().date().year());
	QRect br = fm.boundingRect(copyright);
	p.drawText(630 - br.width(), 480 - br.height(), copyright);
	p.end();
	ui->result->setPixmap(canvas);
	currentImage = canvas;
	ui->actionSave->setEnabled(true);
}

void MainWindow::save()
{
	if (!currentImage.isNull())
	{
		QString file = QFileDialog::getSaveFileName(this, "Save file", lastSaveDir, "*.png");
		if (!file.isEmpty())
		{
			currentImage.save(file);
			QFileInfo finfo(file);
			lastSaveDir = finfo.absolutePath();
			settings.setValue("lastSaveDir", lastSaveDir);
		}
	}
}

void MainWindow::reset()
{
	background = QImage();
	ui->actionReset->setVisible(false);
}
