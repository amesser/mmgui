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
#ifndef MULTIMETEREXPORTWIZARD_H
#define MULTIMETEREXPORTWIZARD_H

#include <QWizard>
#include <QFileDialog>
#include <multimeteradapter.h>

namespace Ui {
class MultimeterExportWizard;
}

class MultimeterExportWizard : public QWizard
{
    Q_OBJECT
    
public:
    explicit MultimeterExportWizard(MultimeterAdapter* adapter, QWidget *parent = 0);
    ~MultimeterExportWizard();

    virtual void done(int result);

public slots:
    void refreshPreview();

private:
    Ui::MultimeterExportWizard *ui;
    QFileDialog                *m_file_dialog;
    MultimeterAdapter          *m_adapter;

};

#endif // MULTIMETEREXPORTWIZARD_H
