#include "widget.h"
#include "ui_widget.h"
#include "QMessageBox"
#include "AES.h"
#include "QByteArray"
#include "QString"
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->lineEdit->setPlaceholderText("请输入8个字符（密码）");    //密码框
    ui->decrypto->setPlaceholderText("请输入需要加密的文本");     //明文框
    ui->crypto->setPlaceholderText("请输入需要解密的文本");       //暗文框
}

Widget::~Widget()
{
    delete ui;
}

//加密按钮
void Widget::on_lock_clicked()
{
    QString cryptoWord,decryptoWord,passWord;                   //分别定义"明文"、"暗文"、"密码"
    int pass[4][4]={0x0};                                       //16个8bit的密钥

    //读取密码
    passWord=ui->lineEdit->text();                              //读取密码框中的数据
    if(passWord.size()<8){QMessageBox::information(NULL, "错误", "请您输入密钥过短\n请增加长度至8");return;}
    else if(passWord.size()>8){QMessageBox::information(NULL, "错误", "您输入的密钥过长\n请缩短长度至8");return;}
    for(int i=0;i<passWord.size();i++){                         //遍历QStrign类型的密码
        int m=i*2,n=i*2+1;                                      //QString[x]长16位，每个数组的单个元素存储8位
        pass[m/4][m%4]=(passWord[i].unicode()&0xff00)>>8;       //取前8位
        pass[n/4][n%4]=passWord[i].unicode()&0x00ff;            //取后8位
    }

    //读取明文
    decryptoWord=ui->decrypto->toPlainText();                   //读取明文框中的数据
    if(decryptoWord.size()==0){QMessageBox::information(NULL, "错误", "请输入内容");return;}
    int NeedSize=8-decryptoWord.size()%8;                       //填充方式：PKCS7Padding 。数据个数最后少几个就填充多少个数
    if(NeedSize==0)NeedSize=8;                                  //即使每个分块长度为8个16bit，也要在末尾加上16个"16"
    for(int i=0;i<NeedSize;i++){
        decryptoWord.append(QString::number(NeedSize));
    }

    for(int i=0;i<decryptoWord.size();i++){
        if((i+1)%8==0){
            int arr[4][4]={0};                                  //每个分块大小为4x4，分块的单个元素存储8bit

            //将QString[x]的16bit分割为2个8bit，并存入数组中，数组的每个元素存储8bit
            arr[0][0]=(decryptoWord[0].unicode()&0xff00)>>8,arr[0][1]=decryptoWord[0].unicode()&0x00ff, arr[0][2]=(decryptoWord[1].unicode()&0xff00)>>8,arr[0][3]=decryptoWord[1].unicode()&0x00ff;
            arr[1][0]=(decryptoWord[2].unicode()&0xff00)>>8,arr[1][1]=decryptoWord[2].unicode()&0x00ff, arr[1][2]=(decryptoWord[3].unicode()&0xff00)>>8,arr[1][3]=decryptoWord[3].unicode()&0x00ff;
            arr[2][0]=(decryptoWord[4].unicode()&0xff00)>>8,arr[2][1]=decryptoWord[4].unicode()&0x00ff, arr[2][2]=(decryptoWord[5].unicode()&0xff00)>>8,arr[2][3]=decryptoWord[5].unicode()&0x00ff;
            arr[3][0]=(decryptoWord[6].unicode()&0xff00)>>8,arr[3][1]=decryptoWord[6].unicode()&0x00ff, arr[3][2]=(decryptoWord[7].unicode()&0xff00)>>8,arr[3][3]=decryptoWord[7].unicode()&0x00ff;
            encrypt(arr,pass);                                  //对每个分块进行加密
            for(int j=0;j<4;j++)
                for(int k=0;k<4;k++)
                    if(k&1){
                        int num=(arr[j][k-1]<<8)+arr[j][k];     //将2个8bit合成1个16bit
                        QString uni=QString("%1").arg(num,4,16);//将16bit的unicode码转换为QString
                        cryptoWord.push_back(uni);              //累计加密、转换，并形成明文
                    }
        }
    }
    for(auto it=cryptoWord.begin();it!=cryptoWord.end();it++)   //遍历明文，如果为QString[x]为空格，则转换为'0'
        if(*it==' ')*it='0';
    ui->crypto->clear();                                        //清除明文框的元素
    ui->crypto->insertPlainText(cryptoWord);                    //将加密后的暗文显示在暗文框中
}


//解密按钮
void Widget::on_release_clicked(){  
    QString cryptoWord,decryptoWord,passWord;                   //分别定义"明文"、"暗文"、"密码"
    int pass[4][4]={0x0};                                       //16个8bit的密钥

    //读取密码
    passWord=ui->lineEdit->text();                              //读取密码
    if(passWord.size()<8){QMessageBox::information(NULL, "错误", "请您输入密钥过短\n请增加长度至8");return;}
    else if(passWord.size()>8){QMessageBox::information(NULL, "错误", "您输入的密钥过长\n请缩短长度至8");return;}
    for(int i=0;i<passWord.size();i++){                         //遍历QStrign类型的密码
        int m=i*2,n=i*2+1;                                      //QString[x]长16位，每个数组的单个元素存储8位
        pass[m/4][m%4]=(passWord[i].unicode()&0xff00)>>8;       //取前8位
        pass[n/4][n%4]=passWord[i].unicode()&0x00ff;            //取后8位
    }

    //读取密文
    cryptoWord=ui->crypto->toPlainText();                       //读取密文框中的数据
    if(cryptoWord.size()==0){QMessageBox::information(NULL, "错误", "请输入内容");return;}
    if((cryptoWord.size()&8)!=0){QMessageBox::information(NULL, "错误", "请输入正确密文");return;}   //密文的大小必须为16个8bit(即8个16bit)的整数倍

    for(int i=0;i<cryptoWord.size();i++){                       //遍历密文
        if((i+1)%32==0){                                        //读取一个分块，每个分块长32位，存储16个8bit
            int arr[4][4]={0};                                  //分块存入到arr中
            for(int j=0;j<16;j++){
                int m=j*2,n=j*2+1;                              //两个QString代表两个4bit，每两个4bit合成8bit存入到arr中的单个元素
                QString StrNum=QString(cryptoWord[m])+QString(cryptoWord[n]);
                arr[j/4][j%4]=StrNum.toInt(nullptr,16);         //QString转换为int
            }
            decrypt(arr,pass);                                  //解密
            for(int j=0;j<4;j++)
                for(int k=0;k<4;k++)
                    if(k&1){                                    //每两个arr[x]元素代表一个16bit的unicode
                        ushort num[0];
                        num[0]=(arr[j][k-1]<<8)+arr[j][k];
                        QString uni=QString::fromUtf16(num);    //16bit的数据转换为unicode类型的QString
                        decryptoWord.push_back(uni);            //QString添加到明文
                    }
        }
    }
    QString tmp=*decryptoWord.rbegin();                         //填充方式：PKCS7Padding
    int i=tmp.toInt();
    decryptoWord.remove(decryptoWord.size()-i,i);               //若最后的阿拉伯数字是x,则删除QString后面的x个元素
    ui->decrypto->clear();                                      //清楚明文框
    ui->decrypto->insertPlainText(decryptoWord);                //显示明文在框中
}


void Widget::on_more_clicked()
{
    QMessageBox::information(NULL, "详情讯息", "加密模式：ECB\n填充方式：填充方式：PKCS7Padding\n邮箱：huhaoyu555@gmail.com\n链接：");
}

