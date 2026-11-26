#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTableWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void loadText();       void encode();        void saveCode();        // п.1
    void loadCode2();      void corrupt();       void saveCorrupted();   // п.2
    void loadCode();       void decodeClean();   void saveDecoded();     // п.3
    void recover();        void saveVariantCode();                       // п.4
    void findInOriginal();

private:
    QTextEdit *textEdit, *codeEdit, *corrEdit, *decEdit, *logEdit;
    QTableWidget *variantsTable;
    QSpinBox *maxVarSpin;
    QDoubleSpinBox *pPanicSpin, *pPauseSpin;
    QLabel *statusLabel;
    QStringList m_variants;
    QStringList m_variantCodes;
    int  m_autoK = 1;
    bool m_trunc = false;

    void log(const QString &s);
    void runRecovery(int forceK);
    int  matchVariants(const QStringList &origCodes, const QVector<int> &lineNos, const QString &path);
};

#endif // MAINWINDOW_H
