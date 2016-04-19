#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHostAddress>

namespace Ui {
class MainWindow;
}

class QSerialPort;
class QUdpSocket;
class QHostAddress;
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void on_leftConnectButton_toggled(bool checked);
    void on_rightConnectButton_toggled(bool checked);
private slots:
    void onLeftPortReadyRead();
    void onRightPortReadyRead();
private:
    void sendDataOnRightPort(const QByteArray &data);
    void sendDataOnLeftPort(const QByteArray &data);
private:
    Ui::MainWindow *ui;
    QSerialPort *mLeftPort;
    QUdpSocket *mRightPort;
    QByteArray mLeftBuffer;
    QByteArray mRightBuffer;
private:
    int mLeftTxCounter;
    int mLeftRxCounter;
    int mRightTxCounter;
    int mRightRxCounter;
private:
    QHostAddress mRightRemoteHost;
    int mRightRemotePort;
};

#endif // MAINWINDOW_H
