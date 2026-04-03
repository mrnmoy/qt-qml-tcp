#include "client.h"

Client::Client(const QString hostAddress, int portNumber)
    : QObject(), m_nNextBlockSize(0) {
  status = false;
  tcpSocket = new QTcpSocket();

  host = hostAddress;
  port = portNumber;

  timeoutTimer = new QTimer();
  timeoutTimer->setSingleShot(true);
  connect(timeoutTimer, &QTimer::timeout, this, &Client::connectionTimeout);

  connect(tcpSocket, &QTcpSocket::disconnected, this, &Client::closeConnection);
}

void Client::connect2host() {
  timeoutTimer->start(3000);

  tcpSocket->connectToHost(host, port);
  connect(tcpSocket, &QTcpSocket::connected, this, &Client::connected);
  connect(tcpSocket, &QTcpSocket::readyRead, this, &Client::readyRead);
}

void Client::connectionTimeout() {
  // qDebug() << tcpSocket->state();
  if (tcpSocket->state() == QAbstractSocket::ConnectingState) {
    tcpSocket->abort();
    // emit tcpSocket->error(QAbstractSocket::SocketTimeoutError); // Error not
    // found
  }
}

void Client::connected() {
  status = true;
  emit statusChanged(status);
}

bool Client::getStatus() { return status; }

void Client::readyRead() {
  QDataStream in(tcpSocket);
  // in.setVersion(QDataStream::Qt_5_10);
  for (;;) {
    if (!m_nNextBlockSize) {
      if (tcpSocket->bytesAvailable() < sizeof(quint16)) {
        break;
      }
      in >> m_nNextBlockSize;
    }

    if (tcpSocket->bytesAvailable() < m_nNextBlockSize) {
      break;
    }

    QString str;
    in >> str;

    if (str == "0") {
      str = "Connection closed";
      closeConnection();
    }

    emit hasReadSome(str);
    m_nNextBlockSize = 0;
  }
}

// void Client::gotDisconnection()
//{
//     status = false;
//     emit statusChanged(status);
// }

void Client::closeConnection() {
  timeoutTimer->stop();

  // qDebug() << tcpSocket->state();
  disconnect(tcpSocket, &QTcpSocket::connected, 0, 0);
  disconnect(tcpSocket, &QTcpSocket::readyRead, 0, 0);

  bool shouldEmit = false;
  switch (tcpSocket->state()) {
  case 0:
    tcpSocket->disconnectFromHost();
    shouldEmit = true;
    break;
  case 2:
    tcpSocket->abort();
    shouldEmit = true;
    break;
  default:
    tcpSocket->abort();
  }

  if (shouldEmit) {
    status = false;
    emit statusChanged(status);
  }
}
