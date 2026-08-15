#include "mainwindow.h"

#include <QMessageBox>
#include <QStackedWidget>

#include "connectpage.h"
#include "gamecontroller.h"
#include "gamepage.h"
#include "homepage.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Fanorona"));
    resize(1100, 700);

    m_stack = new QStackedWidget(this);
    setCentralWidget(m_stack);

    m_homePage = new HomePage(this);
    m_connectPage = new ConnectPage(this);
    m_stack->addWidget(m_homePage);
    m_stack->addWidget(m_connectPage);

    connect(m_homePage, &HomePage::hostRequested, this, [this](const QString &name) {
        m_connectPage->startHosting(name);
        m_stack->setCurrentWidget(m_connectPage);
    });
    connect(m_homePage, &HomePage::joinRequested, this, [this](const QString &name) {
        m_connectPage->startJoining(name);
        m_stack->setCurrentWidget(m_connectPage);
    });
    connect(m_connectPage, &ConnectPage::cancelled, this, [this] {
        m_stack->setCurrentWidget(m_homePage);
    });
    connect(m_connectPage, &ConnectPage::sessionReady, this, &MainWindow::onSessionReady);

    m_stack->setCurrentWidget(m_homePage);
}

void MainWindow::onSessionReady(NetworkSession *session, Player localPlayer, Player firstTurn,
                                 const QString &localName, const QString &remoteName)
{
    if (m_gamePage) {
        m_stack->removeWidget(m_gamePage);
        m_gamePage->deleteLater();
        m_gamePage = nullptr;
    }
    if (m_controller) {
        m_controller->deleteLater();
        m_controller = nullptr;
    }

    m_localPlayer = localPlayer;
    m_gamePage = new GamePage(this);
    m_stack->addWidget(m_gamePage);

    m_controller = new GameController(session, m_gamePage, localPlayer, firstTurn, localName, remoteName, this);
    connect(m_controller, &GameController::gameOver, this, &MainWindow::onGameOver);

    m_stack->setCurrentWidget(m_gamePage);
}

void MainWindow::onGameOver(Player winner, const QString &reason)
{
    const bool localWon = winner == m_localPlayer;
    const QString title = localWon ? tr("Vitória!") : tr("Derrota");
    const QString text = localWon
        ? tr("Você venceu a partida!\nMotivo: %1").arg(reason)
        : tr("Você perdeu a partida.\nMotivo: %1").arg(reason);

    QMessageBox::information(this, title, text);
    m_stack->setCurrentWidget(m_homePage);
}
