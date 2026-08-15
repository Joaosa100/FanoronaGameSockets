#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>

class QLineEdit;

class HomePage : public QWidget {
    Q_OBJECT

public:
    explicit HomePage(QWidget *parent = nullptr);

signals:
    void hostRequested(const QString &playerName);
    void joinRequested(const QString &playerName);

private:
    QString playerNameOrDefault() const;

    QLineEdit *m_nameEdit;
};

#endif // HOMEPAGE_H
