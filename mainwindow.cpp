#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , server(nullptr)
{
    ui->setupUi(this);

    // Устанавливаем окно по центру экрана
    this->setGeometry(QApplication::desktop()->screenGeometry(0).width()/2-400, QApplication::desktop()->screenGeometry(0).height()/2-175, 1000, 500); // Устанавливаем окно в центр экрана

    // Настраиваем размер столбцов таблицы
    ui->tableWidget->setColumnWidth(0, 150);
    ui->tableWidget->setColumnWidth(1, 200);
    ui->tableWidget->setColumnWidth(2, 430);
    ui->tableWidget->setColumnWidth(3, 185);

    // Привязываем кнопку к запуску сервера
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::StartServer);
}

MainWindow::~MainWindow()
{
    if (this->server != nullptr) {
        disconnect(server, &Server::sendIpAdressPortClient, this, &MainWindow::IpAdressPortClient);
        disconnect(server, &Server::sendMessageFromClient, this, &MainWindow::MessageFromClient);
        disconnect(server, &Server::updateCountClient, this, &MainWindow::CountClient);
        disconnect(this, &MainWindow::deleteClient, server, &Server::deleteClientFromVector);
        disconnect(server, &Server::deleteFromTable, this, &MainWindow::UpdateTable);
        delete server;
    }
    delete ui;
}

// Нажатие по кнопке запустить сервер
void MainWindow::StartServer()
{
    if (this->server == nullptr) {
        this->server = new Server(this);
        // Указываем порт, на котором запустили сервер
        ui->label->setText(ui->label->text() + QString::number(this->server->serverPort()));

        // Добавление в таблицу при подключении и удаление при отключении
        connect(server, &Server::sendIpAdressPortClient, this, &MainWindow::IpAdressPortClient); // Для добавления в таблицу клиента
        connect(server, &Server::sendMessageFromClient, this, &MainWindow::MessageFromClient); // Для записи сообщения клиента в таблицу
        connect(server, &Server::updateCountClient, this, &MainWindow::CountClient); // Для обновления клиентов на сервере
        connect(this, &MainWindow::deleteClient, server, &Server::deleteClientFromVector); // Для удаления клиента с сервера
        connect(server, &Server::deleteFromTable, this, &MainWindow::UpdateTable); // Для удаления клиента из таблицы
    }
}

// Добавляем клиента в таблицу
void MainWindow::IpAdressPortClient(QString ip, QString port)
{
    // Добавляем строку в таблицу
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());

    // Добавляем в таблицу port клиента
    QTableWidgetItem *newItemPort = new QTableWidgetItem(port);
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 0, newItemPort);

    // Добавляем в таблицу ip адрес клиента
    QTableWidgetItem *newItemIp = new QTableWidgetItem(ip);
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 1, newItemIp);

    // Добавляем пустое сообщение в таблицу
    QTableWidgetItem *newItemMessage = new QTableWidgetItem("");
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 2, newItemMessage);

    // Добавляем кнопку для кика клиента
    QPushButton* btn_kick = new QPushButton();
    btn_kick->setText("Kick");
    btn_kick->setProperty("row", ui->tableWidget->rowCount() - 1);
    connect(btn_kick, &QPushButton::clicked, this, &MainWindow::DeleteFromTableAndDisconnect);
    ui->tableWidget->setCellWidget(ui->tableWidget->rowCount() - 1, 3, btn_kick);
}

// Обновляем сообщение клиента в таблице
void MainWindow::MessageFromClient(QString message, QString ip, QString port)
{
    // Ищем в таблице клиента с ip и port
    for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
        if (ui->tableWidget->item(i, 0)->text() == port && ui->tableWidget->item(i, 1)->text() == ip) {
            ui->tableWidget->item(i, 2)->setText(message);
       }
    }
}

// Вычисляем клиента, которого нужно удалить из таблицы и передаем серверу удаление из контейнера
void MainWindow::DeleteFromTableAndDisconnect()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender()); // Узнаем отправителя (Кнопку)
    QString ipForDisconnect = ui->tableWidget->item(button->property("row").toInt(), 1)->text(); // ip который нужно кикнуть
    QString portForDisconnect = ui->tableWidget->item(button->property("row").toInt(), 0)->text(); // port который нужно кикнуть    
    emit deleteClient(ipForDisconnect, portForDisconnect); // Сигнал для удаления внутри сервера
}

// Обновляем таблицу, если клиент отключается
void MainWindow::UpdateTable(QString ip, QString port)
{
    // Ищем строку в таблице, где ip и port совпадают и удаляем её
    for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
       if (ui->tableWidget->item(i, 0)->text() == port &&
               ui->tableWidget->item(i, 1)->text() == ip) {
           delete ui->tableWidget->cellWidget(i, 0); // Удаляем itemPort
           delete ui->tableWidget->cellWidget(i, 1); // Удаляем itemIp
           delete ui->tableWidget->cellWidget(i, 2); // Удаляем itemMessage
           delete ui->tableWidget->cellWidget(i, 3); // Удаляем button
           ui->tableWidget->removeRow(i); // Удаляем строку из таблицы
           updatePropertyButton(i); // Обновляем  свойства кнопок кика
           break;
       }
    }
}

// Обновляем свойства кнопок для ссылок на строчки таблицы
void MainWindow::updatePropertyButton(int row)
{
    while (row < ui->tableWidget->rowCount()) {
        ui->tableWidget->cellWidget(row, 3)->setProperty("row", row);
        row++;
    }
}

// Количество подключенных клиентов
void MainWindow::CountClient(int count)
{
    ui->label_2->setText("Количество подключенных клиентов: " + QString::number(count));
}
