#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSerialPort>
#include <QUdpSocket>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    mLeftPort = new QSerialPort(this);
    connect(mLeftPort,SIGNAL(readyRead()),this,SLOT(onLeftPortReadyRead()));
    mRightPort = new QUdpSocket(this);
    connect(mRightPort,SIGNAL(readyRead()),this,SLOT(onRightPortReadyRead()));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_leftConnectButton_toggled(bool checked) {
    if(checked) {
        if(!mLeftPort->isOpen()) {
            mLeftPort->setPortName(ui->leftPortName->text());
            if(mLeftPort->open(QIODevice::ReadWrite)) {
                mLeftPort->setBaudRate(ui->leftPortBaud->value());

                ui->leftConnectButton->setText("Disconnect");
                ui->leftPortName->setEnabled(false);
                ui->leftPortBaud->setEnabled(false);
                mLeftTxCounter = 0;
                ui->leftTXCounter->setText(QString("%1").arg(mLeftTxCounter));
                mLeftRxCounter = 0;
                ui->leftRXCounter->setText(QString("%1").arg(mLeftRxCounter));
            } else {
                QString error = QString("Can't open port named %1").arg(mLeftPort->portName());
                QMessageBox::critical(this,QString("Port open error"),error);
                ui->leftConnectButton->blockSignals(true);
                ui->leftConnectButton->setChecked(false);
                ui->leftConnectButton->blockSignals(false);
            }
        }
    } else {
        if(mLeftPort->isOpen()) {
            mLeftPort->close();
            ui->leftConnectButton->setText("Connect");
            ui->leftPortName->setEnabled(true);
            ui->leftPortBaud->setEnabled(true);
        }
    }
}

void MainWindow::on_rightConnectButton_toggled(bool checked) {
    if(checked) {
        if(mRightPort->state() != QAbstractSocket::BoundState) {
            mRightRemoteHost = QHostAddress(ui->rightRemoteIP->text());
            mRightRemotePort = ui->rightRemotePort->value();
            bool res = mRightPort->bind(QHostAddress::Any, mRightRemotePort);
            if(res) {
                ui->rightConnectButton->setText("Disconnect");
                ui->rightRemoteIP->setEnabled(false);
                ui->rightRemotePort->setEnabled(false);
                mRightTxCounter = 0;
                ui->rightTXCounter->setText(QString("%1").arg(mRightTxCounter));
                mRightRxCounter = 0;
                ui->rightRXCounter->setText(QString("%1").arg(mRightRxCounter));
            }
            else QMessageBox::critical(this,"Port binding error","Can't bind port to current IP");
        }
    } else {
        if(mRightPort->state() == QAbstractSocket::BoundState) {
            mRightPort->disconnectFromHost();
            ui->rightConnectButton->setText("Connect");
            ui->rightRemoteIP->setEnabled(true);
            ui->rightRemotePort->setEnabled(true);
        }
    }
}

void MainWindow::onLeftPortReadyRead() {
    QByteArray readed = mLeftPort->readAll();
    mLeftBuffer.append(readed);
    mLeftRxCounter += readed.size();
    ui->leftRXCounter->setText(QString("%1").arg(mLeftRxCounter));

    int pos;
    while((pos = mLeftBuffer.indexOf('\n',pos)) != -1) {
        sendDataOnRightPort(mLeftBuffer.mid(0,pos+1));
        mLeftBuffer = mLeftBuffer.mid(pos+1);
        pos = 0;
    }
}

void MainWindow::onRightPortReadyRead() {
    while(mRightPort->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(mRightPort->pendingDatagramSize());
        QHostAddress senderHost;
        quint16 senderPort;
        mRightPort->readDatagram(datagram.data(), datagram.size(), &senderHost, &senderPort);
        mRightRxCounter += datagram.size();
        ui->rightRXCounter->setText(QString("%1").arg(mRightRxCounter));
        sendDataOnLeftPort(datagram);
    }
}

void MainWindow::sendDataOnRightPort(const QByteArray &data) {
    if(mRightPort->state() == QAbstractSocket::BoundState) {
        mRightPort->writeDatagram(data, mRightRemoteHost, mRightRemotePort);
        mRightTxCounter += data.size();
        ui->rightTXCounter->setText(QString("%1").arg(mRightTxCounter));
    }
}

void MainWindow::sendDataOnLeftPort(const QByteArray &data) {
    if(mLeftPort->isOpen()) {
        mLeftPort->write(data);
        mLeftTxCounter += data.size();
        ui->leftTXCounter->setText(QString("%1").arg(mLeftTxCounter));
    }
}
