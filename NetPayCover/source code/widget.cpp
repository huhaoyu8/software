#include "widget.h"
#include "ui_widget.h"
#include "QFont"
#include "QMessageBox"
#include "QString"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->Lpercent->setFont(QFont("宋体",15));
    ui->SPension->setRange(0,30);
    ui->SPension->setSingleStep(1);
    ui->SPension->setValue(16);
    ui->SPension->setPageStep(0);
    ui->LCDPension->display(ui->SPension->value()*0.5);
    connect(ui->SPension,&QSlider::sliderMoved,[=](int value){ui->LCDPension->display(value*0.5);});

    ui->SHealth->setRange(0,10);
    ui->SHealth->setSingleStep(1);
    ui->SHealth->setValue(4);
    ui->SHealth->setPageStep(0);
    ui->LCDHealth->display(ui->SHealth->value()*0.5);
    connect(ui->SHealth,&QSlider::sliderMoved,[=](int value){ui->LCDHealth->display(value*0.5);});

    ui->SUnemployment->setRange(0,10);
    ui->SUnemployment->setSingleStep(1);
    ui->SUnemployment->setValue(1);
    ui->SUnemployment->setPageStep(0);
    ui->LCDUnemployment->display(ui->SUnemployment->value()*0.5);
    connect(ui->SUnemployment,&QSlider::sliderMoved,[=](int value){ui->LCDUnemployment->display(value*0.5);});

    ui->SInjury->setRange(0,10);
    ui->SInjury->setSingleStep(1);
    ui->SInjury->setValue(0);
    ui->SInjury->setPageStep(0);
    ui->LCDInjury->display(ui->SInjury->value()*0.5);
    connect(ui->SInjury,&QSlider::sliderMoved,[=](int value){ui->LCDInjury->display(value*0.5);});

    ui->SBirth->setRange(0,10);
    ui->SBirth->setSingleStep(1);
    ui->SBirth->setValue(0);
    ui->SBirth->setPageStep(0);
    ui->LCDBirth->display(ui->SBirth->value()*0.5);
    connect(ui->SBirth,&QSlider::sliderMoved,[=](int value){ui->LCDBirth->display(value*0.5);});

    ui->SHouse->setRange(0,30);
    ui->SHouse->setSingleStep(1);
    ui->SHouse->setValue(22);
    ui->SHouse->setPageStep(0);
    ui->LCDHouse->display(ui->SHouse->value()*0.5);
    connect(ui->SHouse,&QSlider::sliderMoved,[=](int value){ui->LCDHouse->display(value*0.5);});
}

Widget::~Widget()
{
    delete ui;
}

void Widget::beforeTax(int num,double Insurance,double& sala,double& Tax){
    double tax=0,sal=0;
    double SalTax[9][2]={{0,0},{5000,0},{8000,0.03},{17000,0.1},{30000,0.2},{40000,0.25},{60000,0.3},{85000,0.35},{INT_MAX,0.45}};
    for(int i=0;i<9;i++){
        if(num<=SalTax[i][0]){
            tax=(num-SalTax[i-1][0])/(1-SalTax[i][1])-(num-SalTax[i-1][0]); //多余部分缴纳的税
            sal=num+tax;                                                    //扣税前的工资
            tax+=sal/(1-Insurance)-sal;                                     //税和五险一金和社保
            sal/=(1-Insurance);                                             //税前工资
            break;
        }
    }
    sala=sal,Tax=tax;
}

void Widget::afterTax(int num,double Insurance,double& sala,double& Tax){
    double tax=0,sal=0;
    double SalTax[9][2]={{0,0},{5000,0},{8000,0.03},{17000,0.1},{30000,0.2},{40000,0.25},{60000,0.3},{85000,0.35},{INT_MAX,0.45}};
    tax=num*Insurance;                                                          //缴纳的五险一金和社保
    sal=num-tax;                                                                //扣除五险一金和社保后的工资
    for(int i=0;i<9;i++){
        if(sal<=SalTax[i][0]){
            tax+=(sal-SalTax[i-1][0])*SalTax[i][1];                             //累计缴纳的税
            double tmpTax=(sal-SalTax[i-1][0])*SalTax[i][1];                    //多余部分缴纳的税
            sal=sal-tmpTax;                                                     //税后工资
            break;
        }
    }
    sala=sal,Tax=tax;
}

void Widget::on_pushButton_clicked()
{
    QString str=ui->lineEdit->text();
    for(auto it=str.begin();it!=str.end();it++)
        if(*it<'0'||*it>'9'){QMessageBox::information(NULL,"错误","请您仅输入整数");return;}
    int num=str.toInt();

    double Insurance=(ui->SBirth->value()+ui->SHealth->value()
            +ui->SHouse->value()+ui->SInjury->value()
            +ui->SPension->value()+ui->SUnemployment->value())*0.5*0.01;
    double sal=0,tax=0;
    if(ui->before->isChecked()){
        double tmp1,tmp2,tmp3,tmp4;
        beforeTax(num,Insurance,tmp1,tmp2);
        afterTax(tmp1,Insurance,tmp3,tmp4);
        if(fabs(num-tmp3)<1){
            sal=tmp1;
            tax=tmp2;
        }
        else {QMessageBox::information(NULL,"错误","您输入的税前工资不存在（即税后无法到达），\n请重新检查");return;}
    }
    else if(ui->after->isChecked()){
        afterTax(num,Insurance,sal,tax);
    }
    ui->LCDSalary->display(QString::number(sal));
    ui->LCDTax->display(QString::number(tax));
}

void Widget::on_More_clicked()
{
    QMessageBox::information(NULL,"详情讯息","<a href='https://github.com/huhaoyu8/software'>github.com/huhaoyu8/software</a><br/>邮箱：huhaoyu555@gmail.com");
}

