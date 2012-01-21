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
#include "multimeterexportwizard.h"
#include "ui_multimeterexportwizard.h"
#include "multimeterdataexporter.h"

#include <QMap>
#include <QBuffer>
#include "QMessageBox"


MultimeterExportWizard::MultimeterExportWizard(MultimeterAdapter *adapter, QWidget *parent) :
    QWizard(parent),
    ui(new Ui::MultimeterExportWizard),
    m_file_dialog(new QFileDialog),
    m_adapter(adapter)
{
    ui->setupUi(this);

    /* We want to re-use the existing QFileDialog.
     * First, we must select the native qt dialog for non-linux systems.
     * Second, we hide the standard open/cancel buttons */
    m_file_dialog->setOption(QFileDialog::DontUseNativeDialog);
    m_file_dialog->findChild<QWidget*>("buttonBox")->hide();

    /* add the file dialog to the wizard */
    QLayout *layout = new QVBoxLayout;
    layout->addWidget(m_file_dialog);
    ui->page_select_file->setLayout(layout);

    {
        const  QList<SampleSeries> & l = adapter->getSampleSeries();

        QMap<SampleUnit, QPair<qreal,qreal> > map_units;

        for (int i = 0; i < l.count(); i++)
        {
            SampleUnit unit = l.at(i).unit();

            if (map_units.contains(unit))
            {
                map_units[unit].first  = qMin(map_units[unit].first,  l.at(i).boundingRect().left());
                map_units[unit].second = qMin(map_units[unit].second, l.at(i).boundingRect().right());
            }
            else
            {
                map_units[unit] = qMakePair(l.at(i).boundingRect().left(),
                                            l.at(i).boundingRect().right());
            }
        }

        for (int i = 0; i < map_units.count(); i++)
        {
            static const QString format("%1 (%2s - %3s)");

            QListWidgetItem *item = new QListWidgetItem(
                format.arg(SampleSeries::toString(map_units.keys().at(i)))
                      .arg(map_units.values().at(i).first)
                      .arg(map_units.values().at(i).second),
                ui->list_datasets);

            item->setData(Qt::UserRole, map_units.keys().at(i));
        }
    }

    {
        MultimeterDataExporterCreator::CreatorList & l = MultimeterDataExporterCreator::list();

        for (int i = 0; i < l.count(); i++)
        {
            QListWidgetItem *item = new QListWidgetItem(
                        l.at(i)->name(), ui->list_formats);

            item->setData(Qt::UserRole, i);
        }
    }

    connect(ui->list_datasets, SIGNAL(itemSelectionChanged()),
            this,SLOT(refreshPreview()));

    connect(ui->list_formats,SIGNAL(itemSelectionChanged()),
            this,SLOT(refreshPreview()));
}

MultimeterExportWizard::~MultimeterExportWizard()
{
    delete ui;
}

void
MultimeterExportWizard::done(int result)
{
    QWizard::done(result);

    if (QDialog::Accepted != result)
        return;

    if( m_file_dialog->selectedFiles().count() != 1)
        return;

    QString filename = m_file_dialog->selectedFiles().at(0);

    QMessageBox::StandardButton overwrite;

    if (QFile::exists(filename))
    {
        overwrite = QMessageBox::warning(this,
                             QString("Confirm Overwrite"),
                             QString("The file %1 already exists! Really Overwrite?").arg(filename),
                             QMessageBox::Yes | QMessageBox::No,
                             QMessageBox::No);
    }
    else
    {
        overwrite = QMessageBox::Yes;
    }

    if(overwrite = QMessageBox::Yes)
    {
        QFile file(filename);

        file.open(QFile::WriteOnly | QFile::Text | QFile::Truncate);

        QListWidgetItem *item = ui->list_formats->currentItem();

        if (not item)
            return;


        QList<SampleSeries> exportsamples;

        for(int i = 0; i < ui->list_datasets->count(); i++)
        {
            QListWidgetItem *item = ui->list_datasets->item(i);

            if (not item->isSelected())
                continue;

            SampleUnit unit = static_cast<SampleUnit>(item->data(Qt::UserRole).toInt());

            const  QList<SampleSeries> & l = m_adapter->getSampleSeries();

            for(int j = 0; j < l.count(); j++)
            {
                if (l.at(j).unit() == unit)
                    exportsamples.append(l.at(j));
            }
        }

        MultimeterDataExporter *exporter = MultimeterDataExporterCreator::list().at(item->data(Qt::UserRole).toInt())->createExporter();

        exporter->write(file,exportsamples);

        delete exporter;
    }


}

void
MultimeterExportWizard::refreshPreview()
{
    QListWidgetItem *item = ui->list_formats->currentItem();

    if (not item)
        return;


    QList<SampleSeries> exportsamples;

    for(int i = 0; i < ui->list_datasets->count(); i++)
    {
        QListWidgetItem *item = ui->list_datasets->item(i);

        if (not item->isSelected())
            continue;

        SampleUnit unit = static_cast<SampleUnit>(item->data(Qt::UserRole).toInt());

        const  QList<SampleSeries> & l = m_adapter->getSampleSeries();

        for(int j = 0; j < l.count(); j++)
        {
            if (l.at(j).unit() == unit)
                exportsamples.append(l.at(j));
        }
    }

    MultimeterDataExporter *exporter = MultimeterDataExporterCreator::list().at(item->data(Qt::UserRole).toInt())->createExporter();
    QBuffer buf;

    buf.open(QBuffer::WriteOnly);

    exporter->write(buf,exportsamples);

    delete exporter;

    ui->text_preview->setPlainText(buf.buffer());
}

#include "multimeterexportwizard.moc"
