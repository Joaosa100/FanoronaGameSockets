#ifndef BOARDGEOMETRY_H
#define BOARDGEOMETRY_H

#include <optional>
#include <utility>
#include <vector>

enum class Direction {
    N,
    S,
    E,
    W,
    NE,
    NW,
    SE,
    SW
};

Direction opposite(Direction d);
bool isDiagonal(Direction d);

struct RowCol {
    int row;
    int col;
};

class BoardGeometry {
public:
    static constexpr int kRows = 5;
    static constexpr int kCols = 9;
    static constexpr int kPointCount = kRows * kCols;

    static int toIndex(int row, int col);
    static RowCol toRowCol(int index);
    static bool inBounds(int row, int col);

    // A interseção (row,col) tem conexões diagonais quando (row+col) é par —
    // padrão oficial do tabuleiro de Fanorona (malha triangular alternada).
    static bool hasDiagonals(int row, int col);

    // Vizinho de 'index' na direção 'dir', ou nullopt se não existir aresta
    // (fora do tabuleiro, ou diagonal num ponto sem diagonal).
    static std::optional<int> step(int index, Direction dir);

    // Todos os (vizinho, direção) válidos a partir de 'index'.
    static const std::vector<std::pair<int, Direction>>& neighborsOf(int index);

    // Direção de 'from' para 'to', ou nullopt se não forem vizinhos conectados.
    static std::optional<Direction> directionBetween(int from, int to);
};

#endif // BOARDGEOMETRY_H
