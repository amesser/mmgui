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
    samplingEnabled(false)
{
}

int
MultimeterAdapter::getChannelCount() const
{
  return samples.count();
}


const MultimeterAdapter::SampleList &
MultimeterAdapter::getSamplesList(int channel) const
{
  return samples.at(channel);
}

bool
MultimeterAdapter::isSampling() const
{
  return samplingEnabled;
}

void
MultimeterAdapter::resetSamples()
{
  for(int i = 0; i < samples.count(); i++)
    samples[i].clear();

  emit dataChanged();
}

void MultimeterAdapter::startSampling()
{
  if (isSampling())
    return;

  resetSamples();

  timeSamplingStarted = QDateTime::currentMSecsSinceEpoch();
  samplingEnabled     = true;
}

void MultimeterAdapter::stopSampling()
{
  samplingEnabled = false;
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

void
MultimeterAdapter::addSample(int channel, quint64 time, qreal value)
{
  if (!isSampling())
    return;

  samples[channel].append(QPointF(
    (time - timeSamplingStarted) / 1000.0,
    value));
}

void
MultimeterAdapter::setChannelCount(int count)
{
  stopSampling();

  samples.clear();

  while(count--)
    samples.append(SampleList());
}

#include "multimeteradapter.moc"

