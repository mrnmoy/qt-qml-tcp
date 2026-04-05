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

void TCPClient::connect(QString host, int port) {
  timeoutTimer->start(3000);

  tcpSocket->connectToHost(host, port);
  QObject::connect(tcpSocket, &QTcpSocket::connected, this,
                   &TCPClient::connected);
  QObject::connect(tcpSocket, &QTcpSocket::readyRead, this,
                   &TCPClient::readyRead);
}

void TCPClient::disconnect() {
  timeoutTimer->stop();

  QObject::disconnect(tcpSocket, &QTcpSocket::connected, 0, 0);
  QObject::disconnect(tcpSocket, &QTcpSocket::readyRead, 0, 0);

  if (tcpSocket->state())
    tcpSocket->disconnectFromHost();
  else
    tcpSocket->abort();

  status = false;
  emit statusChanged(status);
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
  // QObject::disconnect(tcpSocket, &QTcpSocket::disconnected, 0, 0);
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

    // if (str == "0") {
    //   str = "Connection closed";
    //   disconnect();
    // }

    emit received(str);
    m_nNextBlockSize = 0;
  }
}

void TCPClient::send(QString msg) {
  QByteArray arrBlock;
  QDataStream out(&arrBlock, QIODevice::WriteOnly);
  out << quint16(0) << msg;

  out.device()->seek(0);
  out << quint16(arrBlock.size() - sizeof(quint16));

  tcpSocket->write(arrBlock);
}
