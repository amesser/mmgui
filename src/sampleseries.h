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
#ifndef SAMPLESERIES_H
#define SAMPLESERIES_H

#include <QPointF>
#include <qwt_series_data.h>

enum SampleUnit {
    UNIT_UNKNOWN = 0,
    UNIT_VOLT_AC,
    UNIT_VOLT_DC,
    UNIT_AMPERE_AC,
    UNIT_AMPERE_DC,
    UNIT_OHM,
    UNIT_FARAD,
    UNIT_HERZ,
    UNIT_TEMP_C,
    UNIT_TEMP_F,
    UNIT_RH,
    UNIT_PSI,
    UNIT_PASCAL,
    UNIT_RESERVED_MAX
};

class SampleSeries;

class SampleSeriesPrivateData : public QwtPointSeriesData
{
public:
    explicit SampleSeriesPrivateData(enum SampleUnit unit);

private:
    int                   m_refcount;
    const enum SampleUnit m_unit;

    friend class SampleSeries;
};

class SampleSeries : public QwtSeriesData<QPointF>
{
public:
    explicit SampleSeries(enum SampleUnit unit);
    SampleSeries(const SampleSeries &orig);

    virtual ~SampleSeries();

    const SampleSeries & operator =(const SampleSeries &orig);

    enum  SampleUnit        unit()    const { return m_priv->m_unit; }

    virtual size_t size() const {return m_priv->size();}
    virtual QPointF sample(size_t i) const {return m_priv->sample(i);}
    virtual QRectF boundingRect() const { return m_priv->boundingRect();}

    void addSample(QPointF sample);

    static const char* toString(SampleUnit unit);
private:
    SampleSeriesPrivateData* m_priv;

};

#endif // SAMPLESERIES_H
