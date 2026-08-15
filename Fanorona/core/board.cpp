#include "board.h"

Board::Board()
{
    m_cells.fill(Player::None);

    for (int col = 0; col < BoardGeometry::kCols; ++col) {
        setAt(BoardGeometry::toIndex(0, col), Player::PlayerA);
        setAt(BoardGeometry::toIndex(1, col), Player::PlayerA);
        setAt(BoardGeometry::toIndex(3, col), Player::PlayerB);
        setAt(BoardGeometry::toIndex(4, col), Player::PlayerB);
    }

    // Linha do meio: alterna entre os dois jogadores, com o ponto central vazio.
    static const std::array<Player, BoardGeometry::kCols> kMiddleRow{
        Player::PlayerB, Player::PlayerA, Player::PlayerB, Player::PlayerA,
        Player::None,
        Player::PlayerB, Player::PlayerA, Player::PlayerB, Player::PlayerA,
    };
    for (int col = 0; col < BoardGeometry::kCols; ++col)
        setAt(BoardGeometry::toIndex(2, col), kMiddleRow[col]);
}

Player Board::at(int index) const
{
    return m_cells[index];
}

void Board::setAt(int index, Player p)
{
    m_cells[index] = p;
}

int Board::countPieces(Player p) const
{
    int count = 0;
    for (Player cell : m_cells) {
        if (cell == p)
            ++count;
    }
    return count;
}
