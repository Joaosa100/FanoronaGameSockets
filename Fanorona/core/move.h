#ifndef MOVE_H
#define MOVE_H

#include <vector>

#include "boardgeometry.h"

enum class CaptureType {
    None,
    Approach,
    Withdrawal
};

struct MoveStep {
    int from = -1;
    int to = -1;
    Direction direction = Direction::N;
    CaptureType captureType = CaptureType::None;
    std::vector<int> captured;
};

struct Move {
    std::vector<MoveStep> steps;

    bool isCapture() const
    {
        return !steps.empty() && steps.front().captureType != CaptureType::None;
    }
};

// Uma opção de destino disponível para um clique (from, to): sempre existe a opção
// "movimento simples" (None), mais Approach/Withdrawal quando a captura correspondente
// existir. Nunca as duas capturas se aplicam ao mesmo tempo automaticamente — o
// jogador escolhe quando ambas estiverem presentes.
struct MoveOption {
    CaptureType type = CaptureType::None;
    std::vector<int> captured;
};

#endif // MOVE_H
