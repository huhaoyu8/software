/*
作者:胡浩宇    邮箱：huhaoyu555@gmail.com
流程：
加密：密钥扩展->轮密钥加->字节代替->行移位->列混淆->轮密钥加->字节代替->行移位->轮密钥加
解密：密钥扩展->轮密钥加->逆字节代替->逆行移位->轮密钥加->逆列混淆->逆字节代替->逆行移位->轮密钥加
*/
#ifndef AES_H
#define AES_H
#define getLeft4Bit(num) ((num & 0xf0) >> 4)	//获取整形数据的高8位的左4个位
#define getRight4Bit(num) (num & 0x0f)			//获取整形数据的低8位的右4个位

//S盒
static const int S[16][16] = { 0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };
//逆S盒
static const int SS[16][16] = { 0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };
//Rcon
static const int Rcon[11] = { 0x00,0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36 };

//字节代换，传入4*4的二维数组
static void subBytes(int arr[4][4]){
    for(int i = 0; i < 4; i++)                  //对每一行进行字节代换
        for(int j = 0; j < 4; j++){             //对每一列进行字节代换
            int row = getLeft4Bit(arr[i][j]);   //获得左四位的值
            int col = getRight4Bit(arr[i][j]);  //获得右四位的值
            arr[i][j] = S[row][col];            //利用S盒字节代换
        }
}

//字节代换，传入4个int的一维数组
static void subBytes(int arr[4]){
    for(int i = 0; i < 4; i++){                 //对每一个字节进行字节代换
        int row = getLeft4Bit(arr[i]);          //获得左四位的值
        int col = getRight4Bit(arr[i]);         //获得右四位的值
        arr[i] = S[row][col];                   //利用S盒字节代换
    }
}

//字节逆代换，传入4*4的二维数组
static void desubBytes(int arr[4][4]){
    for(int i = 0; i < 4; i++)                  //对每一行进行字节代换
        for(int j = 0; j < 4; j++){             //对每一列进行字节代换
            int row = getLeft4Bit(arr[i][j]);   //获得左四位的值
            int col = getRight4Bit(arr[i][j]);  //获得右四位的值
            arr[i][j] = SS[row][col];           //利用SS盒字节逆代换
        }
}

//字节逆代换，传入4个int的一维数组
static void desubBytes(int arr[4]){
    for(int i = 0; i < 4; i++){                 //对每一个字节进行字节代换
        int row = getLeft4Bit(arr[i]);          //获得左四位的值
        int col = getRight4Bit(arr[i]);         //获得右四位的值
        arr[i] = SS[row][col];                  //利用SS盒字节逆代换
    }
}

//行移位，左移位
static void shiftRows(int arr[4][4]) {
    //r代表第二行第一个字节，s代表第三行第一个字节，ss代表第三行第二个字节，t代表第四行第四个字节
    int r=arr[1][0],s=arr[2][0],ss=arr[2][1],t=arr[3][3];
    //第一行不移位
    //第二行左移一位
    for(int i=0;i<3;i++)
        arr[1][i]=arr[1][i+1];
    arr[1][3]=r;
    //第三行左移两位
    for(int i=0;i<2;i++)
        arr[2][i]=arr[2][i+2];
    arr[2][2]=s,arr[2][3]=ss;
    //第四行左移三位
    for(int i=3;i>0;i--)
        arr[3][i]=arr[3][i-1];
    arr[3][0]=t;
}

//行逆移位，右移位
static void deshiftRows(int arr[4][4]){
    int r=arr[1][3],s=arr[2][2],ss=arr[2][3],t=arr[3][0];
    //第一行不移位
    //第二行右移一位
    for(int i=3;i>0;i--)
        arr[1][i]=arr[1][i-1];
    arr[1][0]=r;
    //第三行右移两位
    for(int i=3;i>1;i--)
        arr[2][i]=arr[2][i-2];
    arr[2][0]=s,arr[2][1]=ss;
    //第四行右移三位
    for(int i=0;i<3;i++)
        arr[3][i]=arr[3][i+1];
    arr[3][3]=t;
}

//左移位
static void leftRotate(int arr[4]){
    //第一个字节移到最后，其他字节左移一位
    int tmp=arr[0];
    for(int i=0;i<3;i++)
        arr[i]=arr[i+1];
    arr[3]=tmp;
}

//逻辑乘法，GF(2^8)域上的乘法
static int multi(int num,int a){
    if(a==1)                                            //如果a为1，直接返回num
        return num;
    if(a==2){                                           //如果a为2，判断最高位是否为1
        if(num>=0x80)                                   //如果最高位为1
            return ((num&0x7f)<<1)^0x1b;                //0x1b为0x11b的低8位
        else                                            //如果最高位为0
            return num<<1;                              //左移一位，高位补0，也就是乘2
    }
    if(a&1)                                             //如果a为奇数
        return multi(num,a-1)^num;                      //递归调用，a拆分为2和1的组合
    else if (!(a&1))                                    //如果a为偶数
        return multi(multi(num,2),a/2);                 //递归调用，a拆分为a/2和2的组合
}

//列混合
static void mixColum(int arr[4][4]){
    int tmpArr[4][4]={0};                               //临时数组，存放混合后的结果
    int mul[4][4]={                                     //列混合矩阵
        {2,3,1,1},
        {1,2,3,1},
        {1,1,2,3},
        {3,1,1,2}};

    for(int i=0;i<4;i++){                               //遍历列混合矩阵
        for(int j=0;j<4;j++){
            int tmp[4]={0};                             //存放每一行的结果
            for(int k=0;k<4;k++){
                tmp[k]=multi(arr[k][j],mul[i][k]);      //每一行的结果
            }
            tmpArr[i][j]=tmp[0]^tmp[1]^tmp[2]^tmp[3];   //异或处理
        }
    }
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            arr[i][j]=tmpArr[i][j];                     //将结果赋值给原数组
}

//列逆混合
static void demixColum(int arr[4][4]){
    int tmpArr[4][4]={0};                               //临时数组，存放混合后的结果
    int mul[4][4]={                                     //列逆混合矩阵
        {0x0E,0x0B,0X0D,0x09},
        {0x09,0x0E,0x0B,0x0D},
        {0x0D,0x09,0x0E,0x0B},
        {0x0B,0x0D,0x09,0x0E}};

    for(int i=0;i<4;i++){                               //遍历列逆混合矩阵
        for(int j=0;j<4;j++){
            int tmp[4]={0};                             //存放每一行的结果
            for(int k=0;k<4;k++){
                tmp[k]=multi(arr[k][j],mul[i][k]);      //每一行的结果
            }
            tmpArr[i][j]=tmp[0]^tmp[1]^tmp[2]^tmp[3];   //异或处理
        }
    }
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            arr[i][j]=tmpArr[i][j];                     //将结果赋值给原数组
}

//密钥扩展
static void keyExpansion(int key[4][4],int roundKey[44][4]){
    int tmp[4]={0};
    for(int i=0;i<4;i++)
        for(int j=0;j<4;j++)
            roundKey[i][j]=key[i][j];                   //前四轮密钥直接赋值

    for(int i=4;i<44;i++){
        for(int j=0;j<4;j++)                            //如果不是4的倍数，直接赋值
            tmp[j]=roundKey[i-1][j];                    //tmp为上一轮的最后一列

        if(i%4==0){                                     //如果是4的倍数，进行移位、代换、轮常数异或
            leftRotate(tmp);                            //左移一位
            subBytes(tmp);                              //字节代换
            tmp[0]^=Rcon[i/4];                          //轮常数异或
        }

        for(int j=0;j<4;j++)
            roundKey[i][j]=roundKey[i-4][j]^tmp[j];     //轮密钥加
    }
}

//加密
static void encrypt(int arr[4][4],int key[4][4]){
    int roundKey[44][4]={0};
    keyExpansion(key,roundKey);

    //第零轮,轮密钥加
    for(int j=0;j<4;j++)
        for(int k=0;k<4;k++)
            arr[j][k]^=roundKey[j][k];

    //第一轮到第九轮,字节代换,行移位,列混合,轮密钥加
    for(int i=1;i<10;i++){
        subBytes(arr);                          //字节代换
        shiftRows(arr);                         //行移位
        mixColum(arr);                          //列混合
        for(int j=0;j<4;j++)
            for(int k=0;k<4;k++)
                arr[j][k]^=roundKey[i*4+j][k];  //轮密钥加

    }

    //第十轮,字节代换,行移位,轮密钥加
    subBytes(arr);                              //字节代换
    shiftRows(arr);                             //行移位
    for(int j=0;j<4;j++)
        for(int k=0;k<4;k++)
            arr[j][k]^=roundKey[40+j][k];       //轮密钥加

}

//解密
static void decrypt(int arr[4][4],int key[4][4]){
    int roundKey[44][4]={0};
    keyExpansion(key,roundKey);

    //第零轮,轮密钥加
    for(int j=0;j<4;j++)
        for(int k=0;k<4;k++)
            arr[j][k]^=roundKey[40+j][k];

    //第一轮到第九轮,逆行移位,逆字节代换,轮密钥加,逆列混合
    for(int i=1;i<10;i++){
        deshiftRows(arr);                           //逆行移位
        desubBytes(arr);                            //逆字节代换
        for(int j=0;j<4;j++)
            for(int k=0;k<4;k++)
                arr[j][k]^=roundKey[40-i*4+j][k];   //轮密钥加
        demixColum(arr);                            //逆列混合
    }

    //第十轮,逆行移位,逆字节代换,轮密钥加
    deshiftRows(arr);                               //逆行移位
    desubBytes(arr);                                //逆字节代换
    for(int j=0;j<4;j++)
        for(int k=0;k<4;k++)
            arr[j][k]^=roundKey[j][k];              //轮密钥加
}

#endif // AES_H
