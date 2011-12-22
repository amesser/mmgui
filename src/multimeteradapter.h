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
#ifndef MULTIMETERADAPTER_H
#define MULTIMETERADAPTER_H

#include "sampleseries.h"

#include <QObject>
#include <QVector>
#include <QPointF>
#include <QPair>
#include <QStringList>


class MultimeterAdapter;

class MultimeterAdapterDescriptor
{
public:
  MultimeterAdapterDescriptor(const QString & name, const QString & uri) :
    name(name),
    uri(uri)
  {

  }

  virtual MultimeterAdapter* createAdapter() const = 0;

  const QString & getName() const {return name;}
  const QString & getUri() const  {return uri;}

private:
  QString name;
  QString uri;
};

typedef QList<MultimeterAdapterDescriptor*> MultimeterAdapterDescriptorList;

class MultimeterAdapter : public QObject
{
  Q_OBJECT
public:
  explicit MultimeterAdapter(QObject *parent = 0);

  const SampleSeries getSampleSeries(int index) const {return m_sampleSeriesList.at(index); }

  typedef QVector<QPair<SampleUnit, qreal> > ReadingsList;

  virtual const ReadingsList getCurrentReadings() = 0;

  static QStringList                        findDevices();

  static MultimeterAdapter *                createAdapter(const QString uri);
  static MultimeterAdapterDescriptorList &  getAdapterDescriptorList();

  bool isSampling() const;

protected:
  SampleSeries createSampleSeries(enum SampleUnit unit);

signals:
  void dataChanged();

  void sampleSeriesAdded(int index);

  void sampleSeriesReset();

public slots:
  virtual void resetSamples();
  virtual void startSampling();
  virtual void stopSampling();

private:
  QList<SampleSeries>     m_sampleSeriesList;
  bool                     m_samplingActive;
};

template<class T>
class MultimeterAdapterRegistrar
{
public:
  MultimeterAdapterRegistrar(const QString & name, const QString & id) :
    id(id),
    name(name)
  {
      QStringList devices = T::findDevices();

      while(!devices.isEmpty())
          addDevice(devices.takeFirst());
  }

  void addDevice(const QString & device)
  {
    QString uri = id + ":" + device;

    MultimeterAdapterDescriptorList & list = MultimeterAdapter::getAdapterDescriptorList();

    for(int i = 0; i < list.count(); i++)
    {
      if (list.at(i)->getUri() == uri)
        return;
    }

    list.append(new Descriptor(
                  name + " [" + device + "]",
                  uri));
  }

private:
  const QString id;
  const QString name;

  class Descriptor : public MultimeterAdapterDescriptor
  {
  public:
    Descriptor(const QString & name, const QString & uri) :
      MultimeterAdapterDescriptor(name,uri) {}

    virtual MultimeterAdapter* createAdapter() const
    {
      const QString & uri = getUri();
      int pos = uri.indexOf(':');

      if (pos < 0)
        return NULL;

      return new T(uri.mid(pos+1));
    }
  };
};

#endif // MULTIMETERADAPTER_H
