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
#include "multimeteradapter.h"

#include <QDateTime>

MultimeterAdapter::MultimeterAdapter(QObject *parent) :
    QObject(parent),
    m_samplingActive(false)
{
}


bool
MultimeterAdapter::isSampling() const
{
  return m_samplingActive;
}

void
MultimeterAdapter::resetSamples()
{
  m_sampleSeriesList.clear();

  emit sampleSeriesReset();
}

void MultimeterAdapter::startSampling()
{
  if (isSampling())
    return;

  resetSamples();

  m_samplingActive = true;
}

void MultimeterAdapter::stopSampling()
{
  m_samplingActive = false;
}

MultimeterAdapterDescriptorList &
MultimeterAdapter::getAdapterDescriptorList()
{
  static MultimeterAdapterDescriptorList list;
  return list;
}

MultimeterAdapter*
MultimeterAdapter::createAdapter(const QString uri)
{
  MultimeterAdapterDescriptorList & list = getAdapterDescriptorList();

  for(int i = 0; i < list.count(); i++)
  {
    if(list.at(i)->getUri() == uri)
      return list.at(i)->createAdapter();
  }

  return NULL;
}

SampleSeries
MultimeterAdapter::createSampleSeries(SampleUnit unit)
{
  SampleSeries series(unit);

  m_sampleSeriesList.append(series);

  emit sampleSeriesAdded(m_sampleSeriesList.count() - 1);

  return series;
}

#include "multimeteradapter.moc"

