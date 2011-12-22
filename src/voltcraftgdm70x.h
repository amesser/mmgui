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
#ifndef VOLTCRAFTGDM70X_H
#define VOLTCRAFTGDM70X_H

#include "multimeteradapter.h"
#include "vc-gdm70x.h"

#include <QString>
#include <QSocketNotifier>

class VoltcraftGDM70x : public MultimeterAdapter
{
    Q_OBJECT
public:
  explicit VoltcraftGDM70x(const QString & uri, QObject *parent = 0);
  virtual ~VoltcraftGDM70x();

  static const QString & getUnit(enum vc_unit u);

  static QStringList         findDevices();

  virtual const ReadingsList getCurrentReadings();

public slots:
  void readyRead();

  virtual void resetSamples();
  virtual void startSampling();

private:
  struct vc_gdm70x *vc;

  quint64       m_samplingStartTimestamp;
  SampleSeries  m_samplesChannel1;
  SampleSeries  m_samplesChannel2;

  static SampleUnit toMMGUIUnit(vc_unit unit);
  static qreal      toValue(const struct vc_gdm70x_data & data);
};
#endif // VOLTCRAFTGDM70X_H
