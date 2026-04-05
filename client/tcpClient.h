#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QDataStream>
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QTimer>

class TCPClient : public QObject {
  Q_OBJECT
  // QML_ELEMENT
  // QML_SINGLETON

  Q_PROPERTY(bool isConnected READ getStatus NOTIFY statusChanged);

public:
  TCPClient();
  bool getStatus();

signals:
  void statusChanged(bool);
  void received(QString msg);
  void error(QString err);
  // void error(QAbstractSocket::SocketError err);

public slots:
  void connect(QString host, int port);
  void disconnect();
  void send(QString msg);

private slots:
  void timeout();
  void connected();
  void disconnected();
  void readyRead();

private:
  QTcpSocket *tcpSocket;

  bool status;
  quint16 m_nNextBlockSize;
  QTimer *timeoutTimer;
};

#endif
