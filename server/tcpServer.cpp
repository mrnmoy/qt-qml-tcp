#include "tcpServer.h"

TCPServer::TCPServer() : QObject(), m_nNextBlockSize(0) {
  serverStatus = false;
  clientStatus = false;
  tcpServer = new QTcpServer();
}

bool TCPServer::start(QString host, int port) {
  tcpServer->setMaxPendingConnections(1);

  if (!tcpServer->listen(QHostAddress::Any, port)) {
    return false; // port is not available
  }

  QObject::connect(tcpServer, &QTcpServer::newConnection, this,
                   &TCPServer::connected);

  serverStatus = true;
  emit serverStatusChanged(serverStatus);
  return true;
}

void TCPServer::stop() {
  if (tcpServer->isListening()) {
    QObject::disconnect(tcpServer, &QTcpServer::newConnection, this,
                        &TCPServer::connected);

    // QList<QTcpSocket *> tcpClients = server->getClients();
    if (clientStatus)
      disconnect();

    tcpServer->close();
    serverStatus = false;
    emit serverStatusChanged(serverStatus);
  }
}

void TCPServer::disconnect() {
  QObject::disconnect(tcpSocket, &QTcpSocket::connected, 0, 0);
  QObject::disconnect(tcpSocket, &QTcpSocket::readyRead, 0, 0);

  if (tcpSocket->state())
    tcpSocket->disconnectFromHost();
  else
    tcpSocket->abort();
}

void TCPServer::connected() {
  tcpSocket = tcpServer->nextPendingConnection();
  tcpServer->pauseAccepting();

  QObject::connect(tcpSocket, &QTcpSocket::readyRead, this,
                   &TCPServer::readyRead);
  QObject::connect(tcpSocket, &QTcpSocket::disconnected, this,
                   &TCPServer::disconnected);

  clientStatus = true;
  emit clientStatusChanged(clientStatus);
}

void TCPServer::disconnected() {
  // QObject::disconnect(tcpSocket, &QTcpSocket::disconnected, 0, 0);
  tcpServer->resumeAccepting();
  clientStatus = false;
  emit clientStatusChanged(clientStatus);
}

bool TCPServer::getServerStatus() { return serverStatus; }
bool TCPServer::getClientStatus() { return clientStatus; }

void TCPServer::readyRead() {
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

qint64 TCPServer::send(QString msg) {
  QByteArray arrBlock;
  QDataStream out(&arrBlock, QIODevice::WriteOnly);
  out << quint16(0) << msg;

  out.device()->seek(0);
  out << quint16(arrBlock.size() - sizeof(quint16));

  return tcpSocket->write(arrBlock);
}
