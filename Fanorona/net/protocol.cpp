#include "protocol.h"

#include <QJsonArray>

namespace protocol {

QString captureTypeToString(CaptureType type)
{
    switch (type) {
    case CaptureType::None: return QStringLiteral("none");
    case CaptureType::Approach: return QStringLiteral("approach");
    case CaptureType::Withdrawal: return QStringLiteral("withdrawal");
    }
    return QStringLiteral("none");
}

CaptureType captureTypeFromString(const QString &s)
{
    if (s == QStringLiteral("approach")) return CaptureType::Approach;
    if (s == QStringLiteral("withdrawal")) return CaptureType::Withdrawal;
    return CaptureType::None;
}

QJsonObject makeHello(const QString &playerName)
{
    QJsonObject obj;
    obj["type"] = QStringLiteral("hello");
    obj["name"] = playerName;
    return obj;
}

QJsonObject makeStartGame(bool hostPlaysFirst)
{
    QJsonObject obj;
    obj["type"] = QStringLiteral("start_game");
    obj["firstPlayer"] = hostPlaysFirst ? QStringLiteral("host") : QStringLiteral("client");
    return obj;
}

QJsonObject makeMove(const std::vector<MoveStepDTO> &steps)
{
    QJsonArray array;
    for (const auto &step : steps) {
        QJsonObject stepObj;
        stepObj["from"] = step.from;
        stepObj["to"] = step.to;
        stepObj["captureType"] = captureTypeToString(step.captureType);
        QJsonArray capturedArray;
        for (int c : step.captured)
            capturedArray.append(c);
        stepObj["captured"] = capturedArray;
        array.append(stepObj);
    }
    QJsonObject obj;
    obj["type"] = QStringLiteral("move");
    obj["steps"] = array;
    return obj;
}

QJsonObject makeChat(const QString &text)
{
    QJsonObject obj;
    obj["type"] = QStringLiteral("chat");
    obj["text"] = text;
    return obj;
}

QJsonObject makeResign()
{
    QJsonObject obj;
    obj["type"] = QStringLiteral("resign");
    return obj;
}

QString messageType(const QJsonObject &obj)
{
    return obj.value(QStringLiteral("type")).toString();
}

QString helloPlayerName(const QJsonObject &obj)
{
    return obj.value(QStringLiteral("name")).toString();
}

bool startGameHostPlaysFirst(const QJsonObject &obj)
{
    return obj.value(QStringLiteral("firstPlayer")).toString() == QStringLiteral("host");
}

std::vector<MoveStepDTO> moveSteps(const QJsonObject &obj)
{
    std::vector<MoveStepDTO> steps;
    const QJsonArray array = obj.value(QStringLiteral("steps")).toArray();
    for (const auto &value : array) {
        const QJsonObject stepObj = value.toObject();
        MoveStepDTO step;
        step.from = stepObj.value(QStringLiteral("from")).toInt(-1);
        step.to = stepObj.value(QStringLiteral("to")).toInt(-1);
        step.captureType = captureTypeFromString(stepObj.value(QStringLiteral("captureType")).toString());
        for (const auto &c : stepObj.value(QStringLiteral("captured")).toArray())
            step.captured.push_back(c.toInt());
        steps.push_back(step);
    }
    return steps;
}

QString chatText(const QJsonObject &obj)
{
    return obj.value(QStringLiteral("text")).toString();
}

} // namespace protocol
