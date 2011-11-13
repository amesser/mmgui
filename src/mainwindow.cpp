/*
mmgui - A graphical capture and read aout applicatin for digital
        multimeters

Copyright (C) <2011>  Andreas Messer <andi@bastelmap.de>

This program is free software: you can redistribute it and/or modify
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
}

MainWindow::~MainWindow()
{
  delete ui;

  if(adapter)
    delete adapter;
}

const QColor &
MainWindow::getChannelColor(int channel) const
{
    static QColor colors[] =
    {
        QColor(Qt::red),
        QColor(Qt::blue),
    };

    static QColor def(Qt::black);

    if ((unsigned int)channel < sizeof(colors) / sizeof(colors[0]))
        return colors[channel];
    else
        return def;
}
void MainWindow::changeAdapter()
{
  ui->plot->detachItems(QwtPlotItem::Rtti_PlotCurve);

  QList<QLabel *> labels = ui->currentReadings->findChildren<QLabel*>("reading");

  while( labels.count() > 0)
    delete labels.takeFirst();

  disconnect(this,SLOT(refreshData()));

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

  refreshData();

  connect(adapter,SIGNAL(dataChanged()),
          this, SLOT(refreshData()));

  connect(ui->startButton, SIGNAL(clicked()),
          adapter,SLOT(startSampling()));
  connect(ui->stopButton, SIGNAL(clicked()),
          adapter, SLOT(stopSampling()));
}

void MainWindow::refreshData()
{
  QList<QLabel *> labels = ui->currentReadings->findChildren<QLabel*>("reading");

  for (int i = 0; i < labels.count(); i++)
    labels.at(i)->setText(adapter->getChannelReading(i));

  QwtPlotItemList curves = ui->plot->itemList(QwtPlotItem::Rtti_PlotCurve);

  for(int i = 0; i < curves.count(); i++)
  {
    QwtPlotCurve *curve = static_cast<QwtPlotCurve*>(curves[i]);

    curve->setData(new QwtPointSeriesData(adapter->getSamplesList(i)));
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

#include "mainwindow.moc"