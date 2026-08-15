#ifndef MOVERULES_H
#define MOVERULES_H

#include <optional>
#include <utility>
#include <vector>

#include "board.h"
#include "move.h"
#include "piece.h"

class MoveRules {
public:
    // Vizinhos vazios de 'from' conectados por uma linha (destinos possíveis de um
    // primeiro passo, com ou sem captura).
    static std::vector<int> reachableEmptyNeighbors(const Board& board, int from);

    // Opções disponíveis para mover a peça de 'mover' de 'from' (ocupado por 'mover')
    // para 'to' (vizinho vazio de 'from'). Sempre inclui a opção de movimento simples;
    // inclui Approach e/ou Withdrawal se as respectivas linhas de captura existirem.
    static std::vector<MoveOption> optionsFor(const Board& board, int from, int to, Player mover);

    // Opções de CONTINUAÇÃO de uma cadeia de captura a partir de 'currentPos' (posição
    // atual da peça que já capturou nesse turno). Só retorna opções de captura (nunca
    // movimento simples) para destinos vazios, não presentes em 'visited', e cuja
    // direção não seja igual a 'lastDirection' (proibido repetir direção consecutiva).
    static std::vector<std::pair<int, MoveOption>> continuationOptions(
        const Board& board,
        int currentPos,
        Player mover,
        const std::vector<int>& visited,
        std::optional<Direction> lastDirection);

    // Aplica cada passo do movimento sequencialmente: move a peça e remove as peças
    // capturadas daquele passo. O dono da peça é lido do próprio tabuleiro em
    // 'step.from', não precisa ser informado.
    static void applyMove(Board& board, const Move& move);

    // Retorna o jogador vencedor (quem ainda tem peças) ou Player::None se os dois
    // jogadores ainda têm peças no tabuleiro.
    static Player winner(const Board& board);

private:
    // Anda a partir de 'startExclusive' na direção 'dir', acumulando peças de
    // 'opponentPlayer' contíguas, parando em casa vazia, peça que não seja do
    // oponente, ou borda do tabuleiro.
    static std::vector<int> captureLine(const Board& board, int startExclusive, Direction dir, Player opponentPlayer);
};

#endif // MOVERULES_H
