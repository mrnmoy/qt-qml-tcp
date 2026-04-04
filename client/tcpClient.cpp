#include "tcpClient.h"

TCPClient::TCPClient() : QObject(), m_nNextBlockSize(0) {
  status = false;
  tcpSocket = new QTcpSocket();

  timeoutTimer = new QTimer();
  timeoutTimer->setSingleShot(true);
  QObject::connect(timeoutTimer, &QTimer::timeout, this, &TCPClient::timeout);

  QObject::connect(tcpSocket, &QTcpSocket::disconnected, this,
                   &TCPClient::disconnected);
}

QString TCPClient::host() { return _host; };
int TCPClient::port() { return _port; };

void TCPClient::setHost(QString newHost) { _host = newHost; }
void TCPClient::setPort(int newPort) { _port = newPort; }

void TCPClient::connect() {
  timeoutTimer->start(3000);

  tcpSocket->connectToHost(_host, _port);
  QObject::connect(tcpSocket, &QTcpSocket::connected, this,
                   &TCPClient::connected);
  QObject::connect(tcpSocket, &QTcpSocket::readyRead, this,
                   &TCPClient::readyRead);
}

void TCPClient::disconnect() {
  timeoutTimer->stop();

  QObject::disconnect(tcpSocket, &QTcpSocket::connected, 0, 0);
  QObject::disconnect(tcpSocket, &QTcpSocket::readyRead, 0, 0);

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

void TCPClient::timeout() {
  if (tcpSocket->state() == QAbstractSocket::ConnectingState) {
    tcpSocket->abort();
    // emit tcpSocket->error(QAbstractSocket::SocketTimeoutError); // Error not
    // found
  }
}

void TCPClient::connected() {
  status = true;
  emit statusChanged(status);
}

void TCPClient::disconnected() {
  status = false;
  emit statusChanged(status);
}

bool TCPClient::getStatus() { return status; }

void TCPClient::readyRead() {
  QDataStream in(tcpSocket);
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
      disconnect();
    }

    emit hasReadSome(str);
    m_nNextBlockSize = 0;
  }
}

void TCPClient::send(QString msg) {
  QByteArray arrBlock;
  QDataStream out(&arrBlock, QIODevice::WriteOnly);
  // out.setVersion(QDataStream::Qt_5_10);
  out << quint16(0) << msg;

  out.device()->seek(0);
  out << quint16(arrBlock.size() - sizeof(quint16));

  tcpSocket->write(arrBlock);
}

void TCPClient::received(QString msg) { emit someMessage(msg); }

void TCPClient::error(QAbstractSocket::SocketError err) {
  QString strError = "unknown";
  switch (err) {
  case 0:
    strError = "Connection was refused";
    break;
  case 1:
    strError = "Remote host closed the connection";
    break;
  case 2:
    strError = "Host address was not found";
    break;
  case 5:
    strError = "Connection timed out";
    break;
  default:
    strError = "Unknown error";
  }

  emit someError(strError);
}
