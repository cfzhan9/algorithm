#include <iostream>
#include <windows.h>
#include <string> //C++风格字符串
#include <queue>

using namespace std;

int cap = 0;//电容
int capacitor = 0;//电容器
using namespace std;
bool sign;//复位标记 true复位





void ReSet()//复位模式
{
    int reset;
    cout << "请输入复位指令_0：" << endl;
    cin >> reset;
    if (reset == 0)
    {
        cout << "复位成功！" << endl;
        sign = true;
    }
}

void Daisy()//菊花链模式
{
    char f;
    while (true)//输入检查
    {
        cin >> f;
        if (f != 'y' && f != 'n') cout << "请输入正确符号（y/n）" << endl;
        if (f == 'y' || f == 'n') break;
    }
    if (f == 'y')
    {
        cout << "DC寻址..............";
        Sleep(1000);
        cout << "success!" << endl;
    }
}

void Star(int n)//程序模式启动
{
    sign = false;
    if (n == 111)
    {
        cout << "程序模式激活成功！" << endl;
        cout << "收到程序激活命令111" << endl;
    }
    else
    {
        cout << "warning：程序模式激活失败！传感器空闲中" << endl;
        while (!sign)
        {
            ReSet();
        }
    }
}


/*
为了检测VDDI电容的损耗，监测电容的放电行为:如果电容存在，放电将是缓慢的，而一个不存在的电容导致快速的电压下降。为此，VDDI稳压器输出被关闭，VDDI电容将放电。在没有电容的情况下，由于VDDI节点快速放电，传感器将启动硬复位。在评估电容器是否仍然存在后，VDDI稳压器输出再次接通。对于检测到的C-loss，发送相应的错误码(参见3.4.4.2和3.4.5)。该c损耗检测在Ini phase I结束时进行，时间约为15μs。在这有限的时间内，传感器的电流消耗下降到大约0毫安。
*/

/////////////////////////////////////////////////////////////////////////////////////////
//初始C-loss检测（检测VDDI电容的损耗）
void initialCLoss() {
    cout << "稳压器关闭" << endl;
    Sleep(1000);
    while (1) {
        //不存在电容
        if (cap == 0) {
            //VDDI稳压器输出被关闭，传感器将启动硬复位
            cout << "VDDI稳压器输出被关闭，传感器将启动硬复位" << endl;
            Sleep(1000);

            //评估电容器是否仍然存在？
            if (capacitor == 1) {
                //VDDI稳压器输出再次接通
                cout << "VDDI稳压器输出再次接通" << endl;
                Sleep(1000);
            }
            else {
                //发送相应的错误码
                cout << "错误码: C-loss detection" << endl;
                Sleep(1000);
            }
            break;
        }
    }
}


/*
   Fast Offset Cancellation
   1、上电后，在初始化阶段1执行快速偏移量取消。快速，偏移抵消算法是通过计算512个样本的均
   值，然后从加速度信号中减去512个样本的均值。

   2、由于潜在的不稳定偏移值，复位后的前20个样本被忽略。然后计算后续512个样本的均值。这个平均值
   在初始化阶段1中计算一次，该阶段持续（512+20）采样周期。初始化阶段1之后，总是从低通滤波信号中
   减去平均值。

   3、在快速偏移抵消发生后，对信号进行检查，以确保没有剩余偏移。对于此检查，将偏移量减去的值与
   6LSB的极限进行比较（对应于6g，1LSB/g灵敏度）。

   4、如果信号超出此限制，将在初始化阶段3发送一个“传感器缺陷”消息，并伴有相应的“快速偏移抵消错误”
   错误代码（请参阅3.4.4.2节：Fast offset cancellation error X/Y/Z-channel）。如果超过了最大
   校正值，如果没有超过阈值，SMA68x/69x将发送
   错误码“慢偏移抵消错误"（3.4.4.2节：Slow offset cancellation error X/Y/Z-channel）。

   */

   /////////////////////////////////////////////////////////////////////////////////////////
//快速偏移消除
int fastOffsetCancellation(int* fastOffsetArr, int ignoreNum, int fastOffsetNum) {
    //快速偏移样本个数
    int fastTimes = sizeof(fastOffsetArr) / sizeof(fastOffsetArr[0]);
    int sum = 0;
    int meanValue = 0;//样本均值

    for (int i = ignoreNum; i < fastOffsetNum; i++)
    {
        sum += fastOffsetArr[i];
    }
    meanValue = sum / (fastOffsetNum - ignoreNum);
    return meanValue;
}

//int fastOffsetCancellation(){
//    /*
//    此处写循环采样代码
//    */
//    queue<int>q1;
//    int n = 532;
//    //long int sum = 0;
//    int sum = 0;
//
//    //模拟储存532个样本,并计算最后512个样本平均值
//    for (int i = 0; i < n; i++)
//    {
//        sum += i;
//        q1.push(i);
//        if (q1.size() > 512)
//        {
//            sum -= q1.front();
//            q1.pop();
//        }
//    }
//    int Sample_average = sum / double(n);//样本平均值
//
//    //double Low_pass = 0000;//低通滤波信号自定义
//
//    return Sample_average;
//}



/*
初始阶段1（无数据传输，快速偏移抵消，只有在RUN命令之后才进入init阶段）
*/
void initialPhase1() {
    cout << "初始阶段1" << endl;

    initialCLoss();

    system("pause");

    //int fastOffset = fastOffsetCancellation();
    //cout << "快速偏移消除校正值: " << fastOffset << endl;
    int fastOffsetNum, ignoreNum;
    int fastOffsetArr[532];//快速偏移数组

    cout << "快速偏移消除算法计算总样本数: ";
    cin >> fastOffsetNum;

    cout << "忽略样本数: ";
    cin >> ignoreNum;

    for (int i = 0; i < fastOffsetNum; i++)
    {
        cout << "请输入第" << i + 1 << "个数";
        cin >> fastOffsetArr[i];
    }

    int fastOffset = fastOffsetCancellation(fastOffsetArr, ignoreNum, fastOffsetNum);
    cout << "复位后的前" << ignoreNum << "个样本被忽略，计算" << fastOffsetNum << "个样本的均值，均值为：" << fastOffset << endl;
}

/*
初始阶段2（传感器被测试，传感器数据发送给ECU）
*/

//发送状态数据，n为发送次数
void setStatus(int n)
{
    int i = 0;
    while (i < n)
    {
        i++;
        cout << "发送状态数据" << i << "次！" << endl;
    }
    i = 0;
}

void initialPhase2() {
    cout << "初始阶段2" << endl;
    cout << "发送识别码！" << endl;

    setStatus(4);
    cout << "状态数据发送完成！开始发送下一状态数据...." << endl;
    setStatus(1);

}

/*
初始阶段3（传感器状态被传输如果传感器是缺陷的，一个特定的“传感器缺陷”信息发送，直到传感器下电）
*/
void initialPhase3() {
    cout << "初始阶段3" << endl;
}

//运行模式
void runningMode() {

}



int main()
{
    //int fastOffsetArr[532];//快速偏移数组


    ReSet();//复位模式
    Sleep(1000);

    cout << "是否为菊花链模式（y/n）？" << endl;
    Daisy();//菊花链模式

    int n;
    cout << "输入程序激活命令 111：" << endl;
    cin >> n;
    //int n = 111;
    Star(n);//程序模式启动

    initialPhase1();
    system("pause");


    initialPhase2();
    system("pause");

    initialPhase3();




    //system("pause");

    return 0;
}
