#include "boardgeometry.h"

#include <array>

Direction opposite(Direction d)
{
    switch (d) {
    case Direction::N: return Direction::S;
    case Direction::S: return Direction::N;
    case Direction::E: return Direction::W;
    case Direction::W: return Direction::E;
    case Direction::NE: return Direction::SW;
    case Direction::NW: return Direction::SE;
    case Direction::SE: return Direction::NW;
    case Direction::SW: return Direction::NE;
    }
    return d;
}

bool isDiagonal(Direction d)
{
    return d == Direction::NE || d == Direction::NW || d == Direction::SE || d == Direction::SW;
}

namespace {

struct DirectionDelta {
    Direction dir;
    int dRow;
    int dCol;
};

constexpr std::array<DirectionDelta, 8> kDeltas{{
    {Direction::N, -1, 0},
    {Direction::S, 1, 0},
    {Direction::E, 0, 1},
    {Direction::W, 0, -1},
    {Direction::NE, -1, 1},
    {Direction::NW, -1, -1},
    {Direction::SE, 1, 1},
    {Direction::SW, 1, -1},
}};

std::vector<std::vector<std::pair<int, Direction>>> buildNeighborTable()
{
    std::vector<std::vector<std::pair<int, Direction>>> table(BoardGeometry::kPointCount);
    for (int index = 0; index < BoardGeometry::kPointCount; ++index) {
        for (const auto& delta : kDeltas) {
            if (auto neighbor = BoardGeometry::step(index, delta.dir))
                table[index].push_back({*neighbor, delta.dir});
        }
    }
    return table;
}

} // namespace

int BoardGeometry::toIndex(int row, int col)
{
    return row * kCols + col;
}

RowCol BoardGeometry::toRowCol(int index)
{
    return {index / kCols, index % kCols};
}

bool BoardGeometry::inBounds(int row, int col)
{
    return row >= 0 && row < kRows && col >= 0 && col < kCols;
}

bool BoardGeometry::hasDiagonals(int row, int col)
{
    return (row + col) % 2 == 0;
}

std::optional<int> BoardGeometry::step(int index, Direction dir)
{
    const RowCol rc = toRowCol(index);
    if (isDiagonal(dir) && !hasDiagonals(rc.row, rc.col))
        return std::nullopt;

    for (const auto& delta : kDeltas) {
        if (delta.dir != dir)
            continue;
        const int newRow = rc.row + delta.dRow;
        const int newCol = rc.col + delta.dCol;
        if (!inBounds(newRow, newCol))
            return std::nullopt;
        return toIndex(newRow, newCol);
    }
    return std::nullopt;
}

const std::vector<std::pair<int, Direction>>& BoardGeometry::neighborsOf(int index)
{
    static const std::vector<std::vector<std::pair<int, Direction>>> table = buildNeighborTable();
    return table[index];
}

std::optional<Direction> BoardGeometry::directionBetween(int from, int to)
{
    for (const auto& [neighbor, dir] : neighborsOf(from)) {
        if (neighbor == to)
            return dir;
    }
    return std::nullopt;
}
