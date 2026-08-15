#include "chatwidget.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

ChatWidget::ChatWidget(QWidget *parent)
    : QWidget(parent)
{
    m_history = new QTextEdit(this);
    m_history->setReadOnly(true);

    m_input = new QLineEdit(this);
    m_input->setPlaceholderText(tr("Digite uma mensagem..."));

    auto *sendButton = new QPushButton(tr("Enviar"), this);

    auto *inputLayout = new QHBoxLayout;
    inputLayout->addWidget(m_input);
    inputLayout->addWidget(sendButton);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(m_history, 1);
    layout->addLayout(inputLayout);

    connect(sendButton, &QPushButton::clicked, this, &ChatWidget::onSendClicked);
    connect(m_input, &QLineEdit::returnPressed, this, &ChatWidget::onSendClicked);
}

void ChatWidget::onSendClicked()
{
    const QString text = m_input->text().trimmed();
    if (text.isEmpty())
        return;
    m_input->clear();
    emit messageSubmitted(text);
}

void ChatWidget::appendLine(const QString &html)
{
    m_history->append(html);
}

void ChatWidget::appendLocalMessage(const QString &senderName, const QString &text)
{
    appendLine(QStringLiteral("<b>%1:</b> %2").arg(senderName.toHtmlEscaped(), text.toHtmlEscaped()));
}

void ChatWidget::appendRemoteMessage(const QString &senderName, const QString &text)
{
    appendLine(QStringLiteral("<b style=\"color:#2f6fb0\">%1:</b> %2").arg(senderName.toHtmlEscaped(), text.toHtmlEscaped()));
}

void ChatWidget::appendSystemMessage(const QString &text)
{
    appendLine(QStringLiteral("<i style=\"color:#888\">%1</i>").arg(text.toHtmlEscaped()));
}
