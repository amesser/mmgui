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
#include "sampleseries.h"

SampleSeriesPrivateData::SampleSeriesPrivateData(SampleUnit unit):
    QwtPointSeriesData(),
    m_unit(unit),
    m_refcount(0)
{

}

SampleSeries::SampleSeries(SampleUnit unit):
    QwtSeriesData<QPointF>()
{
    m_priv = new SampleSeriesPrivateData(unit);
    m_priv->m_refcount++;
}

SampleSeries::SampleSeries(const SampleSeries &orig) :
    QwtSeriesData<QPointF>()
{
    m_priv = orig.m_priv;
    m_priv->m_refcount++;
}

SampleSeries::~SampleSeries()
{
    m_priv->m_refcount--;

    if (m_priv->m_refcount == 0)
        delete m_priv;
}

const SampleSeries &
SampleSeries::operator =(const SampleSeries &orig)
{
    m_priv->m_refcount--;

    if (m_priv->m_refcount == 0)
        delete m_priv;

    m_priv = orig.m_priv;
    m_priv->m_refcount++;
}

void
SampleSeries::addSample(QPointF sample)
{
    m_priv->d_samples.append(sample);
    m_priv->d_boundingRect.setWidth(-1);
    m_priv->d_boundingRect.setHeight(-1);
}

const char*
SampleSeries::toString(SampleUnit unit)
{
  static const char* s_units[UNIT_RESERVED_MAX] = {
    "Unkown Unit",
    "V (AC)",
    "V (DC)",
    "A (AC)",
    "A (DC)",
    "Ohm",
    "F",
    "Hz",
    "°C",
    "°F",
    "%Rh",
    "Psi",
    "Pa"
  };

  return s_units[unit];
}
