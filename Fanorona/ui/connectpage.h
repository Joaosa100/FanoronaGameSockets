#ifndef CONNECTPAGE_H
#define CONNECTPAGE_H

#include <QJsonObject>
#include <QWidget>

#include "piece.h"

class NetworkSession;
class QLabel;
class QLineEdit;
class QPushButton;
class QStackedWidget;

// Cuida da UI e do handshake de rede (troca de "hello" + sorteio de quem comeca) para
// os dois modos: hospedar (QTcpServer aguardando conexao) e entrar (conecta em
// IP:porta digitados). Emite sessionReady() quando a partida esta pronta para comecar.
class ConnectPage : public QWidget {
    Q_OBJECT

public:
    static constexpr quint16 kDefaultPort = 47321;

    explicit ConnectPage(QWidget *parent = nullptr);

    void startHosting(const QString &localName);
    void startJoining(const QString &localName);

signals:
    void sessionReady(NetworkSession *session, Player localPlayer, Player firstTurn,
                       const QString &localName, const QString &remoteName);
    void cancelled();

private slots:
    void onConnectClicked();
    void onCancelClicked();
    void onSessionConnected();
    void onSessionMessage(const QJsonObject &obj);
    void onSessionError(const QString &message);

private:
    void resetSession();
    QString localAddressesText() const;

    QStackedWidget *m_stack;

    // Pagina "hospedando"
    QLabel *m_hostInfoLabel;

    // Pagina "entrando"
    QLineEdit *m_hostEdit;
    QLineEdit *m_portEdit;
    QPushButton *m_connectButton;
    QLabel *m_joinStatusLabel;

    NetworkSession *m_session = nullptr;
    bool m_isHost = false;
    QString m_localName;
    QString m_remoteName;
    bool m_helloSent = false;
};

#endif // CONNECTPAGE_H
