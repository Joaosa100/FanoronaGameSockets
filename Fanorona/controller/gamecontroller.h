#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <vector>

#include <QJsonObject>
#include <QObject>
#include <QString>

#include "piece.h"

class GamePage;
class NetworkSession;

namespace protocol {
struct MoveStepDTO;
}
struct Move;

// Liga GamePage (UI+regras de interacao) a NetworkSession (transporte), aplicando a
// atribuicao de cores/turno inicial e traduzindo eventos locais <-> mensagens de rede.
class GameController : public QObject {
    Q_OBJECT

public:
    GameController(NetworkSession *session,
                    GamePage *gamePage,
                    Player localPlayer,
                    Player firstTurn,
                    const QString &localName,
                    const QString &remoteName,
                    QObject *parent = nullptr);

signals:
    void gameOver(Player winner, const QString &reason);

private slots:
    void onNetworkMessage(const QJsonObject &obj);
    void onLocalMoveFinished(const Move &move, Player mover);
    void onChatSubmitted(const QString &text);
    void onResignRequested();
    void onGamePageGameOver(Player winner);
    void onNetworkDisconnected();

private:
    Move reconstructRemoteMove(const std::vector<protocol::MoveStepDTO> &steps) const;

    NetworkSession *m_session;
    GamePage *m_gamePage;
    Player m_localPlayer;
    QString m_localName;
    QString m_remoteName;
    QString m_endReason;
    bool m_gameEnded = false;
};

#endif // GAMECONTROLLER_H
