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
#include "voltcraftgdm70x.h"

#include <QDateTime>
#include <QDir>

using namespace std;

VoltcraftGDM70x::VoltcraftGDM70x(const QString & uri, QObject *parent) :
    MultimeterAdapter(parent)
{
  setChannelCount(2);

  vc = vc_gdm70x_create();

  try {
    if (not vc)
      throw "Failed to create vc";

    qWarning(uri.toAscii());
    if (vc_gdm70x_open(vc,uri.toAscii()))
      throw "Failed to open device";
  }
  catch (...)
  {
    vc_gdm70x_destroy(vc);

    throw;
  }

  QSocketNotifier *notifier = new QSocketNotifier(vc->fd, QSocketNotifier::Read, this);

  connect(notifier, SIGNAL(activated(int)),
          this,SLOT(readyRead()));

}

VoltcraftGDM70x::~VoltcraftGDM70x()
{
  vc_gdm70x_destroy(vc);
}

const QString &
VoltcraftGDM70x::getUnit(enum vc_unit u)
{
  static const QString units[] =
  {
    QString("Unknown"),
    QString("V AC"),
    QString("V DC"),
    QString("A AC"),
    QString("A DC"),
    QString("OHM"),
    QString("F"),
    QString("Hz"),
    QString("Logic"),
    QString("Diode"),
    QString("°C"),
    QString("°F"),
    QString("Rh"),
    QString("Pa"),
    QString("Psi"),
  };

  return units[u];
}

QString
VoltcraftGDM70x::getChannelReading(int channel) const
{
  static const QString format("%1 %2 %3");
  struct vc_gdm70x_data* data = (channel == 0) ? &(vc->data1) : &(vc->data2);


  return format.arg(data->value,5,'f',4).arg((char)(data->mult)).arg(getUnit(data->unit));
}

QString
VoltcraftGDM70x::getSamplesUnit(int channel) const
{
  if (channel == 0)
    return getUnit(vc->data1.unit);
  else
    return getUnit(vc->data2.unit);
}

void
VoltcraftGDM70x::readyRead()
{
  const quint64 time = QDateTime::currentMSecsSinceEpoch();

  vc_gdm70x_do(this->vc, 1);

  qreal channel1 = 0,channel2 = 0;

  switch (vc->data1.mult) {
  case MEGA : channel1 = 1e6; break;
  case KILO : channel1 = 1e3; break;
  case NONE : channel1 = 1;   break;
  case MILLI: channel1 = 1e-3;break;
  case MICRO: channel1 = 1e-6;break;
  case NANO: channel1 = 1e-9; break;
  }

  if (vc->data1.unit != UNKNOWN)
  {
    addSample(0,time,channel1 * vc->data1.value);
  }

  switch (vc->data2.mult) {
  case MEGA : channel2 = 1e6; break;
  case KILO : channel2 = 1e3; break;
  case NONE : channel2 = 1;   break;
  case MILLI: channel2 = 1e-3;break;
  case MICRO: channel2 = 1e-6;break;
  case NANO:  channel2 = 1e-9; break;
  }

  if (vc->data2.unit != UNKNOWN)
  {
    addSample(1,time,channel2 * vc->data2.value);
  }

  emit dataChanged();

}

QStringList
VoltcraftGDM70x::findDevices()
{
    QStringList filters;

    filters << "ttyS*"
            << "ttyUSB*";

    QFileInfoList list = QDir("/dev").entryInfoList(filters,
                                                    QDir::System | QDir::Readable | QDir::Writable | QDir::Files,
                                                    QDir::Name);

    QStringList devices;

    for(int i = 0; i < list.count(); i++)
        devices.append(list.at(i).absoluteFilePath());

    return devices;
}


MultimeterAdapterRegistrar<VoltcraftGDM70x> registrar("Voltcraft GDM 703/704/705", "vc_gdm70x");

#include "voltcraftgdm70x.moc"
