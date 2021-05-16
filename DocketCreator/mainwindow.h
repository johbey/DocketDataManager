#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QSqlDatabase>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QLabel;
class QLineEdit;
class QDateEdit;
class QSpinBox;
class QDoubleSpinBox;
class QComboBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

struct Property
{
    Property()
        : label(nullptr)
        , lineEdit(nullptr)
        , dateEdit(nullptr)
        , spinBox(nullptr)
        , doubleSpinBox(nullptr)
        , comboBox(nullptr)
    {

    }
    QString name;
    QLabel* label;
    QLineEdit* lineEdit;
    QDateEdit* dateEdit;
    QSpinBox* spinBox;
    QDoubleSpinBox* doubleSpinBox;
    QComboBox* comboBox;
};

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void printAndSaveDocket();

private:
    void prepareLayout();
    QStringList readPropertyValues();
    bool printDocket(int index);
    int saveDocket();
    QString preparePropertyTable();
    void connectDb();
    bool execQuery(QSqlQuery* query,const QString& sql);
    QStringList extractArguments(const QString& text);
    void createLineEditProperty(Property* currentProperty, bool first, int row, const QString type);
    void createDateEditProperty(Property* currentProperty, bool first, int row, const QString type);
    void createSpinBoxProperty(Property* currentProperty, bool first, int row, const QString type);
    void createDoubleSpinBoxProperty(Property* currentProperty, bool first, int row, const QString type);
    void createComboBoxProperty(Property* currentProperty, bool first, int row, const QString type);
    void createChangeableComboBoxProperty(Property* currentProperty, bool first, int row, const QString type);

private:
    Ui::MainWindow *ui;
    QList<Property> m_properties;
    QSqlDatabase m_db;
};
#endif // MAINWINDOW_H
