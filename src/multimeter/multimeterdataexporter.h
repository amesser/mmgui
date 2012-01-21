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
#ifndef MULTIMETERDATAEXPORTER_H
#define MULTIMETERDATAEXPORTER_H

#include <QList>
#include <QIODevice>
#include <sampleseries.h>

class MultimeterDataExporter
{
public:
    MultimeterDataExporter();

    virtual void write(QIODevice &out, QList<SampleSeries> & datasets) = 0;
};

class MultimeterDataExporterCreator
{
public:
    MultimeterDataExporterCreator();
    virtual ~MultimeterDataExporterCreator();

    typedef QList<MultimeterDataExporterCreator*> CreatorList;

    static CreatorList & list();

    virtual MultimeterDataExporter* createExporter() const = 0;
    virtual const QString &         name() const           = 0;
};

template<typename T>
class MultimeterDataExporterRegistrar : public MultimeterDataExporterCreator
{
public:
    MultimeterDataExporterRegistrar() {}
    virtual const QString & name() const {return T::name();}
    virtual MultimeterDataExporter * createExporter() const { return new T;}
};

#endif // MULTIMETERDATAEXPORTER_H
