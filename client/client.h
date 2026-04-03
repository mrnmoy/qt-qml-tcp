#ifndef CLIENT_H
#define CLIENT_H

#include <QDataStream>
#include <QString>
#include <QTcpSocket>
#include <QTimer>

class Client : public QObject {
  Q_OBJECT

public:
  Client(const QString host, int port);

  QTcpSocket *tcpSocket;
  bool getStatus();

public slots:
  void closeConnection();
  void connect2host();

signals:
  void statusChanged(bool);
  void hasReadSome(QString msg);

private slots:
  void readyRead();
  void connected();
  void connectionTimeout();

private:
  QString host;
  int port;
  bool status;
  quint16 m_nNextBlockSize;
  QTimer *timeoutTimer;
};

#endif
