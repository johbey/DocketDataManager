#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void readDocket(int value);

private:
    void connectDb();
    bool execQuery(QSqlQuery* query,const QString& sql);

private:
    Ui::MainWindow *ui;
    QSqlDatabase m_db;
};
#endif // MAINWINDOW_H
