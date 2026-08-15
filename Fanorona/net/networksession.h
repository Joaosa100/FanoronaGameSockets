#ifndef NETWORKSESSION_H
#define NETWORKSESSION_H

#include <QAbstractSocket>
#include <QByteArray>
#include <QJsonObject>
#include <QObject>

class QTcpServer;
class QTcpSocket;

// Uma conexao ponto-a-ponto entre os dois jogadores: o host abre um QTcpServer e
// aceita a primeira conexao recebida; o client conecta diretamente via IP:porta.
// Depois de conectados, os dois lados usam a mesma API (sendMessage/messageReceived)
// independente de quem e host ou client. Mensagens sao objetos JSON delimitados por
// '\n' (NDJSON) sobre o unico QTcpSocket resultante.
class NetworkSession : public QObject {
    Q_OBJECT

public:
    explicit NetworkSession(QObject *parent = nullptr);

    bool startHosting(quint16 port, QString *errorMessage = nullptr);
    void connectToHost(const QString &host, quint16 port);

    void sendMessage(const QJsonObject &message);
    bool isConnected() const;

signals:
    void connected();
    void disconnected();
    void messageReceived(const QJsonObject &message);
    void errorOccurred(const QString &message);

private slots:
    void handleNewConnection();
    void handleSocketConnected();
    void handleReadyRead();
    void handleSocketError(QAbstractSocket::SocketError socketError);
    void handleDisconnected();

private:
    void attachSocket(QTcpSocket *socket);

    QTcpServer *m_server = nullptr;
    QTcpSocket *m_socket = nullptr;
    QByteArray m_buffer;
};

#endif // NETWORKSESSION_H
