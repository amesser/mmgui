#ifndef MULTIMETEREXPORTWIZARD_H
#define MULTIMETEREXPORTWIZARD_H

#include <QWizard>
#include <QFileDialog>

namespace Ui {
class MultimeterExportWizard;
}

class MultimeterExportWizard : public QWizard
{
    Q_OBJECT
    
public:
    explicit MultimeterExportWizard(QWidget *parent = 0);
    ~MultimeterExportWizard();
    
private:
    Ui::MultimeterExportWizard *ui;
    QFileDialog                *m_file_dialog;
};

#endif // MULTIMETEREXPORTWIZARD_H
