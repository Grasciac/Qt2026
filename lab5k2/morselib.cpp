#include "morselib.h"
#include <QRandomGenerator>
#include <QSet>
#include <algorithm>
#include <climits>
#include <functional>
#include <queue>
#include <vector>

namespace morse {

const std::map<QString, QString> CHAR2CODE = {
    {QStringLiteral("А"), "*-"},   {QStringLiteral("Б"), "-***"}, {QStringLiteral("В"), "*--"},
    {QStringLiteral("Г"), "--*"},  {QStringLiteral("Д"), "-**"},  {QStringLiteral("Е"), "*"},
    {QStringLiteral("Ж"), "***-"}, {QStringLiteral("З"), "--**"}, {QStringLiteral("И"), "**"},
    {QStringLiteral("Й"), "*---"}, {QStringLiteral("К"), "-*-"},  {QStringLiteral("Л"), "*-**"},
    {QStringLiteral("М"), "--"},   {QStringLiteral("Н"), "-*"},   {QStringLiteral("О"), "---"},
    {QStringLiteral("П"), "*--*"}, {QStringLiteral("Р"), "*-*"},  {QStringLiteral("С"), "***"},
    {QStringLiteral("Т"), "-"},    {QStringLiteral("У"), "**-"},  {QStringLiteral("Ф"), "**-*"},
    {QStringLiteral("Х"), "****"}, {QStringLiteral("Ц"), "-*-*"}, {QStringLiteral("Ч"), "---*"},
    {QStringLiteral("Ш"), "----"}, {QStringLiteral("Щ"), "--*-"}, {QStringLiteral("Ъ"), "--*--*"},
    {QStringLiteral("Ы"), "-*--"}, {QStringLiteral("Ь"), "-**-"}, {QStringLiteral("Э"), "**-**"},
    {QStringLiteral("Ю"), "**--"}, {QStringLiteral("Я"), "*-*-"},
    };
const std::map<QString, QString> CODE2CHAR = [] {
    std::map<QString, QString> m;
    for (const auto &kv : CHAR2CODE) m.emplace(kv.second, kv.first);
    return m;
}();

static const QString WORD_SEP = QStringLiteral("//");
static QStringList lineWords(const QString &line)   { return line.split(WORD_SEP, Qt::SkipEmptyParts); }
static QStringList wordLetters(const QString &word) { return word.split(QLatin1Char('/'), Qt::SkipEmptyParts); }

QString encodeText(const QString &text) {
    QStringList outLines;
    const QStringList lines = text.split(QLatin1Char('\n'));
    for (const QString &line : lines) {
        QStringList wordsOut;
        const QStringList words = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
        for (const QString &w : words) {
            QStringList codes;
            for (const QChar &c : w) {
                auto it = CHAR2CODE.find(QString(c).toUpper());
                if (it != CHAR2CODE.end()) codes << it->second;
            }
            if (!codes.isEmpty()) wordsOut << codes.join(QLatin1Char('/'));
        }
        outLines << wordsOut.join(WORD_SEP);
    }
    return outLines.join(QLatin1Char('\n'));
}

QString decodeClean(const QString &code) {
    QStringList outLines;
    const QStringList lines = code.split(QLatin1Char('\n'));
    for (const QString &line : lines) {
        QStringList wordsOut;
        const QStringList words = lineWords(line);
        for (const QString &w : words) {
            QString s;
            const QStringList lets = wordLetters(w);
            for (const QString &cc : lets) {
                auto it = CODE2CHAR.find(cc);
                s += (it != CODE2CHAR.end()) ? it->second : QStringLiteral("?");
            }
            wordsOut << s;
        }
        outLines << wordsOut.join(QLatin1Char(' '));
    }
    return outLines.join(QLatin1Char('\n'));
}

QString corruptCode(const QString &code, const CorruptOptions &opt) {
    quint32 seed = opt.seed ? opt.seed : QRandomGenerator::system()->generate();
    QRandomGenerator rng(seed);
    struct Block { QString code; bool originStart; };
    QVector<QVector<QVector<Block>>> stream;
    const QStringList lines = code.split(QLatin1Char('\n'));
    for (const QString &line : lines) {
        QVector<QVector<Block>> words;
        const QStringList ws = lineWords(line);
        for (const QString &w : ws) {
            QVector<Block> blocks;
            const QStringList ls = wordLetters(w);
            for (const QString &s : ls) blocks.push_back(Block{s, true});
            if (!blocks.isEmpty()) words.push_back(blocks);
        }
        stream.push_back(words);
    }

    for (auto &words : stream) {                    // один проход = одно применение нарушения
        QVector<QVector<Block>> newWords;
        for (const auto &blocks : words) {
            QVector<Block> res;
            for (Block b : blocks) {
                if (rng.generateDouble() < opt.pPanic) {
                    const int start = b.originStart ? 1 : 0;
                    QVector<int> pos;
                    for (int i = start; i < int(b.code.size()); ++i)
                        if (b.code[i] == QLatin1Char('-')) pos << i;
                    if (!pos.isEmpty())
                        b.code[pos.at(rng.bounded(int(pos.size())))] = QLatin1Char('*');
                }
                if (int(b.code.size()) >= 2 && rng.generateDouble() < opt.pPause) {
                    const int p = 1 + int(rng.bounded(uint(b.code.size() - 1)));
                    res.push_back(Block{b.code.left(p), b.originStart});
                    res.push_back(Block{b.code.mid(p), false});
                } else {
                    res.push_back(b);
                }
            }
            newWords.push_back(res);
        }
        words = newWords;
    }

    QStringList outLines;
    for (const auto &words : stream) {
        QStringList ws;
        for (const auto &blocks : words) {
            QStringList ls;
            for (const auto &b : blocks) ls << b.code;
            ws << ls.join(QLatin1Char('/'));
        }
        outLines << ws.join(WORD_SEP);
    }
    return outLines.join(QLatin1Char('\n'));
}

namespace {
struct Var { QString text; int merges; int flips; int letters; };
inline int penaltyOf(const Var &v) { return v.merges + v.flips; }
inline quint64 costOf(const Var &v) { return (quint64(penaltyOf(v)) << 32) | quint64(v.letters); }

bool matchesFlips(const QString &concat, const QString &s, int maxFlips, int &flipsOut) {
    flipsOut = 0;
    if (concat.size() != s.size()) return false;
    if (concat[0] != s[0]) return false;
    for (int i = 0; i < int(concat.size()); ++i) {
        QChar qc = concat[i], qs = s[i];
        if (qc == QLatin1Char('-')) { if (qs != QLatin1Char('-')) return false; }
        else if (qc == QLatin1Char('*')) {
            if (qs == QLatin1Char('-')) { if (++flipsOut > maxFlips) return false; }
            else if (qs != QLatin1Char('*')) return false;
        } else return false;
    }
    return true;
}

QVector<Var> combineBest(const QVector<Var> &A, const QVector<Var> &B, QChar sep, int cap, bool *trunc) {
    QVector<Var> out;
    if (A.isEmpty() || B.isEmpty()) return out;
    using St = QPair<quint64, QPair<int, int>>;
    auto cmp = [](const St &x, const St &y) {
        if (x.first != y.first) return x.first > y.first;
        if (x.second.first != y.second.first) return x.second.first > y.second.first;
        return x.second.second > y.second.second;
    };
    std::priority_queue<St, QVector<St>, decltype(cmp)> pq(cmp);
    QSet<quint64> seen;
    auto push = [&](int i, int j) {
        if (i >= int(A.size()) || j >= int(B.size())) return;
        const quint64 key = (quint64(i) << 32) | quint64(j);
        if (seen.contains(key)) return;
        seen.insert(key);
        pq.push(St{ costOf(A[i]) + costOf(B[j]), {i, j} });
    };
    push(0, 0);
    while (!pq.empty() && out.size() < cap) {
        const St cur = pq.top(); pq.pop();
        const int i = cur.second.first, j = cur.second.second;
        Var v;
        v.text    = A[i].text.isEmpty() ? B[j].text : (A[i].text + sep + B[j].text);
        v.merges  = A[i].merges  + B[j].merges;
        v.flips   = A[i].flips   + B[j].flips;
        v.letters = A[i].letters + B[j].letters;
        out.push_back(v);
        push(i + 1, j);
        push(i, j + 1);
    }
    if (!pq.empty() && trunc) *trunc = true;
    return out;
}
}

QStringList recoverVariants(const QString &corrupted, int maxCycles, int maxVariants, bool *truncated) {
    const int maxFlips = (1 << maxCycles) - 1;
    const int maxGroup = (1 << maxCycles);
    if (truncated) *truncated = false;
    if (maxVariants < 1) maxVariants = 1;

    QVector<Var> acc = { Var{QString(), 0, 0, 0} };
    const QStringList lines = corrupted.split(QLatin1Char('\n'));
    for (const QString &line : lines) {
        QVector<Var> lineAcc = { Var{QString(), 0, 0, 0} };
        const QStringList words = lineWords(line);
        for (const QString &w : words) {
            const QStringList lets = wordLetters(w);
            const int n = int(lets.size());

            // BEST-FIRST по разбиениям слова: варианты выдаются в порядке возрастания cost,
            // поэтому лимит оставляет самые правдоподобные (мин. штраф), а не первые из DFS.
            QVector<Var> wordVars;
            struct St { QString prefix; int idx, merges, flips, letters; };
            auto cmp = [](const QPair<quint64, St> &a, const QPair<quint64, St> &b) { return a.first > b.first; };
            std::priority_queue<QPair<quint64, St>, QVector<QPair<quint64, St>>, decltype(cmp)> pq(cmp);
            pq.push({ 0, St{QString(), 0, 0, 0, 0} });
            while (!pq.empty() && int(wordVars.size()) < maxVariants) {
                const St cur = pq.top().second; pq.pop();
                if (cur.idx == n) { wordVars.push_back(Var{cur.prefix, cur.merges, cur.flips, cur.letters}); continue; }
                for (int g = 1; g <= maxGroup && cur.idx + g <= n; ++g) {
                    QString concat;
                    for (int q = cur.idx; q < cur.idx + g; ++q) concat += lets[q];
                    for (const auto &kv : CODE2CHAR) {
                        int fl = 0;
                        if (matchesFlips(concat, kv.first, maxFlips, fl)) {
                            St ns{ cur.prefix + kv.second, cur.idx + g,
                                  cur.merges + (g - 1), cur.flips + fl, cur.letters + 1 };
                            const quint64 c = (quint64(ns.merges + ns.flips) << 32) | quint64(ns.letters);
                            pq.push({ c, ns });
                        }
                    }
                }
            }
            if (!pq.empty() && truncated) *truncated = true;

            lineAcc = combineBest(lineAcc, wordVars, QLatin1Char(' '), maxVariants, truncated);
            if (lineAcc.isEmpty()) return QStringList();
        }
        acc = combineBest(acc, lineAcc, QLatin1Char('\n'), maxVariants, truncated);
        if (acc.isEmpty()) return QStringList();
    }
    QStringList res;
    res.reserve(acc.size());
    for (const Var &v : acc) res << v.text;
    return res;
}


QStringList recoverAuto(const QString &corrupted, int maxVariants, int *usedK, bool *truncated) {
    const int KMAX = 5;
    for (int K = 1; K <= KMAX; ++K) {          // минимальная глубина, где код вообще декодируем
        QStringList vars = recoverVariants(corrupted, K, maxVariants, truncated);
        if (!vars.isEmpty()) { if (usedK) *usedK = K; return vars; }
    }
    if (usedK) *usedK = KMAX;                  // нигде не декодируется — отдаём последнюю попытку
    return recoverVariants(corrupted, KMAX, maxVariants, truncated);
}

bool canProduce(const QString &origLine, const QString &corrLine, int maxCycles) {
    const int maxFlips = (1 << maxCycles) - 1;
    const int maxGroup = (1 << maxCycles);
    const QStringList wo = lineWords(encodeText(origLine));
    const QStringList wc = lineWords(corrLine);
    if (wo.size() != wc.size()) return false;
    for (int i = 0; i < int(wo.size()); ++i) {
        const QStringList lo = wordLetters(wo[i]);
        const QStringList lc = wordLetters(wc[i]);
        std::vector<std::vector<char>> dp(lo.size() + 1, std::vector<char>(lc.size() + 1, 0));
        dp[0][0] = 1;
        for (int a = 0; a < int(lo.size()); ++a)
            for (int b = 0; b < int(lc.size()); ++b)
                if (dp[a][b])
                    for (int g = 1; g <= maxGroup && b + g <= int(lc.size()); ++g) {
                        QString concat;
                        for (int q = 0; q < g; ++q) concat += lc[b + q];
                        int fl = 0;
                        if (matchesFlips(concat, lo[a], maxFlips, fl)) dp[a + 1][b + g] = 1;
                    }
        if (!dp[lo.size()][lc.size()]) return false;
    }
    return true;
}

} // namespace morse
