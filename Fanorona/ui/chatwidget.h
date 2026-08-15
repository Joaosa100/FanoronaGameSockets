#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>

class QLineEdit;
class QTextEdit;

class ChatWidget : public QWidget {
    Q_OBJECT

public:
    explicit ChatWidget(QWidget *parent = nullptr);

    void appendLocalMessage(const QString &senderName, const QString &text);
    void appendRemoteMessage(const QString &senderName, const QString &text);
    void appendSystemMessage(const QString &text);

signals:
    void messageSubmitted(const QString &text);

private slots:
    void onSendClicked();

private:
    void appendLine(const QString &html);

    QTextEdit *m_history;
    QLineEdit *m_input;
};

#endif // CHATWIDGET_H
