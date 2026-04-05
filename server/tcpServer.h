#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QDataStream>
#include <QObject>
#include <QString>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

class TCPServer : public QObject {
  Q_OBJECT
  // QML_ELEMENT
  // QML_SINGLETON

  Q_PROPERTY(bool isListening READ getServerStatus NOTIFY serverStatusChanged);
  Q_PROPERTY(bool isConnected READ getClientStatus NOTIFY clientStatusChanged);

public:
  TCPServer();

signals:
  void serverStatusChanged(bool);
  void clientStatusChanged(bool);

  void hasReadSome(QString msg);

  void someError(QString err);
  void someMessage(QString msg);

public slots:
  bool start(QString host, int port);
  void stop();
  void disconnect();

  qint64 send(QString msg);
  void received(QString msg);
  void error(QAbstractSocket::SocketError err);

private slots:
  void connected();
  void disconnected();
  void readyRead();

private:
  QTcpServer *tcpServer;
  QTcpSocket *tcpSocket;

  bool serverStatus;
  bool clientStatus;
  quint16 m_nNextBlockSize;

  bool getServerStatus();
  bool getClientStatus();
};

#endif
