#include "multimeterexportwizard.h"
#include "ui_multimeterexportwizard.h"
#include "QFileDialog"

MultimeterExportWizard::MultimeterExportWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::MultimeterExportWizard)
{
    ui->setupUi(this);

    /* We want to re-use the existing QFileDialog.
     * First, we must select the native qt dialog for non-linux systems.
     * Second, we delete the standard open/cancel buttons */
    m_file_dialog = new QFileDialog;
    m_file_dialog->setOption(QFileDialog::DontUseNativeDialog);
    delete m_file_dialog->findChild<QWidget*>("buttonBox");

    /* add the file dialog to the wizard */
    QLayout *layout = new QVBoxLayout;
    layout->addWidget(m_file_dialog);
    ui->page_select_file->setLayout(layout);
}

MultimeterExportWizard::~MultimeterExportWizard()
{
    delete ui;
}

#include "multimeterexportwizard.moc"
