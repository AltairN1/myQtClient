#pragma once
#include <QtWidgets>
#include <QTcpSocket>
#include <QFile>
#include <QFileDialog>

class myClient : public QWidget {
    Q_OBJECT
    QTcpSocket* m_pTcpSocket;
    QTextEdit* m_ptxtInfo;
    QLineEdit* m_ptxtInput;
    quint16 m_nNextBlockSize;
    QString m_strFileName;
public:
    myClient(const QString& strHost, int nPort, QWidget* pwgt = 0);
private slots:
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError);
    void slotSendToServer();
    void slotConnected();
    void slotLoad();
signals:
    void changeWindowTitle(const QString&);
};
