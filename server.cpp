#include "server.h"

Server::Server(QObject *parent)
    : QTcpServer{parent}
{
    // Ставим на прослушку порт по определённому ip адресу
    //QHostAddress("192.168.100.5") - для глобальной сети
    //QHostAddress::LocalHost - для локальной сети
    if (this->listen(QHostAddress("192.168.100.5"), 27777)) {
        qDebug() << "The server is listening. Ip:" << this->serverAddress() << "Port:" << this->serverPort();
    } else {
        qDebug() << "Server not working.";
    }
}

// Обрабатываем входящее соединение
void Server::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket* socket = new QTcpSocket(); // Создаём соккет для клиента
    socket->setSocketDescriptor(socketDescriptor); // Сохраняем подключившегося пользователя
    connect(socket, &QTcpSocket::readyRead, this, &Server::readyRead); // При получении сообщения от клиента обрабатываем его
    connect(socket, &QTcpSocket::disconnected, this, &Server::clientDisconnected); // При отключении клиента от сервера удаляем сокет
    clients.push_back(socket); // Храним клиентов в контейнере
    qDebug() << "Client connected. Deskriptor:" << socketDescriptor << "Ip:" << socket->peerAddress() << "Port:" << socket->peerPort();
    emit sendIpAdressPortClient(socket->peerAddress().toString(), QString::number(socket->peerPort())); // Сигнал для добавления в таблицу на форме
    emit updateCountClient(this->clients.size()); // Сигнал об изменении количества подключенных клиентов на форме
}

// Получаем информацию, что написал клиент
void Server::readyRead()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender()); // Получаем отправителя
    QDataStream in(socket); // Для работы с данными в соккете
    in.setVersion(QDataStream::Version::Qt_5_9); // Для исключения ошибок

    if (in.status() == QDataStream::Ok) {
        qDebug() << "Read data client. Descriptor:" << socket->socketDescriptor() << "Ip:" << socket->peerAddress() << "Port:" << socket->peerPort();
        QString message;
        in >> message;
        qDebug() << message;
        emit sendMessageFromClient(message, socket->peerAddress().toString(), QString::number(socket->peerPort()));
    } else {
        qDebug() << "Error read" << in.status();
    }
}

// Удаляем клиента из контейнера
void Server::deleteClientFromVector(QString ip, QString port)
{
    QMutableVectorIterator<QTcpSocket*> mit(this->clients); // Для удаления элемента из QVector (соккета)
    while(mit.hasNext()) {
        QTcpSocket *socket = mit.next(); // Храним тут следующий элемент
        // Если в соккете ip и port совпадают, то удаляем из вектора и обновляем таблицу
        if (QString::number(socket->peerPort()) == port && socket->peerAddress().toString() == ip) {
            mit.remove(); // Удаляем элемент вектора
            emit socket->disconnected(); // Удаляем соккет
            break;
        }
    }
}

// Когда клиент отключается
void Server::clientDisconnected()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender()); // Получаем отправителя
    // Если пользователь сам отсоединился от сервера, то не забываем удалить из контейнера
    if (socket->socketDescriptor() == -1) {
        deleteClientFromVector(socket->peerAddress().toString(), QString::number(socket->peerPort()));
    }
    qDebug() << "Client disconnected. Descriptor:" <<  socket->socketDescriptor() << "Ip:" << socket->peerAddress() << "Port:" << socket->peerPort();
    disconnect(socket, &QTcpSocket::readyRead, this, &Server::readyRead);
    disconnect(socket, &QTcpSocket::disconnected, this, &Server::clientDisconnected); // При отключении клиента от сервера удаляем сокет
    emit deleteFromTable(socket->peerAddress().toString(), QString::number(socket->peerPort())); // Шлём сигнал для удаления клиента из таблицы
    emit updateCountClient(this->clients.size());
    socket->deleteLater(); // Удаляем соккет
}
