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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <qwt_plot_curve.h>
#include "multimeteradapter.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

  static const char* toString(SampleUnit unit);

  static const QColor & getColor(SampleUnit unit);

public slots:
  void changeAdapter();
  void refreshData();
  void refreshAdapters();

  void sampleSeriesAdded(int index);
  void sampleSeriesReset();

  void exportSamples();
private:
    Ui::MainWindow    *ui;
    MultimeterAdapter *adapter;
};

#endif // MAINWINDOW_H
