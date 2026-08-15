#ifndef PIECE_H
#define PIECE_H

enum class Player {
    None,
    PlayerA,
    PlayerB
};

inline Player opponent(Player p)
{
    if (p == Player::PlayerA) return Player::PlayerB;
    if (p == Player::PlayerB) return Player::PlayerA;
    return Player::None;
}

#endif // PIECE_H
