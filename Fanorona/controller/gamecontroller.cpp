#include "gamecontroller.h"

#include "boardgeometry.h"
#include "chatwidget.h"
#include "gamepage.h"
#include "move.h"
#include "moverules.h"
#include "networksession.h"
#include "protocol.h"

GameController::GameController(NetworkSession *session,
                                 GamePage *gamePage,
                                 Player localPlayer,
                                 Player firstTurn,
                                 const QString &localName,
                                 const QString &remoteName,
                                 QObject *parent)
    : QObject(parent)
    , m_session(session)
    , m_gamePage(gamePage)
    , m_localPlayer(localPlayer)
    , m_localName(localName)
    , m_remoteName(remoteName)
{
    m_session->setParent(this);

    const QString nameA = localPlayer == Player::PlayerA ? m_localName : m_remoteName;
    const QString nameB = localPlayer == Player::PlayerB ? m_localName : m_remoteName;
    m_gamePage->setPlayerNames(nameA, nameB);
    m_gamePage->setLocalPlayer(localPlayer);
    m_gamePage->startGame(firstTurn);

    connect(m_gamePage, &GamePage::moveFinished, this, &GameController::onLocalMoveFinished);
    connect(m_gamePage, &GamePage::gameOver, this, &GameController::onGamePageGameOver);
    connect(m_gamePage, &GamePage::resignRequested, this, &GameController::onResignRequested);
    connect(m_gamePage->chat(), &ChatWidget::messageSubmitted, this, &GameController::onChatSubmitted);

    connect(m_session, &NetworkSession::messageReceived, this, &GameController::onNetworkMessage);
    connect(m_session, &NetworkSession::disconnected, this, &GameController::onNetworkDisconnected);

    m_gamePage->chat()->appendSystemMessage(tr("Partida iniciada. %1 joga primeiro.")
                                                 .arg(firstTurn == Player::PlayerA ? nameA : nameB));
}

void GameController::onLocalMoveFinished(const Move &move, Player /*mover*/)
{
    if (m_gameEnded)
        return;

    std::vector<protocol::MoveStepDTO> dtoSteps;
    for (const auto &step : move.steps) {
        protocol::MoveStepDTO dto;
        dto.from = step.from;
        dto.to = step.to;
        dto.captureType = step.captureType;
        dto.captured = step.captured;
        dtoSteps.push_back(dto);
    }
    m_session->sendMessage(protocol::makeMove(dtoSteps));
}

Move GameController::reconstructRemoteMove(const std::vector<protocol::MoveStepDTO> &steps) const
{
    Board scratch = m_gamePage->currentBoard();
    const Player mover = opponent(m_localPlayer);

    Move move;
    for (const auto &dto : steps) {
        const auto options = MoveRules::optionsFor(scratch, dto.from, dto.to, mover);
        MoveOption chosen = options.front();
        for (const auto &option : options) {
            if (option.type == dto.captureType) {
                chosen = option;
                break;
            }
        }

        MoveStep step;
        step.from = dto.from;
        step.to = dto.to;
        step.direction = BoardGeometry::directionBetween(dto.from, dto.to).value_or(Direction::N);
        step.captureType = chosen.type;
        step.captured = chosen.captured;

        Move singleStep;
        singleStep.steps.push_back(step);
        MoveRules::applyMove(scratch, singleStep);

        move.steps.push_back(step);
    }
    return move;
}

void GameController::onNetworkMessage(const QJsonObject &obj)
{
    if (m_gameEnded)
        return;

    const QString type = protocol::messageType(obj);
    if (type == QStringLiteral("move")) {
        m_endReason = tr("captura de todas as peças");
        const Move move = reconstructRemoteMove(protocol::moveSteps(obj));
        m_gamePage->applyExternalMove(move);
    } else if (type == QStringLiteral("chat")) {
        m_gamePage->chat()->appendRemoteMessage(m_remoteName, protocol::chatText(obj));
    } else if (type == QStringLiteral("resign")) {
        m_endReason = tr("%1 desistiu").arg(m_remoteName);
        m_gamePage->forceGameOver(m_localPlayer);
    }
}

void GameController::onChatSubmitted(const QString &text)
{
    m_session->sendMessage(protocol::makeChat(text));
    m_gamePage->chat()->appendLocalMessage(m_localName, text);
}

void GameController::onResignRequested()
{
    if (m_gameEnded)
        return;
    m_session->sendMessage(protocol::makeResign());
    m_endReason = tr("%1 desistiu").arg(m_localName);
    m_gamePage->forceGameOver(opponent(m_localPlayer));
}

void GameController::onGamePageGameOver(Player winner)
{
    if (m_gameEnded)
        return;
    m_gameEnded = true;
    if (m_endReason.isEmpty())
        m_endReason = tr("captura de todas as peças");
    emit gameOver(winner, m_endReason);
}

void GameController::onNetworkDisconnected()
{
    if (!m_gameEnded)
        m_gamePage->chat()->appendSystemMessage(tr("O oponente desconectou."));
}
