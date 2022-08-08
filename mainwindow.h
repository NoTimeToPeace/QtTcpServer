#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QDesktopWidget>
#include <QVector>
#include "server.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT // Можем использовать сигналы и слоты

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void updatePropertyButton(int); // Обновление строчек, на которые ссылаются кнопки

public slots:
    void IpAdressPortClient(QString, QString); // Добавление в таблицу при подключении
    void MessageFromClient(QString, QString, QString); // Добавление сообщения от клиента в таблицу
    void CountClient(int); // Обновление количества клиентов на сервере
    void DeleteFromTableAndDisconnect(); // Удаление клиента из таблицы и кик с сервера
    void UpdateTable(QString, QString); // Обновление таблицы

private slots:
    void StartServer(); // Запуск сервера

private:
    Ui::MainWindow *ui; // Доступ к графическому интерфейсу
    Server* server; // Сервер

signals:
    void deleteClient(QString, QString); // Для удаления из контейнера
};
#endif // MAINWINDOW_H
