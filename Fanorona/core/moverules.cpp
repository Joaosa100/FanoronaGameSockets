#include "moverules.h"

#include <algorithm>

namespace {

bool contains(const std::vector<int>& values, int v)
{
    return std::find(values.begin(), values.end(), v) != values.end();
}

} // namespace

std::vector<int> MoveRules::reachableEmptyNeighbors(const Board& board, int from)
{
    std::vector<int> result;
    for (const auto& [neighbor, dir] : BoardGeometry::neighborsOf(from)) {
        if (board.at(neighbor) == Player::None)
            result.push_back(neighbor);
    }
    return result;
}

std::vector<int> MoveRules::captureLine(const Board& board, int startExclusive, Direction dir, Player opponentPlayer)
{
    std::vector<int> captured;
    std::optional<int> current = BoardGeometry::step(startExclusive, dir);
    while (current && board.at(*current) == opponentPlayer) {
        captured.push_back(*current);
        current = BoardGeometry::step(*current, dir);
    }
    return captured;
}

std::vector<MoveOption> MoveRules::optionsFor(const Board& board, int from, int to, Player mover)
{
    std::vector<MoveOption> options;
    options.push_back({CaptureType::None, {}});

    const auto dir = BoardGeometry::directionBetween(from, to);
    if (!dir)
        return options; // 'to' não é vizinho de 'from', sem opções de captura

    const Player rival = opponent(mover);

    const auto approach = captureLine(board, to, *dir, rival);
    if (!approach.empty())
        options.push_back({CaptureType::Approach, approach});

    const auto withdrawal = captureLine(board, from, opposite(*dir), rival);
    if (!withdrawal.empty())
        options.push_back({CaptureType::Withdrawal, withdrawal});

    return options;
}

std::vector<std::pair<int, MoveOption>> MoveRules::continuationOptions(
    const Board& board,
    int currentPos,
    Player mover,
    const std::vector<int>& visited,
    std::optional<Direction> lastDirection)
{
    std::vector<std::pair<int, MoveOption>> result;

    for (const auto& [neighbor, dir] : BoardGeometry::neighborsOf(currentPos)) {
        if (board.at(neighbor) != Player::None)
            continue;
        if (contains(visited, neighbor))
            continue;
        if (lastDirection && dir == *lastDirection)
            continue;

        const auto options = optionsFor(board, currentPos, neighbor, mover);
        for (const auto& option : options) {
            if (option.type != CaptureType::None)
                result.push_back({neighbor, option});
        }
    }

    return result;
}

void MoveRules::applyMove(Board& board, const Move& move)
{
    for (const auto& step : move.steps) {
        const Player mover = board.at(step.from);
        board.setAt(step.from, Player::None);
        board.setAt(step.to, mover);
        for (int captured : step.captured)
            board.setAt(captured, Player::None);
    }
}

Player MoveRules::winner(const Board& board)
{
    if (board.countPieces(Player::PlayerA) == 0)
        return Player::PlayerB;
    if (board.countPieces(Player::PlayerB) == 0)
        return Player::PlayerA;
    return Player::None;
}
