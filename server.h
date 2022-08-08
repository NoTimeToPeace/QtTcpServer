#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QByteArray>
#include <QString>
#include <QDataStream>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);

private:
    void sendToClient(QString &); // Для отправки сообщений клиентам

private:
    QVector<QTcpSocket*> clients; // Храним тут пользователей

public slots:
    void incomingConnection (qintptr socketDescriptor ) override; // Для входящих подключений
    void readyRead(); // Для обработки входящих сообщений
    void clientDisconnected(); // Для отключения клиента
    void deleteClientFromVector(QString, QString); // Для удаления клиента из контейнера

signals:
    void sendIpAdressPortClient(QString, QString); // Сигнал для добавления клиента в таблицу на сервере
    void sendMessageFromClient(QString message, QString ip, QString port); // Сигнал для добавления сообщения от клиента на сервере
    void updateCountClient(int); // Сигнал для обновления количества клиентов на сервере
    void deleteFromTable(QString, QString); // Сигнал для удаления клиента из таблицы формы
};

#endif // SERVER_H
