#ifndef BOARD_H
#define BOARD_H

#include <array>

#include "boardgeometry.h"
#include "piece.h"

class Board {
public:
    Board();

    Player at(int index) const;
    void setAt(int index, Player p);

    int countPieces(Player p) const;

    Board clone() const { return *this; }

private:
    std::array<Player, BoardGeometry::kPointCount> m_cells;
};

#endif // BOARD_H
