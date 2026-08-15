#include "gamepage.h"

#include <algorithm>

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "boardgeometry.h"
#include "boardwidget.h"
#include "capturechoicedialog.h"
#include "chatwidget.h"
#include "moverules.h"

namespace {

bool contains(const std::vector<int> &values, int v)
{
    return std::find(values.begin(), values.end(), v) != values.end();
}

} // namespace

GamePage::GamePage(QWidget *parent)
    : QWidget(parent)
{
    m_boardWidget = new BoardWidget(this);
    m_statusLabel = new QLabel(this);
    m_finishTurnButton = new QPushButton(tr("Finalizar Jogada"), this);
    m_resignButton = new QPushButton(tr("Desistir"), this);
    m_chatWidget = new ChatWidget(this);

    m_finishTurnButton->setEnabled(false);

    auto *boardSideLayout = new QVBoxLayout;
    boardSideLayout->addWidget(m_statusLabel);
    boardSideLayout->addWidget(m_boardWidget, 1);

    auto *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(m_finishTurnButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(m_resignButton);
    boardSideLayout->addLayout(buttonsLayout);

    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->addLayout(boardSideLayout, 3);
    mainLayout->addWidget(m_chatWidget, 1);

    connect(m_boardWidget, &BoardWidget::pointClicked, this, &GamePage::onPointClicked);
    connect(m_finishTurnButton, &QPushButton::clicked, this, &GamePage::onFinishTurnClicked);
    connect(m_resignButton, &QPushButton::clicked, this, &GamePage::onResignClicked);

    m_boardWidget->setBoard(m_board);
    refreshStatusLabel();
}

void GamePage::setLocalPlayer(std::optional<Player> localPlayer)
{
    m_localPlayer = localPlayer;
    // PlayerA nasce nas linhas de cima do tabuleiro; vira a exibicao para que
    // o jogador local sempre veja suas proprias pecas na parte de baixo.
    m_boardWidget->setFlipped(m_localPlayer == Player::PlayerA);
    refreshStatusLabel();
}

void GamePage::setPlayerNames(const QString &nameA, const QString &nameB)
{
    m_nameA = nameA;
    m_nameB = nameB;
    refreshStatusLabel();
}

void GamePage::startGame(Player first)
{
    m_board = Board();
    m_currentTurn = first;
    m_gameOver = false;
    resetTurnState();
    m_boardWidget->setBoard(m_board);
    refreshStatusLabel();
    emit boardChanged(m_board);
    emit turnChanged(m_currentTurn);
}

void GamePage::resetTurnState()
{
    m_selected.reset();
    m_chainActive = false;
    m_visited.clear();
    m_lastDirection.reset();
    m_pendingSteps.clear();
    m_finishTurnButton->setEnabled(false);
    updateHighlights();
    m_boardWidget->setSelected(std::nullopt);
}

bool GamePage::canInteract() const
{
    if (m_gameOver)
        return false;
    return !m_localPlayer.has_value() || *m_localPlayer == m_currentTurn;
}

void GamePage::updateHighlights()
{
    std::vector<int> destinations;
    if (m_chainActive) {
        const Board &board = m_workingBoard;
        auto options = MoveRules::continuationOptions(board, *m_selected, m_currentTurn, m_visited, m_lastDirection);
        for (const auto &[dest, option] : options) {
            if (!contains(destinations, dest))
                destinations.push_back(dest);
        }
    } else if (m_selected) {
        destinations = MoveRules::reachableEmptyNeighbors(m_board, *m_selected);
    }
    m_boardWidget->setHighlighted(destinations);
}

void GamePage::applyStepToWorkingBoard(const MoveStep &step)
{
    Move single;
    single.steps.push_back(step);
    MoveRules::applyMove(m_workingBoard, single);
    m_pendingSteps.push_back(step);
}

void GamePage::onPointClicked(int index)
{
    if (!canInteract())
        return;

    if (m_chainActive) {
        auto options = MoveRules::continuationOptions(m_workingBoard, *m_selected, m_currentTurn, m_visited, m_lastDirection);
        std::vector<MoveOption> matching;
        for (const auto &[dest, option] : options) {
            if (dest == index)
                matching.push_back(option);
        }
        if (matching.empty())
            return; // clique fora dos destinos validos da cadeia, ignora

        const MoveOption chosen = matching.size() == 1 ? matching.front()
                                                         : CaptureChoiceDialog::choose(this, matching);

        const auto dir = BoardGeometry::directionBetween(*m_selected, index);
        MoveStep step;
        step.from = *m_selected;
        step.to = index;
        step.direction = *dir;
        step.captureType = chosen.type;
        step.captured = chosen.captured;
        applyStepToWorkingBoard(step);

        m_visited.push_back(index);
        m_lastDirection = *dir;
        m_selected = index;

        auto next = MoveRules::continuationOptions(m_workingBoard, index, m_currentTurn, m_visited, m_lastDirection);
        if (next.empty()) {
            finishTurn();
        } else {
            m_finishTurnButton->setEnabled(true);
            updateHighlights();
            m_boardWidget->setSelected(m_selected);
            m_boardWidget->setBoard(m_workingBoard);
        }
        return;
    }

    if (m_board.at(index) == m_currentTurn) {
        m_selected = (m_selected && *m_selected == index) ? std::nullopt : std::make_optional(index);
        m_boardWidget->setSelected(m_selected);
        updateHighlights();
        return;
    }

    if (!m_selected)
        return;

    const auto destinations = MoveRules::reachableEmptyNeighbors(m_board, *m_selected);
    if (!contains(destinations, index))
        return;

    const auto options = MoveRules::optionsFor(m_board, *m_selected, index, m_currentTurn);
    const MoveOption chosen = options.size() == 1 ? options.front() : CaptureChoiceDialog::choose(this, options);

    const auto dir = BoardGeometry::directionBetween(*m_selected, index);
    MoveStep step;
    step.from = *m_selected;
    step.to = index;
    step.direction = *dir;
    step.captureType = chosen.type;
    step.captured = chosen.captured;

    m_workingBoard = m_board;
    applyStepToWorkingBoard(step);

    if (chosen.type == CaptureType::None) {
        finishTurn();
        return;
    }

    m_visited = {*m_selected, index};
    m_lastDirection = *dir;
    m_selected = index;
    m_chainActive = true;

    auto next = MoveRules::continuationOptions(m_workingBoard, index, m_currentTurn, m_visited, m_lastDirection);
    if (next.empty()) {
        finishTurn();
    } else {
        m_finishTurnButton->setEnabled(true);
        updateHighlights();
        m_boardWidget->setSelected(m_selected);
        m_boardWidget->setBoard(m_workingBoard);
    }
}

void GamePage::onFinishTurnClicked()
{
    if (m_chainActive)
        finishTurn();
}

void GamePage::finishTurn()
{
    const Move move{m_pendingSteps};
    const Player mover = m_currentTurn;
    m_board = m_workingBoard;
    resetTurnState();
    m_boardWidget->setBoard(m_board);
    emit boardChanged(m_board);
    emit moveFinished(move, mover);
    checkWinnerAndAdvanceTurn();
}

void GamePage::checkWinnerAndAdvanceTurn()
{
    const Player winner = MoveRules::winner(m_board);
    if (winner != Player::None) {
        m_gameOver = true;
        refreshStatusLabel();
        emit gameOver(winner);
        return;
    }
    m_currentTurn = opponent(m_currentTurn);
    refreshStatusLabel();
    emit turnChanged(m_currentTurn);
}

void GamePage::applyExternalMove(const Move &move)
{
    MoveRules::applyMove(m_board, move);
    m_boardWidget->setBoard(m_board);
    emit boardChanged(m_board);
    checkWinnerAndAdvanceTurn();
}

void GamePage::forceGameOver(Player winner)
{
    m_gameOver = true;
    resetTurnState();
    refreshStatusLabel();
    emit gameOver(winner);
}

void GamePage::onResignClicked()
{
    if (m_gameOver)
        return;
    emit resignRequested();
}

void GamePage::refreshStatusLabel()
{
    if (m_gameOver) {
        m_statusLabel->setText(tr("Partida encerrada."));
        return;
    }
    const QString name = m_currentTurn == Player::PlayerA ? m_nameA : m_nameB;
    if (!m_localPlayer || *m_localPlayer == m_currentTurn)
        m_statusLabel->setText(tr("Vez de %1 (%2)").arg(name, m_currentTurn == Player::PlayerA ? tr("claras") : tr("escuras")));
    else
        m_statusLabel->setText(tr("Aguardando %1...").arg(name));
}
