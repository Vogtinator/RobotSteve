#include "examplesdialog.h"
#include "ui_examplesdialog.h"

#include <QXmlStreamReader>
#include <QMessageBox>
#include <QFile>

ExamplesDialog::ExamplesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExamplesDialog)
{
    ui->setupUi(this);

    //Load XML
    QFile file_xml{":/examples/Examples.xml"};
    if(!file_xml.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, trUtf8("Fehler beim Laden"), trUtf8("Die Beispiele konnten nicht geladen werden!"));
        return;
    }

    QXmlStreamReader file_reader{&file_xml};
    bool error = false;
    Example example;
    while(!file_reader.atEnd())
    {
        if(file_reader.readNext() == QXmlStreamReader::Invalid)
        {
            error = true;
            break;
        }

        if(file_reader.isEndElement() && file_reader.name().compare("examples", Qt::CaseInsensitive) == 0)
            break;

        if(file_reader.isEndElement() && file_reader.name().compare("example", Qt::CaseInsensitive) == 0)
        {
            examples.push_back(example);
            ui->examplesList->addItem(example.name);
        }
        else if(file_reader.isStartElement() && file_reader.name().compare("example", Qt::CaseInsensitive) == 0)
        {
            example.name = file_reader.attributes().value("name").toString();
            example.filename = file_reader.attributes().value("filename").toString();
            if(example.name.isEmpty() || example.filename.isEmpty())
            {
                error = true;
                break;
            }

            example.description.clear();
        }
        else if(file_reader.isStartElement() && file_reader.name().compare("description", Qt::CaseInsensitive) == 0)
            example.description = file_reader.readElementText(QXmlStreamReader::IncludeChildElements);
    }

    if(error)
        QMessageBox::critical(this, trUtf8("Fehler beim Laden"), trUtf8("Die Beispiele konnten nicht geladen werden!"));

    connect(ui->examplesList, SIGNAL(clicked(QModelIndex)), this, SLOT(exampleClicked(QModelIndex)));
    connect(ui->examplesList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(exampleDoubleClicked(QModelIndex)));
}

ExamplesDialog::~ExamplesDialog()
{
    delete ui;
}

void ExamplesDialog::exampleClicked(QModelIndex index)
{
    Q_UNUSED(index);

    const Example *example{0};
    for(auto& i : examples)
    {
        if(i.name == ui->examplesList->currentItem()->text())
            example = &i;
    }

    if(example == 0)
        return;

    ui->titleLabel->setText(example->name);
    ui->descriptionLabel->setText(example->description);
}

void ExamplesDialog::exampleDoubleClicked(QModelIndex index)
{
    Q_UNUSED(index);

    const Example *example{0};
    for(auto& i : examples)
    {
        if(i.name == ui->examplesList->currentItem()->text())
            example = &i;
    }

    if(example == 0)
        return;

    emit exampleChosen(example->name, example->filename);

    this->close();
}

void ExamplesDialog::accept()
{
    exampleDoubleClicked(ui->examplesList->currentIndex());
    this->close();
}
