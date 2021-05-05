#include "myClient.h"

myClient::myClient(const QString& strHost, int nPort, QWidget* pwgt) :
                   QWidget(pwgt), m_nNextBlockSize(0) {
    m_pTcpSocket = new QTcpSocket(this);
    m_pTcpSocket->connectToHost(strHost, nPort);
    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(slotError(QAbstractSocket::SocketError)));
    m_ptxtInfo = new QTextEdit;
    m_ptxtInput = new QLineEdit;
    m_ptxtInfo->setReadOnly(true);
    QPushButton* pcmd = new QPushButton("&Send");
    connect(pcmd, SIGNAL(clicked()), SLOT(slotSendToServer()));
    connect(m_ptxtInput, SIGNAL(returnPressed()),
            this, SLOT(slotSendToServer()));

    QMenu* pmnuFile = new QMenu("&File");
    pmnuFile->addAction("&Load", this, SLOT(slotLoad()),QKeySequence("CTRL+S"));
    pmnuFile->addAction("&Quit", qApp, SLOT(quit()),QKeySequence("CTRL+Q"));
    QMenuBar* mnuBar = new QMenuBar;
    mnuBar->addMenu(pmnuFile);

    QVBoxLayout* pvbxLayout = new QVBoxLayout;
    pvbxLayout->addWidget(new QLabel("<H1>Client</H1>"));
    pvbxLayout->setMenuBar(mnuBar);
    pvbxLayout->addWidget(m_ptxtInfo);
    pvbxLayout->addWidget(m_ptxtInput);
    pvbxLayout->addWidget(pcmd);
    setLayout(pvbxLayout);
}

void myClient::slotConnected(){
    m_ptxtInfo->append("Received the connected() signal");
}

void myClient::slotReadyRead(){
    QDataStream in(m_pTcpSocket);
    in.setVersion(QDataStream::Qt_6_1);
    for(;;) {
        if(!m_nNextBlockSize){
            if (m_pTcpSocket->bytesAvailable() < sizeof(quint16)){
                break;
            }
            in>>m_nNextBlockSize;
        }
        if (m_pTcpSocket->bytesAvailable() < m_nNextBlockSize){
            break;
        }
        QTime time;
        QString str;
        in >> time >> str;
        //m_ptxtInfo->append(time.toString() + " " + str);
        m_ptxtInfo->append(str);
        m_nNextBlockSize = 0;
    }
}

void myClient::slotError(QAbstractSocket::SocketError err) {
    QString strError = "Error: " + (err == QAbstractSocket::HostNotFoundError ?
                                    "The host was not found." :
                                    err == QAbstractSocket::RemoteHostClosedError ?
                                    "The remote host in closed." :
                                    err == QAbstractSocket::ConnectionRefusedError ?
                                    "The connection was refused." :
                                    QString(m_pTcpSocket->errorString()));
    m_ptxtInfo->append(strError);
}

void myClient::slotSendToServer(){
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_1);
    out << quint16(0) << QTime::currentTime() << m_ptxtInput->text();
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    m_pTcpSocket->write(arrBlock);
    m_ptxtInput->setText("");
}

void myClient::slotLoad(){
    QString str = QFileDialog::getOpenFileName();
    if(str.isEmpty()){
        return;
    }
    QFile file(str);
    if(file.open(QIODevice::ReadOnly)){
        QTextStream stream(&file);
        m_ptxtInput->setText(stream.readAll());
        file.close();
        m_strFileName = str;
        emit changeWindowTitle(m_strFileName);
    }
}
