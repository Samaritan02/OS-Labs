#include<iostream>
#include<cstdio>
#include<cstring>
#include<string>
#include<vector>
#include<queue>
#include<cstdlib>
#include<iomanip>
#include<algorithm>
using namespace std;
#define N 20                            //进程数

///数据结构——通用的进程控制块PCB
struct PCB
{
    char process_name;                  //进程名
    double arrival_time;                //进程到达时间
    double service_time;                //进程要求服务时间
    double start_time;                  //进程开始执行时间
    double need_service_time;           //进程还需占用CPU时间
    double end_time;                    //进程执行完成时间
    double process_priority;            //进程优先级
    double turnaround_time;             //进程周转时间
    double weight_turnaround_time;      //进程带权周转时间
    string status;                      //进程状态
};

PCB pcb[N];                             //进程
double average_TT;                      //平均周转时间
double average_WTT;                     //平均带权周转时间
int n;                                  //进程数
int flag = 0;

///函数定义：
void Init_process();                     //进程初始化
bool cmp_name(PCB a, PCB b);             //按进程名排序，按序输出
bool cmp_process_priority(PCB a, PCB b); //按优先级大小排序

void HPF();                              //HPF优先级调度算法
void Result_Print(PCB Tmp_pcb[]);        //最终结果打印（各进程的完成时间，各进程的周转时间及带权周转时间）
bool All_finished(PCB pd_pcb[]);         //判断是不是所有进程都已经执行结束，作为循环的判断条件（状态输出的时间优化）

/************************************
主函数：main()
*************************************/
int main()
{
    Init_process();    //进程初始化
    HPF();
    return 0;
}

/************************************
函数名称：Init_process()
函数功能：进程初始化，输入进程数，以及每个进程的进程名 到达时间 服务时间
返回值：无
*************************************/
void Init_process()
{
    int i;
    cout<<endl<<"---------------高优先级调度算法--------------"<<endl;
    cout<<"请输入进程数：";
    cin>>n;
    cout<<"请依次输入 进程名 到达时间 服务时间 优先级(如：A 0 3 5)，注意优先级数值越大优先级越高"<<endl;
    for( i = 0; i < n; i++)
    {
        char p_name;             //进程名
        double p_arrive,p_service; //进程到达时间 服务时间
        double p_priority;
        cin>>p_name>>p_arrive>>p_service>>p_priority;
        pcb[i].process_name = p_name;
        pcb[i].arrival_time = p_arrive;
        pcb[i].service_time = p_service;
        pcb[i].process_priority = p_priority;
        if(!p_arrive) pcb[i].status = "Ready";
        else pcb[i].status = "WFA";
    }
}

/************************************
函数名称：cmp_name(PCB a, PCB b)
函数功能：按进程名排序，按字母序从小到大输出
返回值：无
*************************************/
bool cmp_name(PCB a, PCB b)
{
    return int(a.process_name) < int(b.process_name);
    //ASCII 码判断大小
}

/************************************
函数名称：Print(PCB Tmp_pcb[])
函数功能：格式化输出
返回值：无
*************************************/
void Result_Print(PCB Tmp_pcb[])
{
    PCB cp_pcb[N];
    int i;
    for( i=0; i < n; i++)
        cp_pcb[i] = Tmp_pcb[i];
    sort(cp_pcb,cp_pcb+n,cmp_name);
    /*for( i = 0; i < 100; i++) cout<<"*";*/
    cout<<endl;
    cout<<setw(3)<<" "<<"进程名"<<setw(14)<<"      到达时间"<<setw(13)<<"    服务时间"<<setw(12)<<"    完成时间"<<setw(12)<<"    周转时间"<<setw(20)<<"      带权周转时间"<<"\t  "<<endl;
   
    for( i = 0; i < n; i++)
    {
        cout<<setw(6)<<cp_pcb[i].process_name<<setw(14)<<cp_pcb[i].arrival_time<<setw(12)<<cp_pcb[i].service_time
        <<setw(12)<<cp_pcb[i].end_time<<setw(12)
        <<cp_pcb[i].turnaround_time<<setw(4)<<" "<<"\t\t"<<cp_pcb[i].weight_turnaround_time<<setw(8)<<"\t"<<" "<<" "<<" ";
     cout<<" "<<endl;
    }
   /* for( i = 0; i < 100; i++) cout<<"*";*/
    cout<<endl;
}

/************************************
函数名称：bool All_finished(PCB pd_pcb[]);
函数功能：判断是不是所有进程都已经
执行结束，作为循环的判断条件（状态
输出的时间优化）
返回值：true or false
*************************************/
bool All_finished(PCB pd_pcb[])
{
    bool Flagge = true;
    string estimate = "Finished";
    for (int i = 0; i < n; i++)
    {
        if(pd_pcb[i].status!=estimate)
        {
            Flagge = false;
            return Flagge;
        }
    }
    return Flagge;
    //默认全部完成，检索到未完成的进程即返回false
}

/************************************
函数名称：cmp_process_priority(PCB a, PCB b)
函数功能：按优先级大小排序
（注：优先级数值越大，优先级越高）
返回值：无
*************************************/
bool cmp_process_priority(PCB a, PCB b)
{
    return a.process_priority > b.process_priority;
    //根据优先级降序排序
}

/************************************
函数名称：HPF()
函数功能：HPF优先级调度算法
返回值：无
*************************************/
void HPF()
{
    PCB HPF_pcb[N];//优先级队列
    int time = 0;//时间点
    string estimate = "Ready";
    for (int i = 0; i < n; i++)
    {
        HPF_pcb[i] = pcb[i];
    }
    sort(HPF_pcb, HPF_pcb + n, cmp_process_priority);//按优先级排序
    while(!All_finished(HPF_pcb))//如果不是全部完成
    {
        for (int i = 0; i < n; i++)
        {
            if(HPF_pcb[i].status==estimate)//找到最靠前的Ready进程
            {
                time += HPF_pcb[i].service_time;//更新时间点
                for (int j = 0; j < n;j++)
                {
                    if((HPF_pcb[j].arrival_time<time||abs(HPF_pcb[j].arrival_time-time)<0.000001)&&HPF_pcb[j].status!="Finished")
                    {
                        HPF_pcb[j].status = "Ready";//更新进程状态
                    }
                }
                HPF_pcb[i].status = "Finished";//更新被调度的进程状态
                HPF_pcb[i].turnaround_time = time - HPF_pcb[i].arrival_time;
                HPF_pcb[i].end_time = time;
                int temptime = (HPF_pcb[i].turnaround_time / HPF_pcb[i].service_time+0.0005) * 1000;//四舍五入3位小数存储加权周转时间
                HPF_pcb[i].weight_turnaround_time = ((double)temptime)/1000;
                time--;
                break;
            }
        }
        time++;//若没有进程ready，则更新时间点
        for (int j = 0; j < n;j++)
        {
            if((HPF_pcb[j].arrival_time<time||abs(HPF_pcb[j].arrival_time-time)<0.000001)&&HPF_pcb[j].status!="Finished")
            {
                HPF_pcb[j].status = "Ready";
            }
        }
    }
    //以下的HPF_pcb为已经调度好的进程数组，需要在上面代码中完成处理
    Result_Print(HPF_pcb);
    average_TT = 0;
    average_WTT = 0;
    for(int i = 0; i < n; i++)
    {
        average_TT += HPF_pcb[i].turnaround_time;
        average_WTT += HPF_pcb[i].weight_turnaround_time;
    }
    cout<<"调用优先级算法的平均周转时间为:";
    cout<<average_TT/n<<endl;
    cout<<"调用优先级算法的平均带权周转时间为:";
    cout<<average_WTT/n<<endl<<endl;
}