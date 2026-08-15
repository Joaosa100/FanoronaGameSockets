#include "networksession.h"

#include <QHostAddress>
#include <QJsonDocument>
#include <QTcpServer>
#include <QTcpSocket>

NetworkSession::NetworkSession(QObject *parent)
    : QObject(parent)
{
}

bool NetworkSession::startHosting(quint16 port, QString *errorMessage)
{
    if (m_server) {
        if (errorMessage)
            *errorMessage = tr("Já está hospedando uma partida.");
        return false;
    }

    m_server = new QTcpServer(this);
    if (!m_server->listen(QHostAddress::Any, port)) {
        if (errorMessage)
            *errorMessage = m_server->errorString();
        m_server->deleteLater();
        m_server = nullptr;
        return false;
    }

    connect(m_server, &QTcpServer::newConnection, this, &NetworkSession::handleNewConnection);
    return true;
}

void NetworkSession::connectToHost(const QString &host, quint16 port)
{
    auto *socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &NetworkSession::handleSocketConnected);
    connect(socket, &QAbstractSocket::errorOccurred, this, &NetworkSession::handleSocketError);
    attachSocket(socket);
    socket->connectToHost(host, port);
}

void NetworkSession::handleNewConnection()
{
    QTcpSocket *socket = m_server->nextPendingConnection();
    if (!socket)
        return;

    // Jogo de 2 jogadores: aceita a primeira conexao e para de escutar por outras.
    m_server->close();

    attachSocket(socket);
    emit connected();
}

void NetworkSession::attachSocket(QTcpSocket *socket)
{
    m_socket = socket;
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkSession::handleReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &NetworkSession::handleDisconnected);
}

void NetworkSession::handleSocketConnected()
{
    emit connected();
}

void NetworkSession::handleReadyRead()
{
    m_buffer += m_socket->readAll();

    int newlineIndex;
    while ((newlineIndex = m_buffer.indexOf('\n')) != -1) {
        const QByteArray line = m_buffer.left(newlineIndex);
        m_buffer.remove(0, newlineIndex + 1);
        if (line.trimmed().isEmpty())
            continue;

        QJsonParseError parseError;
        const QJsonDocument doc = QJsonDocument::fromJson(line, &parseError);
        if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
            emit errorOccurred(tr("Mensagem de rede inválida recebida."));
            continue;
        }
        emit messageReceived(doc.object());
    }
}

void NetworkSession::handleSocketError(QAbstractSocket::SocketError)
{
    if (m_socket)
        emit errorOccurred(m_socket->errorString());
}

void NetworkSession::handleDisconnected()
{
    emit disconnected();
}

void NetworkSession::sendMessage(const QJsonObject &message)
{
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState)
        return;
    QByteArray bytes = QJsonDocument(message).toJson(QJsonDocument::Compact);
    bytes.append('\n');
    m_socket->write(bytes);
}

bool NetworkSession::isConnected() const
{
    return m_socket && m_socket->state() == QAbstractSocket::ConnectedState;
}
