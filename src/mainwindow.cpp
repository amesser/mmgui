/*
mmgui - A graphical capture and read aout applicatin for digital
        multimeters

Copyright (C) 2011  Andreas Messer <andi@bastelmap.de>

mmgui is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "multimeterexportwizard.h"

#include <QLabel>

#include <QBrush>
#include <QPen>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
     adapter(NULL)
{
    ui->setupUi(this);

    refreshAdapters();

    changeAdapter();

    connect(ui->deviceSelector,SIGNAL(currentIndexChanged(int)),
            this, SLOT(changeAdapter()));

    connect(ui->exportButton,SIGNAL(clicked()),
            this,SLOT(exportSamples()));
}

MainWindow::~MainWindow()
{
  delete ui;

  if(adapter)
    delete adapter;
}

const QColor &
MainWindow::getColor(SampleUnit unit)
{
    static const QColor s_colors[UNIT_RESERVED_MAX] = {
      QColor(Qt::black),
      QColor(Qt::darkRed),
      QColor(Qt::darkRed),
      QColor(Qt::darkBlue),
      QColor(Qt::darkBlue),
      QColor(Qt::darkGreen),
      QColor(238,118,0),
      QColor(Qt::darkYellow),
      QColor(Qt::darkMagenta),
      QColor(Qt::darkMagenta),
      QColor(165,42,42),
      QColor(Qt::darkCyan),
      QColor(Qt::darkCyan),
    };

    return s_colors[unit];
}

void MainWindow::changeAdapter()
{
  ui->plot->detachItems(QwtPlotItem::Rtti_PlotCurve);

  QList<QLabel *> labels = ui->currentReadings->findChildren<QLabel*>("reading");

  while( labels.count() > 0)
    delete labels.takeFirst();

  disconnect(this,SLOT(refreshData()));
  disconnect(this,SLOT(sampleSeriesAdded(int)));
  disconnect(this,SLOT(sampleSeriesReset()));

  if (adapter)
  {
      disconnect(adapter,SLOT(startSampling()));
      disconnect(adapter,SLOT(stopSampling()));

    delete adapter;
    adapter = NULL;
  }


  if (ui->deviceSelector->currentIndex() > -1)
  {
    try
    {
      adapter = MultimeterAdapter::createAdapter(ui->deviceSelector->itemData(ui->deviceSelector->currentIndex()).toString());
    }
    catch (...)
    {
      ;
    }
  }

  if (not adapter)
    return;

  /*
  for(int i = 0; i < adapter->getChannelCount(); i++)
  {
    QLabel *label = new QLabel();
    label->setObjectName("reading");
    ui->currentReadings->layout()->addWidget(label);

    QPalette palette;
    palette.setColor(QPalette::WindowText, getChannelColor(i));

    label->setPalette(palette);

    QwtPlotCurve *curve = new QwtPlotCurve();

    curve->setData(new QwtPointSeriesData(adapter->getSamplesList(i)));

    if (i == 0)
      curve->setYAxis(QwtPlot::yLeft);
    else
      curve->setYAxis(QwtPlot::yRight);

    curve->setPen(QPen(getChannelColor(i)));

    ui->plot->enableAxis(curve->yAxis());

    curve->attach(ui->plot);
  }
*/
  refreshData();

  connect(adapter,SIGNAL(sampleSeriesAdded(int)),
          this, SLOT(sampleSeriesAdded(int)));

  connect(adapter,SIGNAL(sampleSeriesReset()),
          this, SLOT(sampleSeriesReset()));

  connect(adapter,SIGNAL(dataChanged()),
          this, SLOT(refreshData()));

  connect(ui->startButton, SIGNAL(clicked()),
          adapter,SLOT(startSampling()));

  connect(ui->stopButton, SIGNAL(clicked()),
          adapter, SLOT(stopSampling()));
}

void MainWindow::refreshData()
{
  MultimeterAdapter::ReadingsList readings = adapter->getCurrentReadings();

  QList<QLabel *> labels = ui->currentReadings->findChildren<QLabel*>("reading");

  while(readings.count() > labels.count())
  {
    QLabel *label = new QLabel();
    label->setObjectName("reading");
    ui->currentReadings->layout()->addWidget(label);
    labels.append(label);
  }

  while(readings.count() < labels.count())
  {
    delete labels.takeLast();
  }

  for (int i = 0; i < labels.count(); i++)
  {
      QLabel* label = labels.at(i);

      label->setText(QString("%1 %2").arg(readings.at(i).second,5,'f',4).arg(SampleSeries::toString(readings.at(i).first)));

      QPalette palette;

      palette.setColor(QPalette::WindowText, getColor(readings.at(i).first));
      label->setPalette(palette);
  }

  ui->plot->replot();
}

void MainWindow::refreshAdapters()
{
  ui->deviceSelector->clear();

  MultimeterAdapterDescriptorList & adapters = MultimeterAdapter::getAdapterDescriptorList();

  for(int i = 0; i <  adapters.count(); i++)
  {
    ui->deviceSelector->addItem(adapters.at(i)->getName(), adapters.at(i)->getUri());
  }
}



void
MainWindow::sampleSeriesAdded(int index)
{
  SampleSeries series = adapter->getSampleSeries(index);

  QwtPlotCurve *curve = new QwtPlotCurve();

  curve->setData(new SampleSeries(series));
  curve->setTitle(SampleSeries::toString(series.unit()));

  QPen pen(getColor(series.unit()));
  pen.setWidth(3);

  curve->setPen(pen);

  ui->plot->enableAxis(curve->yAxis());
  curve->attach(ui->plot);
}

void
MainWindow::sampleSeriesReset()
{
  ui->plot->detachItems();
}

void
MainWindow::exportSamples()
{
    if (adapter)
    {
        MultimeterExportWizard wizard(adapter,this);
        wizard.exec();
    }
}

#include "mainwindow.moc"
