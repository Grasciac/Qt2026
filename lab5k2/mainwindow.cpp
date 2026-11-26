#include "mainwindow.h"
#include "morselib.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QtGlobal>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QMessageBox>
#include <QHeaderView>

// ---------- файловые помощники ----------
static QString readFile(const QString &path) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return QString();
    QTextStream in(&f);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    in.setCodec("UTF-8");                 // в Qt5 поток по умолчанию Latin-1 — кириллица ломалась
#endif
    QString s = in.readAll();
    s.replace(QStringLiteral("\r\n"), QStringLiteral("\n")).replace(QLatin1Char('\r'), QLatin1Char('\n'));
    return s;
}
static bool writeFile(const QString &path, const QString &data) {
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&f);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");
#endif
    out << data;
    return true;
}
static QString oneLine(const QString &s) { return QString(s).replace(QLatin1Char('\n'), QChar(0x21B5)); }
// для ячеек таблицы: перевод строки показываем как ↵, оригинал храним в m_variants

// ---------- конструктор: сборка интерфейса ----------
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    auto *central = new QWidget(this); setCentralWidget(central);
    auto *root = new QVBoxLayout(central);

    // ---- п.1 Кодирование ----
    auto *g1 = new QGroupBox(tr("1. Кодирование (текст -> код Морзе; слова: '//')"), central);
    auto *h1 = new QHBoxLayout(g1);
    textEdit = new QTextEdit(g1); textEdit->setPlaceholderText(tr("Исходный текст..."));
    codeEdit = new QTextEdit(g1); codeEdit->setPlaceholderText(tr("Код Морзе..."));
    auto *pbLoadText = new QPushButton(tr("Загрузить текст..."), g1);
    auto *pbEnc      = new QPushButton(tr("Кодировать ->"), g1);
    auto *pbSaveCode = new QPushButton(tr("Сохранить код в файл..."), g1);
    auto *v1 = new QVBoxLayout; v1->addWidget(pbLoadText); v1->addWidget(pbEnc); v1->addWidget(pbSaveCode);
    h1->addWidget(textEdit); h1->addLayout(v1); h1->addWidget(codeEdit);
    connect(pbLoadText, &QPushButton::clicked, this, &MainWindow::loadText);
    connect(pbEnc,      &QPushButton::clicked, this, &MainWindow::encode);
    connect(pbSaveCode, &QPushButton::clicked, this, &MainWindow::saveCode);

    // ---- п.2 Передача + нарушение (1 нажатие = 1 проход) ----
    auto *g2 = new QGroupBox(tr("2. Передача: вар.3 + вар.1; 1 нажатие = 1 раз (повтор: сохрани и загрузи код сюда)"), central);
    auto *h2 = new QHBoxLayout(g2);
    pPanicSpin = new QDoubleSpinBox(g2); pPanicSpin->setRange(0, 1); pPanicSpin->setSingleStep(0.05); pPanicSpin->setValue(0.30);
    pPauseSpin = new QDoubleSpinBox(g2); pPauseSpin->setRange(0, 1); pPauseSpin->setSingleStep(0.05); pPauseSpin->setValue(0.30);
    auto *pbLoadCode2 = new QPushButton(tr("Загрузить код..."), g2);        // вход п.2 из Morse-txt
    auto *pbCor       = new QPushButton(tr("Испортить код ->"), g2);
    auto *pbSaveCor   = new QPushButton(tr("Сохранить испорченный код..."), g2);
    corrEdit = new QTextEdit(g2); corrEdit->setPlaceholderText(tr("Испорченный код..."));
    h2->addWidget(new QLabel(tr("P(паника):"), g2)); h2->addWidget(pPanicSpin);
    h2->addWidget(new QLabel(tr("P(пауза):"), g2));  h2->addWidget(pPauseSpin);
    h2->addWidget(pbLoadCode2); h2->addWidget(pbCor); h2->addWidget(pbSaveCor); h2->addWidget(corrEdit, 1);
    connect(pbLoadCode2, &QPushButton::clicked, this, &MainWindow::loadCode2);
    connect(pbCor,       &QPushButton::clicked, this, &MainWindow::corrupt);
    connect(pbSaveCor,   &QPushButton::clicked, this, &MainWindow::saveCorrupted);

    // ---- п.3 Декодирование ----
    auto *g3 = new QGroupBox(tr("3. Декодирование (ошибки '?' -> предложение запустить п.4)"), central);
    auto *h3 = new QHBoxLayout(g3);
    auto *pbLoadCode = new QPushButton(tr("Загрузить код..."), g3);
    auto *pbDec      = new QPushButton(tr("Декодировать ->"), g3);
    auto *pbSaveDec  = new QPushButton(tr("Сохранить декодированный текст..."), g3);
    decEdit = new QTextEdit(g3); decEdit->setReadOnly(true);
    auto *v3 = new QVBoxLayout; v3->addWidget(pbLoadCode); v3->addWidget(pbDec); v3->addWidget(pbSaveDec);
    h3->addLayout(v3); h3->addWidget(decEdit, 1);
    connect(pbLoadCode, &QPushButton::clicked, this, &MainWindow::loadCode);
    connect(pbDec,      &QPushButton::clicked, this, &MainWindow::decodeClean);
    connect(pbSaveDec,  &QPushButton::clicked, this, &MainWindow::saveDecoded);

    // ---- п.4 Восстановление + поиск в исходном ----
    auto *g4 = new QGroupBox(tr("4. Восстановление (1 раз, K подбирается сам): расшифровка И последовательность"), central);
    auto *h4 = new QHBoxLayout(g4);
    maxVarSpin = new QSpinBox(g4); maxVarSpin->setRange(1, 1000000); maxVarSpin->setValue(1000);
    auto *pbRec     = new QPushButton(tr("Восстановить варианты"), g4);
    auto *pbSaveVar = new QPushButton(tr("Сохранить код выбранного варианта..."), g4);
    auto *pbFind    = new QPushButton(tr("Найти в исходном файле..."), g4);
    auto *v4 = new QVBoxLayout;
    v4->addWidget(new QLabel(tr("Макс. вариантов:"), g4)); v4->addWidget(maxVarSpin);
    v4->addWidget(pbRec); v4->addWidget(pbSaveVar); v4->addWidget(pbFind);
    variantsTable = new QTableWidget(0, 2, g4);
    variantsTable->setHorizontalHeaderLabels({ tr("Расшифровка (текст)"), tr("Последовательность (код Морзе)") });
    variantsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    variantsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    variantsTable->horizontalHeader()->setStretchLastSection(true);
    h4->addLayout(v4); h4->addWidget(variantsTable, 1);
    connect(pbRec,     &QPushButton::clicked, this, &MainWindow::recover);
    connect(pbSaveVar, &QPushButton::clicked, this, &MainWindow::saveVariantCode);
    connect(pbFind,    &QPushButton::clicked, this, &MainWindow::findInOriginal);

    logEdit = new QTextEdit(central); logEdit->setReadOnly(true);
    statusLabel = new QLabel(tr("Готово"), central);
    root->addWidget(g1); root->addWidget(g2); root->addWidget(g3); root->addWidget(g4);
    root->addWidget(logEdit, 1); root->addWidget(statusLabel);

    setWindowTitle(tr("Лаб. №5 Кодирование (Морзе), вариант 3+1"));
    resize(1150, 840);
}

void MainWindow::log(const QString &s) { logEdit->append(s); }

// ---------- п.1 ----------
void MainWindow::loadText() {
    const QString p = QFileDialog::getOpenFileName(this, tr("Открыть текст"), QString(), tr("Текст (*.txt);;Все файлы (*)"));
    if (!p.isEmpty()) { textEdit->setPlainText(readFile(p)); log(tr("Загружен текст: ") + p); }
}
void MainWindow::encode() {
    codeEdit->setPlainText(morse::encodeText(textEdit->toPlainText()));
    log(tr("Кодирование выполнено (разделитель слов: '//')."));
}
void MainWindow::saveCode() {
    const QString p = QFileDialog::getSaveFileName(this, tr("Сохранить код Морзе"), QString(), tr("Текст (*.txt);;Все файлы (*)"));
    if (!p.isEmpty() && writeFile(p, codeEdit->toPlainText())) log(tr("Код сохранён: ") + p);
}

// ---------- п.2 ----------
void MainWindow::loadCode2() {
    const QString p = QFileDialog::getOpenFileName(this, tr("Открыть код Морзе (вход п.2)"), QString(), tr("Текст (*.txt);;Все файлы (*)"));
    if (!p.isEmpty()) { codeEdit->setPlainText(readFile(p)); log(tr("Код загружен в пункт 2: ") + p); }
}
void MainWindow::corrupt() {
    morse::CorruptOptions o;
    o.pPanic = pPanicSpin->value(); o.pPause = pPauseSpin->value();
    corrEdit->setPlainText(morse::corruptCode(codeEdit->toPlainText(), o));
    log(tr("Нарушение применено 1 раз (вар.3 + вар.1)."));
}
void MainWindow::saveCorrupted() {
    const QString p = QFileDialog::getSaveFileName(this, tr("Сохранить испорченный код"), QString(), tr("Текст (*.txt);;Все файлы (*)"));
    if (!p.isEmpty() && writeFile(p, corrEdit->toPlainText())) log(tr("Испорченный код сохранён: ") + p);
}

// ---------- п.3 ----------
void MainWindow::loadCode() {
    const QString p = QFileDialog::getOpenFileName(this, tr("Открыть код Морзе (вход п.3)"), QString(), tr("Текст (*.txt);;Все файлы (*)"));
    if (!p.isEmpty()) { corrEdit->setPlainText(readFile(p)); log(tr("Код загружен в пункт 3: ") + p); }
}
void MainWindow::decodeClean() {
    const QString dec = morse::decodeClean(corrEdit->toPlainText());
    decEdit->setPlainText(dec);
    log(tr("Прямое декодирование выполнено."));
    if (dec.contains(QLatin1Char('?')))
        if (QMessageBox::question(this, tr("Декодирование"),
                                  tr("В коде обнаружены ошибки ('?'). Запустить алгоритм восстановления (п. 4)?"),
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
            recover();
}
void MainWindow::saveDecoded() {
    const QString p = QFileDialog::getSaveFileName(this, tr("Сохранить декодированный текст"), QString(), tr("Текст (*.txt);;Все файлы (*)"));
    if (!p.isEmpty() && writeFile(p, decEdit->toPlainText())) log(tr("Декодированный текст сохранён: ") + p);
}

// ---------- п.4 ----------
void MainWindow::recover() {          // кнопка: глубина подбирается автоматически
    runRecovery(0);
}

void MainWindow::runRecovery(int forceK) {
    bool trunc = false; int K = forceK;
    if (forceK > 0)
        m_variants = morse::recoverVariants(corrEdit->toPlainText(), forceK, maxVarSpin->value(), &trunc);
    else
        m_variants = morse::recoverAuto(corrEdit->toPlainText(), maxVarSpin->value(), &K, &trunc);
    m_autoK = K;
    m_trunc = trunc;
    m_variantCodes.clear();
    variantsTable->setRowCount(0);
    for (const QString &v : m_variants) {
        const QString code = morse::encodeText(v);
        m_variantCodes << code;
        const int row = variantsTable->rowCount();
        variantsTable->insertRow(row);
        variantsTable->setItem(row, 0, new QTableWidgetItem(oneLine(v)));
        variantsTable->setItem(row, 1, new QTableWidgetItem(oneLine(code)));
    }
    if (forceK > 0)
        log(tr("Восстановление: глубина K = %1 (задана проверкой по исходному файлу), вариантов = %2%3")
                .arg(K).arg(m_variants.size()).arg(trunc ? tr(" (список усечён до лимита)") : QString()));
    else
        log(tr("Восстановление: авто-глубина K = %1, вариантов = %2%3")
                .arg(K).arg(m_variants.size()).arg(trunc ? tr(" (список усечён до лимита)") : QString()));
    if (m_variants.isEmpty()) log(tr("Вариантов нет: код несовместим с моделью."));
}

void MainWindow::saveVariantCode() {
    const int row = variantsTable->currentRow();
    if (row < 0 || row >= m_variantCodes.size()) return;
    const QString p = QFileDialog::getSaveFileName(this, tr("Сохранить код Морзе варианта"), QString(), tr("Текст (*.txt);;Все файлы (*)"));
    if (!p.isEmpty() && writeFile(p, m_variantCodes[row]))
        log(tr("Код варианта #%1 сохранён: %2").arg(row + 1).arg(p));
}

// ---------- поиск в исходном файле: сравнение ПО КОДУ МОРЗЕ ----------
int MainWindow::matchVariants(const QStringList &origCodes, const QVector<int> &lineNos, const QString &path) {
    for (int i = 0; i < m_variants.size(); ++i) {
        QStringList vcodes;                                   // коды строк варианта
        for (const QString &s : m_variants[i].split(QLatin1Char('\n'))) {
            const QString c = morse::encodeText(s.trimmed());
            if (!c.isEmpty()) vcodes << c;
        }
        if (vcodes.isEmpty()) continue;
        if (vcodes == origCodes) {                            // (а) вариант = весь файл
            variantsTable->selectRow(i);
            log(tr("НАЙДЕНО: вариант #%1 совпадает с исходным файлом целиком (%2 строк кода).")
                    .arg(i + 1).arg(origCodes.size()));
            return i;
        }
        for (int start = 0; start + vcodes.size() <= origCodes.size(); ++start) {   // (б) диапазон строк
            bool ok = true;
            for (int t = 0; t < vcodes.size() && ok; ++t) ok = (vcodes[t] == origCodes[start + t]);
            if (ok) {
                variantsTable->selectRow(i);
                log(tr("НАЙДЕНО: вариант #%1 совпадает со строками %2–%3 исходного файла %4.")
                        .arg(i + 1).arg(lineNos[start]).arg(lineNos[start + vcodes.size() - 1]).arg(path));
                return i;
            }
        }
    }
    return -1;
}

void MainWindow::findInOriginal() {
    const QString p = QFileDialog::getOpenFileName(this, tr("Открыть ИСХОДНЫЙ текст"), QString(), tr("Текст (*.txt);;Все файлы (*)"));
    if (p.isEmpty()) return;
    QStringList origCodes, origTexts; QVector<int> lineNos;   // код строки, текст строки, реальный номер строки
    const QStringList raw = readFile(p).split(QLatin1Char('\n'));
    for (int i = 0; i < raw.size(); ++i) {
        const QString c = morse::encodeText(raw[i].trimmed()); // пунктуация/регистр/пробелы кодом не видны
        if (!c.isEmpty()) { origCodes << c; origTexts << raw[i].trimmed(); lineNos << (i + 1); }
    }
    if (matchVariants(origCodes, lineNos, p) >= 0) {           // вариант уже есть в таблице
        statusLabel->setText(tr("Совпадение найдено в списке вариантов"));
        return;
    }
    const QStringList corrLines = corrEdit->toPlainText().split(QLatin1Char('\n'));
    for (int K = 1; K <= 5; ++K)                               // доказательство совместимости с подбором глубины
        for (int t = 0; t < origTexts.size(); ++t)
            for (int c = 0; c < corrLines.size(); ++c)
                if (morse::canProduce(origTexts[t], corrLines[c], K)) {
                    log(tr("НАЙДЕНО проверкой: строка %1 исходного файла совместима с испорченным кодом (строка %2, глубина K=%3).")
                            .arg(lineNos[t]).arg(c + 1).arg(K));
                    statusLabel->setText(tr("Исходная строка: %1").arg(lineNos[t]));
                    if (K != m_autoK) runRecovery(K);          // пересборка таблицы на доказанной глубине
                    const int idx = matchVariants(origCodes, lineNos, p);
                    if (idx >= 0)
                        log(tr("Эта строка присутствует в таблице вариантов под №%1 (подсвечена).").arg(idx + 1));
                    else if (m_trunc)
                        log(tr("Истинный вариант за пределами лимита (позиция > %1): увеличь «Макс. вариантов».").arg(m_variants.size()));
                    else
                        log(tr("Вариант доказан проверкой, но не сопоставился со строками файла даже по коду Морзе — проверь содержимое файла."));
                    return;
                }
    log(tr("Совпадений с исходным файлом не найдено."));
    statusLabel->setText(tr("Совпадений нет"));
}
