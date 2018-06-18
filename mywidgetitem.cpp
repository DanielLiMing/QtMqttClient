#include "mywidgetitem.h"

MyWidgetItem::MyWidgetItem(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    //label = new QLabel;
    btn_del = new QPushButton;
    text = new QLineEdit;
    btn_del->setText(tr("取消订阅"));
    text->setEnabled(false);
    layout->addWidget(text);
    layout->addWidget(btn_del);
    setLayout(layout);

}
