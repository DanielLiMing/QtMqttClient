#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mywidgetitem.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //setMouseTracking(true);
    ui->lw_topics->setMouseTracking(true);
    ui->lw_topics->setSortingEnabled(true);
    m_client = new QMqttClient(this);
    m_client->setUsername(ui->le_username->text());
    m_client->setPassword(ui->le_passwd->text());
    connect(this->ui->pb_connect,SIGNAL(clicked()),this,SLOT(on_btn_connect_clicked()));
    connect(this->ui->pb_clear_sendbuf,SIGNAL(clicked()),this,SLOT(on_btn_clr_send_buf_click()));
    connect(this->ui->pb_clear_recvbuf,SIGNAL(clicked()),this,SLOT(on_btn_clr_recv_buf_click()));
    connect(this->ui->pb_exp_to_file,SIGNAL(clicked()),this,SLOT(on_btn_exp2file_click()));
    connect(this->ui->pb_show_time,SIGNAL(clicked()),this,SLOT(on_btn_showtime_click()));
    connect(this->ui->pb_show_topic,SIGNAL(clicked()),this,SLOT(on_btn_showtopic_click()));
    connect(this->ui->pb_unsub_all,SIGNAL(clicked()),this,SLOT(on_btn_unsub_all_click()));
    connect(this->ui->pb_subcribe,SIGNAL(clicked()),this,SLOT(on_btn_subtopic_click()));
    connect(this->ui->pb_unsubscribe,SIGNAL(clicked()),this,SLOT(on_btn_unsubtopic_click()));
    connect(this->ui->pb_publish,SIGNAL(clicked()),this,SLOT(on_btn_publish_click()));
    connect(this->ui->pb_ping,SIGNAL(clicked()),this,SLOT(on_btn_ping_click()));

    connect(m_client, SIGNAL(stateChanged(QMqttClient::ClientState)),this,SLOT(updateLogStateChange(QMqttClient::ClientState)));
    connect(m_client, SIGNAL(disconnected()),this,SLOT(brokenDisconnect()));
    connect(m_client, SIGNAL(messageReceived(QByteArray,QMqttTopicName)),this,SLOT(topicMessageReceived(QByteArray,QMqttTopicName)));
    connect(m_client, SIGNAL(pingResponseReceived()),this,SLOT(pingResponseReceived()));


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_connect_clicked(void)
{
    if(this->ui->le_server_port->text().isEmpty())
    {
        QMessageBox::critical(this,tr("Error"),tr("请检查参数！"));
        return;
    }
    hostPort = this->ui->le_server_port->text().toInt();
    if(this->ui->le_server_ip->text().isEmpty())
    {
        if(ui->le_server_addr->text().isEmpty())
        {
            QMessageBox::critical(this,tr("Error"),tr("请检查参数！"));
            return;
        }
        pressAddress(ui->le_server_addr->text());
        return;
    }else
    {
        hostName = ui->le_server_ip->text();
    }

    qDebug() << "on_btn_connect_clicked";
    if(m_client->state() == QMqttClient::Disconnected){
        this->ui->pb_connect->setText(tr("断开连接"));
        m_client->setHostname(hostName);
        m_client->setPort(hostPort);
        this->ui->le_server_addr->setEnabled(false);
        this->ui->le_server_ip->setEnabled(false);
        this->ui->le_server_port->setEnabled(false);
        this->ui->le_username->setEnabled(false);
        this->ui->le_passwd->setEnabled(false);
        m_client->connectToHost();
    }
    else{
        this->ui->pb_connect->setText(tr("连接"));
        this->ui->le_server_addr->setEnabled(true);
        this->ui->le_server_ip->setEnabled(true);
        this->ui->le_server_port->setEnabled(true);
        this->ui->le_username->setEnabled(true);
        this->ui->le_passwd->setEnabled(true);
        m_client->disconnectFromHost();
    }
}
void MainWindow::on_btn_clr_send_buf_click(void)
{
    qDebug() << "on_btn_clr_send_buf_click";
    ui->pet_publish_payload->clear();
}
void MainWindow::on_btn_clr_recv_buf_click(void)
{
    qDebug() << "on_btn_clr_recv_buf_click";
    this->ui->pet_recv->clear();
}
void MainWindow::on_btn_exp2file_click(void)
{
    qDebug() << "on_btn_exp2file_click";
    QString path = QFileDialog::getSaveFileName(this,tr("保存路径"),".",tr("Text Files(*.txt)"));
    if(!path.isEmpty())
    {
        QFile file(path);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::warning(this,tr("Error"),tr("Save Failed!"));
            return;
        }
        QTextStream out(&file);
        out << ui->pet_recv->toPlainText();
        file.close();
    }
    else
    {
        QMessageBox::information(this,tr("提示"),tr("未选择路径！"));
    }
}
void MainWindow::on_btn_showtime_click(void)
{
    qDebug() << "on_btn_showtime_click";
}
void MainWindow::on_btn_showtopic_click(void)
{
    qDebug() << "on_btn_showtopic_click";
}
void MainWindow::on_btn_unsub_all_click(void)
{
    qDebug() << "on_btn_unsub_all_click";
    while(ui->lw_topics->count())
    {
        QListWidgetItem *item_sel = ui->lw_topics->item(0);
        MyWidgetItem *cur_item =dynamic_cast<MyWidgetItem *>(ui->lw_topics->itemWidget(item_sel));
        m_client->unsubscribe(cur_item->text->text());
        ui->lw_topics->takeItem(0);
    }
    QMessageBox::information(this,tr("提示"),tr("取消订阅成功"));

}
void MainWindow::on_btn_subtopic_click(void)
{
    qDebug() << "on_btn_subtopic_click";
    QString topic = ui->le_sub_topic->text();
    for(int i=0;i<ui->lw_topics->count();i++)
    {
        QListWidgetItem *item_sel = ui->lw_topics->item(i);
        MyWidgetItem *cur_item =dynamic_cast<MyWidgetItem *>(ui->lw_topics->itemWidget(item_sel));
        if(cur_item->text->text() == topic )
        {
            QMessageBox::critical(this,tr("Error"),tr("主题：%1已经订阅").arg(topic));
            return;
        }
    }
    auto subscription = m_client->subscribe(topic);
    if(!subscription)
    {
        QMessageBox::critical(this,tr("Error"),tr("订阅失败！请查看是否连接？"));
        return;
    }

    MyWidgetItem *myitem = new MyWidgetItem;
    myitem->text->setText(ui->le_sub_topic->text());


    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(QSize(0,30));
    ui->lw_topics->addItem(item);
    ui->lw_topics->setItemWidget(item,myitem);

    connect(myitem->btn_del,&QPushButton::clicked,[this](){

        int item_n = ui->lw_topics->currentRow();
        if(item_n < 0) return;
        QListWidgetItem *item_sel = ui->lw_topics->item(item_n);
        MyWidgetItem *cur_item =dynamic_cast<MyWidgetItem *>(ui->lw_topics->itemWidget(item_sel));
        m_client->unsubscribe(cur_item->text->text());
        ui->lw_topics->takeItem(item_n);
    });
}
void MainWindow::on_btn_unsubtopic_click(void)
{
    qDebug() << "on_btn_unsubtopic_click";
    QString topic = ui->le_sub_topic->text();
    if(!topic.isEmpty())
    {
        m_client->unsubscribe(ui->le_sub_topic->text());
    }


}
void MainWindow::on_btn_publish_click(void)
{
    qDebug() << "on_btn_publish_click";

    QString topic = this->ui->le_topic->text();
    QString payload = this->ui->pet_publish_payload->toPlainText();
    int Qos = this->ui->sb_Qos->value();

    if(m_client->publish(topic,payload.toUtf8(),Qos) == -1)
    {
        qDebug() << "public error";
    }
    this->ui->pet_recv->appendPlainText(payload);
}

void MainWindow::on_btn_ping_click(void)
{
    ui->pb_ping->setEnabled(false);
    m_client->requestPing();
}


void MainWindow::updateLogStateChange(QMqttClient::ClientState  state)
{
    const QString content = QDateTime::currentDateTime().toString()
            + QLatin1String(": State Change")
            + QString::number(m_client->state())
            + QLatin1Char('\n');
    ui->pet_recv->appendPlainText(content);
}
void MainWindow::brokenDisconnect(void)
{
    this->ui->le_server_addr->setEnabled(true);
    this->ui->le_server_ip->setEnabled(true);
    this->ui->le_server_port->setEnabled(true);
    this->ui->le_username->setEnabled(true);
    this->ui->le_passwd->setEnabled(true);
    ui->pb_connect->setText(tr("连接"));
}
void MainWindow::topicMessageReceived(const QByteArray &message, const QMqttTopicName &topic)
{
    QString content;
    if(ui->pb_show_time->isChecked())
    {
        content = QDateTime::currentDateTime().toString();
    }
    if(ui->pb_show_topic->isChecked())
    {
        content += QLatin1String(" Received Topic: ") + topic.name();
    }

    content += QLatin1String(" Message: ") + message + QLatin1Char('\n');
    ui->pet_recv->appendPlainText(content);
}
void MainWindow::pingResponseReceived(void)
{
    ui->pb_ping->setEnabled(true);
    const QString content = QDateTime::currentDateTime().toString()
            +QLatin1String(" PingReponse")
            +QLatin1Char('\n');
    ui->pet_recv->appendPlainText(content);
}

void MainWindow::pressAddress(QString addr)
{
    QHostInfo::lookupHost(addr,this,SLOT(lookedup(QHostInfo)));

}
void MainWindow::lookedup(const QHostInfo &info)
{
    if (info.error() != QHostInfo::NoError)
    {
         qDebug() << "Lookup failed:" << info.errorString();
         return;
    }
    hostName=info.addresses()[0].toString();
    ui->le_server_ip->setText(hostName);
    this->ui->pb_connect->setText(tr("断开连接"));
    m_client->setHostname(hostName);
    m_client->setPort(hostPort);
    this->ui->le_server_addr->setEnabled(false);
    this->ui->le_server_ip->setEnabled(false);
    this->ui->le_server_port->setEnabled(false);
    this->ui->le_username->setEnabled(false);
    this->ui->le_passwd->setEnabled(false);
    m_client->connectToHost();
//    for (int i = 0;i < info.addresses().size();i++)
//    {
//         qDebug() << "Found address:" << info.addresses()[i].toString() << endl;
//    }
}


