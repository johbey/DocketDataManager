#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLabel>
#include <QLineEdit>
#include <QFile>
#include <QTextDocument>
#include <QtPrintSupport/QPrinter>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QDateEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QScopeGuard>
#include <QDesktopServices>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::printAndSaveDocket);
    connectDb();
    prepareLayout();

    auto* version = new QLabel("v001");
    version->setStyleSheet("font-size: 8pt;");
    ui->statusbar->addPermanentWidget(version, 1);
    auto* label = new QLabel("© github.com/johbey");
    label->setStyleSheet("font-size: 8pt;");
    ui->statusbar->addPermanentWidget(label);

    ui->gridLayoutProperties->setColumnStretch(0, 2);
    ui->gridLayoutProperties->setColumnStretch(1, 1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connectDb()
{
    m_db = QSqlDatabase::addDatabase("QSQLITE","SQLITE");

    //const QString basePath = QStandardPaths::writableLocation(QStandardPaths::);
    m_db.setDatabaseName("database.sqlite");
    if (!m_db.open())
    {
        QMessageBox::critical(this, tr("Error"), tr("Can not connect to the database! Error: %1. Please retry later.").arg(m_db.lastError().text()));
        exit(1);
    }
}

bool MainWindow::execQuery(QSqlQuery* query, const QString& sql)
{
    if (!query->exec(sql))
    {
        QMessageBox::critical(this, tr("Error"), tr("SQL Error: %1").arg(query->lastError().text()));
        return false;
    }
    return true;
}

QStringList MainWindow::extractArguments(const QString& text)
{
    const int first = text.indexOf("(");
    const int last = text.lastIndexOf(")");
    QString result;
    if (first != -1
            && last != -1)
    {
        result = text.mid(first+1, last-first-1);
        qDebug() << first << last << result;
    }
    return result.split(";");
}

void MainWindow::createLineEditProperty(Property* currentProperty, bool first, int row, const QString type)
{
    const QString defaultValue = extractArguments(type).first();

    QLineEdit* lineEdit = new QLineEdit(ui->propertiesWidget);
    if (first)
    {
        lineEdit->setFocus();
    }
    lineEdit->setText(defaultValue);
    ui->gridLayoutProperties->addWidget(lineEdit,row,1);
    currentProperty->lineEdit = lineEdit;
}

void MainWindow::createDateEditProperty(Property* currentProperty, bool first, int row, const QString type)
{
    const QString defaultValue = extractArguments(type).first();
    QDate defaultDate(QDate::currentDate());
    if (!defaultValue.isEmpty())
    {
        defaultDate = QDate::fromString(defaultValue, Qt::ISODate);
    }

    QDateEdit* dateEdit = new QDateEdit(ui->propertiesWidget);
    if (first)
    {
        dateEdit->setFocus();
    }
    dateEdit->setDate(defaultDate);
    dateEdit->setCalendarPopup(true);
    ui->gridLayoutProperties->addWidget(dateEdit,row,1);
    currentProperty->dateEdit = dateEdit;
}

void MainWindow::createSpinBoxProperty(Property* currentProperty, bool first, int row, const QString type)
{
    QString defaultValue("0");
    QString suffix;
    if (type.length() > 5)
    {
        const QStringList arguments = extractArguments(type);
        defaultValue = arguments.first();
        if (arguments.count() > 1)
        {
            suffix = arguments.value(1);
        }
    }

    QSpinBox* spinBox = new QSpinBox(ui->propertiesWidget);
    if (first)
    {
        spinBox->setFocus();
    }
    spinBox->setMaximum(std::numeric_limits<int>::max());
    spinBox->setMinimum(std::numeric_limits<int>::min());
    spinBox->setValue(defaultValue.toInt());
    spinBox->setSuffix(suffix);
    ui->gridLayoutProperties->addWidget(spinBox,row,1);
    currentProperty->spinBox = spinBox;
}

void MainWindow::createDoubleSpinBoxProperty(Property* currentProperty, bool first, int row, const QString type)
{
    QString defaultValue("0.0");
    QString suffix;
    if (type.length() > 8)
    {
        const QStringList arguments = extractArguments(type);
        defaultValue = arguments.first();
        if (arguments.count() > 1)
        {
            suffix = arguments.value(1);
        }
    }

    QDoubleSpinBox* doubleSpinBox = new QDoubleSpinBox(ui->propertiesWidget);
    if (first)
    {
        doubleSpinBox->setFocus();
    }
    doubleSpinBox->setMaximum(std::numeric_limits<double>::max());
    doubleSpinBox->setMinimum(std::numeric_limits<double>::min());
    doubleSpinBox->setSingleStep(0.1);
    doubleSpinBox->setValue(defaultValue.toDouble());
    doubleSpinBox->setSuffix(suffix);
    ui->gridLayoutProperties->addWidget(doubleSpinBox,row,1);
    currentProperty->doubleSpinBox = doubleSpinBox;
}

void MainWindow::createComboBoxProperty(Property* currentProperty, bool first, int row, const QString type)
{
    const QStringList entries = extractArguments(type);

    QComboBox* comboBox = new QComboBox(ui->propertiesWidget);
    if (first)
    {
        comboBox->setFocus();
    }
    comboBox->insertItems(0, entries);
    ui->gridLayoutProperties->addWidget(comboBox,row,1);
    currentProperty->comboBox = comboBox;
}

void MainWindow::prepareLayout()
{
    QSqlQuery query(m_db);

    if (execQuery(&query, "SELECT value FROM settings WHERE name='maximizeCreator'")
        && query.next())
    {
        const QString value = query.value(0).toString();
        if (value == "true")
        {
            this->showMaximized();
        }
    }

    if (execQuery(&query, "SELECT value FROM settings WHERE name='titleCreator'")
        && query.next())
    {
        const QString value = query.value(0).toString();
        this->setWindowTitle(value);
    }

    if (execQuery(&query, "SELECT value FROM settings WHERE name='buttonCreator'")
        && query.next())
    {
        const QString value = query.value(0).toString();
        ui->pushButton->setText(value);
    }

    int row = 0;
    if (execQuery(&query, "SELECT value FROM settings WHERE name='headerCreator'")
        && query.next())
    {
        QLabel* label = new QLabel(query.value(0).toString());
        QFont font = label->font();
        font.setPointSize(20);
        font.setBold(true);
        label->setFont(font);
        ui->gridLayoutProperties->addWidget(label,row,0,1,2);
        ++row;
        QSpacerItem* spacer = new QSpacerItem(0, 13);
        ui->gridLayoutProperties->addItem(spacer,row,0);
        ++row;
    }

    if (!execQuery(&query, "SELECT name, type FROM properties"))
    {
        return;
    }

    bool first = true;
    while (query.next())
    {
        Property currentProperty;

        const QString name = query.value(0).toString();
        currentProperty.name = name;
        QLabel* label = new QLabel(name, ui->propertiesWidget);
        label->setWordWrap(true);
        ui->gridLayoutProperties->addWidget(label,row,0);
        currentProperty.label = label;

        const QString type = query.value(1).toString();
        if (type.startsWith("string"))
        {
            createLineEditProperty(&currentProperty, first, row, type);
        }
        else if (type.startsWith("date"))
        {
            createDateEditProperty(&currentProperty, first, row, type);
        }
        else if (type.startsWith("int"))
        {
            createSpinBoxProperty(&currentProperty, first, row, type);
        }
        else if (type.startsWith("double"))
        {
            createDoubleSpinBoxProperty(&currentProperty, first, row, type);
        }
        else if (type.startsWith("combo"))
        {
            createComboBoxProperty(&currentProperty, first, row, type);
        }
        else if (type.startsWith("changeCombo"))
        {
            createComboBoxProperty(&currentProperty, first, row, type);
            currentProperty.comboBox->setEditable(true);
        }
        first = false;
        ++row;
        m_properties.append(currentProperty);
    }
}

QStringList MainWindow::readPropertyValues()
{
    QStringList result;
    for (const auto& property : m_properties)
    {
        QString value;
        if (property.lineEdit)
        {
            value = property.lineEdit->text().simplified();
        }
        else if (property.dateEdit)
        {
            const QDate date = property.dateEdit->date();
            if (QLocale().name() == "de_DE")
            {
                value = date.toString("dd.MM.yyyy");
            }
            else
            {
                value = date.toString("yyyy-MM-dd");
            }
        }
        else if (property.spinBox)
        {
            value = QLocale().toString(property.spinBox->value());
        }
        else if (property.doubleSpinBox)
        {
            value = QLocale().toString(property.doubleSpinBox->value(),'f', 2);
        }
        else if (property.comboBox)
        {
            value = property.comboBox->currentText().simplified();
        }

        if (value.isEmpty())
        {
            QMessageBox::warning(this, tr("Warning"), tr("Please fill in all fields!"));
            return QStringList();
        }
        result.append(value.replace(";","_"));
    }
    return result;
}

void MainWindow::printAndSaveDocket()
{
    const int index = saveDocket();
    if (printDocket(index))
    {
        exit(0);
    }
}

bool MainWindow::printDocket(int index)
{
    if (index < 0)
    {
        return false;
    }

    QFile printTemplate("printTemplate.html");
    QByteArray printData = "Index: %INDEX%<br><br>%PROPERTIES%";
    if (!printTemplate.open(QFile::ReadOnly))
    {
        QMessageBox::warning(this, tr("Warning"), tr("Can not find 'printTemplate.html'! Default PDF layout will be used."));
    }
    else
    {
        printData = printTemplate.readAll();
    }


    const QString table = preparePropertyTable();

    printData.replace("%PROPERTIES%", table.toUtf8());
    printData.replace("%INDEX%", QString::number(index).toUtf8());

    QTextDocument doc;
    doc.setHtml(printData);

    QPrinter printer;
    printer.setPageOrientation(QPageLayout::Portrait);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageSize(QPageSize(QPageSize::A4));

    QString path = QFileDialog::getSaveFileName(
                NULL,
                "Save as PDF...",
                QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                "PDF Files (*.pdf)");

    printer.setOutputFileName(path);

    doc.print(&printer);
    QDesktopServices::openUrl(QUrl(path));
    return true;
}

int MainWindow::saveDocket()
{
    const QString properties = readPropertyValues().join(";");
    if (properties.isEmpty())
    {
        return -1;
    }
    QSqlQuery query(m_db);
    auto cleanup = qScopeGuard([&] { execQuery(&query, "ROLLBACK TRANSACTION"); });
    if (!execQuery(&query, "BEGIN TRANSACTION"))
    {
        return -1;
    }
    if (!execQuery(&query, "INSERT INTO dockets (data) VALUES (\""+properties+"\")"))
    {
        return -1;
    }
    if (!execQuery(&query, "SELECT last_insert_rowid()"))
    {
        return -1;
    }

    if (!query.next())
    {
        return -1;
    }
    const int currentIndex = query.value(0).toInt();

    if (!execQuery(&query, "COMMIT"))
    {
        return -1;
    }
    cleanup.dismiss();
    return currentIndex;
}

QString MainWindow::preparePropertyTable()
{
    QString result;
    result.append("<table CELLPADDING=2>");

    auto propertyIter = m_properties.begin();
    for (const auto& property : readPropertyValues())
    {
        result.append("<tr>");
        result.append("<td valign=middle>");
        result.append(propertyIter->name + ": ");
        result.append("</td>");
        result.append("<td valign=middle>");
        result.append(" " + property);
        result.append("</td>");
        result.append("</tr>");
        propertyIter++;
    }
    result.append("</table> ");
    return result;
}
