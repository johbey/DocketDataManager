#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSettings>
#include <QFile>
#include <QClipboard>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connectDb();
    connect(ui->spinBox, qOverload<int>(&QSpinBox::valueChanged), this, &MainWindow::readDocket);
    connect(ui->copyButton, &QPushButton::clicked, this, [=]
    {
        QClipboard *clipboard = QApplication::clipboard();
        QString originalText = clipboard->text();

        clipboard->setText(ui->label->text());
    });

    QSqlQuery query(m_db);
    if (!execQuery(&query, "SELECT MAX(id) FROM dockets"))
    {
        return;
    }
    if (!query.next())
    {
        return;
    }
    const int docketCount = query.value(0).toInt();
    ui->spinBox->setMinimum(0);
    ui->spinBox->setMaximum(docketCount);

    auto* version = new QLabel("v001");
    version->setStyleSheet("font-size: 8pt;");
    ui->statusbar->addPermanentWidget(version, 1);
    auto* label = new QLabel("© github.com/johbey");
    label->setStyleSheet("font-size: 8pt;");
    ui->statusbar->addPermanentWidget(label);

    if (execQuery(&query, "SELECT value FROM settings WHERE name='titleViewer'")
        && query.next())
    {
        const QString value = query.value(0).toString();
        this->setWindowTitle(value);
    }

    if (execQuery(&query, "SELECT value FROM settings WHERE name='textViewer'")
        && query.next())
    {
        const QString value = query.value(0).toString();
        ui->spinBox->setSpecialValueText(value);
    }

    if (execQuery(&query, "SELECT value FROM settings WHERE name='buttonViewer'")
        && query.next())
    {
        const QString value = query.value(0).toString();
        ui->copyButton->setText(value);
    }

    ui->spinBox->setFocus();
    ui->spinBox->selectAll();
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

bool MainWindow::execQuery(QSqlQuery* query,const QString& sql)
{
    if (!query->exec(sql))
    {
        QMessageBox::critical(this, "Error", query->lastError().text());
        return false;
    }
    return true;
}

void MainWindow::readDocket(int value)
{
    ui->label->setText(QString());

    QSqlQuery query(m_db);
    if (!execQuery(&query, "SELECT id, data FROM dockets WHERE id="+QString::number(value)))
    {
        return;
    }

    if (!query.next())
    {
        return;
    }

    const int currentIndex = ui->spinBox->value();
    QString values = QString::number(currentIndex) + ";" + query.value(1).toString();
    values.replace(";", "\n");
    ui->label->setText(values);

    return;
}

