#ifndef SERVER_H
#define SERVER_H

#include <QDataStream>
#include <QList>
#include <QTcpServer>
#include <QTcpSocket>

class Server : public QObject {
  Q_OBJECT

public:
  Server();
  QTcpServer *tcpServer;
  QList<QTcpSocket *> getClients();

public slots:
  virtual void newConnection();
  void readClient();
  void gotDisconnection();
  qint64 sendToClient(QTcpSocket *socket, const QString &str);

signals:
  void gotNewMesssage(QString msg);
  void smbDisconnected();

private:
  quint16 m_nNextBlockSize;
  QList<QTcpSocket *> clients;
};

#endif
