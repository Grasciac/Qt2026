#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QComboBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QPushButton>
#include <QButtonGroup>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// Класс для хранения данных пары
class Pair {
public:
    Pair(const QString& subject, const QString& teacher, const QString& audience,
         int number, const QString& lessonType, const QStringList& expectations);

    bool saveToFile(const QString& filename = "result.txt") const;

private:
    QString m_subject;
    QString m_teacher;
    QString m_audience;
    int m_number;
    QString m_lessonType;
    QStringList m_expectations;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onResetClicked();
    void onSaveClicked();

private:
    void setupUi();
    bool validateFields();
    void showError(const QString& message);

    // Поля ввода
    QLineEdit *subjectEdit;
    QLineEdit *teacherEdit;
    QLineEdit *audienceEdit;
    QComboBox *numberCombo;

    // Radio buttons
    QRadioButton *lectureRadio;
    QRadioButton *practiceRadio;
    QButtonGroup *typeGroup;

    // Checkboxes
    QCheckBox *controlCheck;
    QCheckBox *attendanceCheck;
    QCheckBox *cancelCheck;

    // Buttons
    QPushButton *resetButton;
    QPushButton *saveButton;

    // Регулярные выражения для валидации
    QRegularExpression subjectRegex;
    QRegularExpression teacherRegex;
    QRegularExpression audienceRegex;
};
#endif // MAINWINDOW_H
