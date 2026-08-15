#ifndef GAMEPAGE_H
#define GAMEPAGE_H

#include <optional>
#include <vector>

#include <QWidget>

#include "board.h"
#include "move.h"
#include "piece.h"

class BoardWidget;
class ChatWidget;
class QLabel;
class QPushButton;

class GamePage : public QWidget {
    Q_OBJECT

public:
    explicit GamePage(QWidget *parent = nullptr);

    // nullopt = modo local (hot-seat): os dois jogadores sao controlaveis nesta
    // mesma instancia, alternando turnos. Um valor definido restringe a interacao
    // por clique ao turno desse jogador (usado no modo em rede).
    void setLocalPlayer(std::optional<Player> localPlayer);
    // Nomes exibidos para cada cor (independente de quem e local/remoto).
    void setPlayerNames(const QString &nameA, const QString &nameB);

    // Reinicia para a posicao inicial padrao e comeca com 'first' jogando.
    void startGame(Player first);

    // Aplica um movimento ja validado (tipicamente recebido pela rede) diretamente,
    // sem passar pelo fluxo de clique/selecao.
    void applyExternalMove(const Move &move);

    // Encerra a partida imediatamente declarando 'winner' vencedor (desistencia).
    void forceGameOver(Player winner);

    Board currentBoard() const { return m_board; }
    Player currentTurn() const { return m_currentTurn; }
    bool isGameOver() const { return m_gameOver; }

    ChatWidget *chat() const { return m_chatWidget; }

signals:
    void moveFinished(const Move &move, Player mover);
    void boardChanged(const Board &board);
    void turnChanged(Player turn);
    void gameOver(Player winner);
    void resignRequested();

private slots:
    void onPointClicked(int index);
    void onFinishTurnClicked();
    void onResignClicked();

private:
    bool canInteract() const;
    void resetTurnState();
    void applyStepToWorkingBoard(const MoveStep &step);
    void finishTurn();
    void refreshStatusLabel();
    void updateHighlights();
    void checkWinnerAndAdvanceTurn();

    BoardWidget *m_boardWidget;
    ChatWidget *m_chatWidget;
    QLabel *m_statusLabel;
    QPushButton *m_finishTurnButton;
    QPushButton *m_resignButton;

    Board m_board;
    Board m_workingBoard;
    Player m_currentTurn = Player::PlayerA;
    std::optional<Player> m_localPlayer;
    QString m_nameA = tr("Jogador A");
    QString m_nameB = tr("Jogador B");

    std::optional<int> m_selected;
    bool m_chainActive = false;
    std::vector<int> m_visited;
    std::optional<Direction> m_lastDirection;
    std::vector<MoveStep> m_pendingSteps;

    bool m_gameOver = false;
};

#endif // GAMEPAGE_H
