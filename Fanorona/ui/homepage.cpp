#include "homepage.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

HomePage::HomePage(QWidget *parent)
    : QWidget(parent)
{
    auto *title = new QLabel(tr("<h1>Fanorona</h1>"), this);
    title->setAlignment(Qt::AlignCenter);

    auto *subtitle = new QLabel(tr("Jogo de tabuleiro Fanorona em rede"), this);
    subtitle->setAlignment(Qt::AlignCenter);

    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText(tr("Seu nome"));
    m_nameEdit->setMaxLength(24);

    auto *hostButton = new QPushButton(tr("Hospedar Partida"), this);
    auto *joinButton = new QPushButton(tr("Entrar em Partida"), this);
    hostButton->setMinimumHeight(40);
    joinButton->setMinimumHeight(40);

    auto *layout = new QVBoxLayout(this);
    layout->addStretch();
    layout->addWidget(title);
    layout->addWidget(subtitle);
    layout->addSpacing(20);
    layout->addWidget(new QLabel(tr("Nome do jogador:"), this));
    layout->addWidget(m_nameEdit);
    layout->addSpacing(10);
    layout->addWidget(hostButton);
    layout->addWidget(joinButton);
    layout->addStretch();
    layout->setContentsMargins(200, 40, 200, 40);

    connect(hostButton, &QPushButton::clicked, this, [this] {
        emit hostRequested(playerNameOrDefault());
    });
    connect(joinButton, &QPushButton::clicked, this, [this] {
        emit joinRequested(playerNameOrDefault());
    });
}

QString HomePage::playerNameOrDefault() const
{
    const QString name = m_nameEdit->text().trimmed();
    return name.isEmpty() ? tr("Jogador") : name;
}
