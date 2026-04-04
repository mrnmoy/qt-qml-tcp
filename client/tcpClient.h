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

  // Q_PROPERTY(bool currentStatus READ getStatus NOTIFY statusChanged);
  Q_PROPERTY(QString host READ host WRITE setHost);
  Q_PROPERTY(int port READ port WRITE setPort);
  Q_PROPERTY(bool isConnected READ getStatus NOTIFY statusChanged);

public:
  TCPClient();
  // TCPClient(const QString host, int port);
  // explicit Backend(QObject *parent = nullptr);
  bool getStatus();

signals:
  void statusChanged(bool);
  void hasReadSome(QString msg);

  void someError(QString err);
  void someMessage(QString msg);

public slots:
  void connect();
  void disconnect();

  void send(QString msg);
  void received(QString msg);
  void error(QAbstractSocket::SocketError err);

private slots:
  void timeout();
  void connected();
  void disconnected();
  void readyRead();

private:
  QTcpSocket *tcpSocket;

  QString _host;
  int _port;
  bool status;
  quint16 m_nNextBlockSize;
  QTimer *timeoutTimer;

  QString host();
  int port();
  void setHost(QString host);
  void setPort(int port);
};

#endif
