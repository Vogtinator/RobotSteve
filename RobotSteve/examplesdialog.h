#ifndef EXAMPLESDIALOG_H
#define EXAMPLESDIALOG_H

#include <QDialog>
#include <QVector>
#include <QModelIndex>

namespace Ui {
class ExamplesDialog;
}

struct Example {
    QString name;
    QString description;
    QString filename;
};

class ExamplesDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ExamplesDialog(QWidget *parent = 0);
    ~ExamplesDialog();

public slots:
    void exampleClicked(QModelIndex index);
    void exampleDoubleClicked(QModelIndex index);
    void accept() override;

signals:
    void exampleChosen(QString name, QString filename);
    
private:
    Ui::ExamplesDialog *ui;
    QVector<Example> examples;
};

#endif // EXAMPLESDIALOG_H
