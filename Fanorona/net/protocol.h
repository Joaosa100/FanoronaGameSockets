#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <vector>

#include <QJsonObject>
#include <QString>

#include "move.h"

// Protocolo de rede: mensagens JSON (uma por linha, NDJSON) trocadas entre host e
// client atraves de um unico QTcpSocket. Ver NetworkSession para o framing.
namespace protocol {

// Um passo de movimento como trafega na rede: from/to/captureType descrevem o
// movimento; 'captured' vai junto so como checagem de sanidade/depuracao -- quem
// recebe NUNCA deve confiar cegamente nele, e sim recomputar com MoveRules.
struct MoveStepDTO {
    int from = -1;
    int to = -1;
    CaptureType captureType = CaptureType::None;
    std::vector<int> captured;
};

QString captureTypeToString(CaptureType type);
CaptureType captureTypeFromString(const QString &s);

QJsonObject makeHello(const QString &playerName);
QJsonObject makeStartGame(bool hostPlaysFirst);
QJsonObject makeMove(const std::vector<MoveStepDTO> &steps);
QJsonObject makeChat(const QString &text);
QJsonObject makeResign();

// "hello" | "start_game" | "move" | "chat" | "resign" | "" (desconhecido/invalido)
QString messageType(const QJsonObject &obj);

QString helloPlayerName(const QJsonObject &obj);
bool startGameHostPlaysFirst(const QJsonObject &obj);
std::vector<MoveStepDTO> moveSteps(const QJsonObject &obj);
QString chatText(const QJsonObject &obj);

} // namespace protocol

#endif // PROTOCOL_H
