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
    MultimeterAdapter(parent),
    m_samplesChannel1(UNIT_UNKNOWN),
    m_samplesChannel2(UNIT_UNKNOWN)
{
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

const MultimeterAdapter::ReadingsList
VoltcraftGDM70x::getCurrentReadings()
{
  ReadingsList readings;

  readings.append(ReadingsList::value_type(toMMGUIUnit(vc->data1.unit),
                            toValue(vc->data1)));

  readings.append(ReadingsList::value_type(toMMGUIUnit(vc->data2.unit),
                            toValue(vc->data2)));

  return readings;
}

/*
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
*/

qreal
VoltcraftGDM70x::toValue(const vc_gdm70x_data &data)
{
  qreal mult = 0;

  switch (data.mult) {
  case MEGA : mult = 1e6;  break;
  case KILO : mult = 1e3;  break;
  case NONE : mult = 1;    break;
  case MILLI: mult = 1e-3; break;
  case MICRO: mult = 1e-6; break;
  case NANO:  mult = 1e-9; break;
  }

  return data.value * mult;
}

void
VoltcraftGDM70x::readyRead()
{
  const quint64 time = QDateTime::currentMSecsSinceEpoch();

  vc_gdm70x_do(this->vc, 1);

  if(isSampling())
  {
      if (vc->data1.unit != UNKNOWN)
      {
        if (UNIT_UNKNOWN == m_samplesChannel1.unit())
          m_samplesChannel1 = createSampleSeries(toMMGUIUnit(vc->data1.unit));

        m_samplesChannel1.addSample(QPointF((time - m_samplingStartTimestamp) / 1000., toValue(vc->data1)));
      }

      if (vc->data2.unit != UNKNOWN)
      {
          if (UNIT_UNKNOWN == m_samplesChannel2.unit())
          m_samplesChannel2 = createSampleSeries(toMMGUIUnit(vc->data2.unit));

        m_samplesChannel2.addSample(QPointF((time - m_samplingStartTimestamp) / 1000., toValue(vc->data2)));
      }
  }

  emit dataChanged();

}

void VoltcraftGDM70x::resetSamples()
{
    m_samplesChannel1 = SampleSeries(UNIT_UNKNOWN);
    m_samplesChannel2 = SampleSeries(UNIT_UNKNOWN);

    MultimeterAdapter::resetSamples();
}

void VoltcraftGDM70x::startSampling()
{
  MultimeterAdapter::startSampling();
  m_samplingStartTimestamp = QDateTime::currentMSecsSinceEpoch();
}

SampleUnit
VoltcraftGDM70x::toMMGUIUnit(vc_unit unit)
{
  static const SampleUnit s_VcUnit2MMGUIUnit[] =
  {
    UNIT_UNKNOWN,
    UNIT_VOLT_AC,
    UNIT_VOLT_DC,
    UNIT_AMPERE_AC,
    UNIT_AMPERE_DC,
    UNIT_OHM,
    UNIT_FARAD,
    UNIT_HERZ,
    UNIT_VOLT_DC,
    UNIT_VOLT_DC,
    UNIT_TEMP_C,
    UNIT_TEMP_F,
    UNIT_RH,
    UNIT_PASCAL,
    UNIT_PSI
  };

  return s_VcUnit2MMGUIUnit[unit];
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
