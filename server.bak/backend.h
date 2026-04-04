#ifndef BACKEND_H
#define BACKEND_H

#include "server.h"
#include <QObject>

class Backend : public QObject {
  Q_OBJECT
public:
  explicit Backend(QObject *parent = nullptr);

signals:
  void smbConnected();
  void smbDisconnected();
  void newMessage(QString msg);

public slots:
  QString stopClicked();
  QString startClicked();
  QString testClicked();
  void smbConnectedToServer();
  void smbDisconnectedFromServer();
  void gotNewMesssage(QString msg);

private:
  Server *server;
};

#endif
