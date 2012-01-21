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
#include "multimeterdataexporter.h"

#include <QString>
#include <QSet>

class MultimeterDataExporterCSV : public MultimeterDataExporter
{
public:
    MultimeterDataExporterCSV() {}

    virtual void write(QIODevice &out, QList<SampleSeries> &datasets);

    static const QString & name();
};

void
MultimeterDataExporterCSV::write(QIODevice &out, QList<SampleSeries> &datasets)
{
    QSet<SampleUnit> units;

    for (int i = 0; i < datasets.count(); i++)
        units.insert(datasets.at(i).unit());

    for (int i = 0; i < units.count(); i++)
    {
        SampleUnit u = units.toList().at(i);

        if (i > 0)
            out.write(QString(",\n").toAscii().data());

        out.write(QString("Time,%1\n").arg(SampleSeries::toString(u)).toAscii().data());

        for (int j = 0; j < datasets.count(); j++)
        {
            const SampleSeries series = datasets.at(j);

            if(series.unit() != u)
                continue;

            for (int k = 0; k < series.size(); k++)
            {
                QPointF sample = series.sample(k);

                out.write( QString("%1,%2\n").arg(sample.x()).arg(sample.y()).toAscii().data());
            }
        }
    }
}

const QString &
MultimeterDataExporterCSV ::name()
{
    static const QString n("Comma separated Values (CSV)");
    return n;
}

MultimeterDataExporterRegistrar<MultimeterDataExporterCSV> CsvRegistrar;
