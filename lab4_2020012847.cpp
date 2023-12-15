#include <iostream>
#include <stack>

using namespace std;


void showResult(int pageMissNum, int pageFrontNum, stack<int> pageDesert) {
    cout << "缺页次数：" << pageMissNum << "次" << endl;
    cout << "一共调用：" << pageFrontNum << "次" << endl;
    cout << "缺页中断率：" << pageMissNum*1.0/pageFrontNum*100 << "%" << endl;
    cout << "淘汰页面：";

    stack<int> helpstack;   // 辅助栈，实现栈的逆置

    while (!pageDesert.empty()){
		helpstack.push(pageDesert.top());
        pageDesert.pop();
    }
    while (!helpstack.empty()){
        cout << helpstack.top() << " ";
        helpstack.pop();
    }
	cout << endl;
}
void FIFO(int pageFrameNum, int pageFrontNum, int* pages){

	stack<int> pageDesert; // 淘汰页面
    int pageFrame[pageFrameNum];    // 物理页框
    for(int i = 0 ; i < pageFrameNum ; i++){
        pageFrame[i] = 0;
    }

    int pageMissNum = 0;    // 缺页次数
    int count = 0;
    int helpNum = 0;    // 实现FIFO算法
    while (count < pageFrontNum)
    {
        cout << "第" << count+1 << "次：" << endl;
        
        bool isMiss = true;     // 判断本次是否缺页
        bool isEmpty = true;    // 判断物理页框是否有空位
        bool isExist = false;   // 判断物理页框中是否存在本次页面

        // 判断物理页框中是否已经存在本次页面
        for(int i = 0 ; i < pageFrameNum ; i++){
            if(pages[count] == pageFrame[i])
            {
                isExist = true;
                break;
            }
        }
        // 若本次页面在物理页框中已存在，则直接进入下次页面走向
        if(isExist){
            isMiss = false;
            goto label1;//直接打印结果
        }
        // 判断物理页框有无空位
        for(int i = 0 ; i < pageFrameNum ; i++){
            isEmpty = false;
            if(!pageFrame[i])
            {
                isEmpty = true;
                i = helpNum;// 将helpNum看作一个指针，始终指向可以装新页面的位置
                break;
            }
        }
        // 本次页面在物理页框中不存在，且有空位，按顺序放入
        if(!isExist && isEmpty){
            pageFrame[helpNum] = pages[count];
            helpNum = (helpNum + 1) % pageFrameNum;
        }

        // 本次页面在物理页框中不存在，且物理页框中没有空位了
        // 实现 FIFO 算法
        if(!isExist && !isEmpty){
            pageDesert.push(pageFrame[helpNum]);
            pageFrame[helpNum] = pages[count];
            helpNum = (helpNum + 1) % pageFrameNum;
        }

        // 计算缺页次数
        if(isMiss == true){
            pageMissNum++;
        }
        label1:
        cout << "目前物理页框中页面编号为：";
        for (int i = 0 ; i < pageFrameNum ; i++){
            cout << pageFrame[i] << " ";
        }
        cout << endl;
        count++;
    }
    cout << endl;
    showResult(pageMissNum, pageFrontNum, pageDesert);
    return;
}

// set for LRU
struct PageFrame{
	int pageId;
	int useTime;
};
void LRU(int pageFrameNum, int pageFrontNum, int* pages) {
	stack<int> pageDesert; // 淘汰页面

	PageFrame pageFrame[pageFrameNum];	// 物理页框
    for(int i = 0 ; i < pageFrameNum ; i++){
        pageFrame[i].pageId = 0;
		pageFrame[i].useTime = pageFrontNum + 1;
    }

    int pageMissNum = 0;    // 缺页次数
    int count = 0;
    while (count < pageFrontNum)
    {
        cout << "第" << count+1 << "次：" << endl;
        
        bool isMiss = true;     // 判断本次是否缺页
        bool isEmpty = true;    // 判断物理页框是否有空位
        bool isExist = false;   // 判断物理页框中是否存在本次页面

        // 判断物理页框中是否已经存在本次页面
        for(int i = 0 ; i < pageFrameNum ; i++){
            if(pages[count] == pageFrame[i].pageId)
            {
                isExist = true;
                pageFrame[i].useTime = count;//记录它上次被访问的时间戳
                break;
            }
        }
        // 若本次页面在物理页框中已存在，则直接进入下次页面走向
        if(isExist){
            isMiss = false;
            goto label2;//直接打印结果
        }
        // 判断物理页框有无空位
        for(int i = 0 ; i < pageFrameNum ; i++){
            isEmpty = false;
            if(!(pageFrame[i].pageId))
            {
                isEmpty = true;
                break;
            }
        }
        // 本次页面在物理页框中不存在，且有空位，按顺序放入
        if(!isExist && isEmpty){
            for(int i = 0 ; i < pageFrameNum ; i++){
                if(!(pageFrame[i].pageId))
                {
                    pageFrame[i].pageId = pages[count];
                    pageFrame[i].useTime = count;//记录它上次被访问的时间戳
                    break;
                }
            }
        }
        // 本次页面在物理页框中不存在，且物理页框中没有空位了
        // 实现 LRU 算法
        if(!isExist && !isEmpty){
            int which = 0;
            for (int i = 0; i < pageFrameNum; i++)//找到最小的时间戳
            {
                if(pageFrame[i].useTime < pageFrame[which].useTime)
                {
                    which = i;
                }
            }
            pageDesert.push((pageFrame[which].pageId));
            pageFrame[which].pageId = pages[count];
            pageFrame[which].useTime = count;
        }
        // 计算缺页次数
        if(isMiss == true){
            pageMissNum++;
        }
        label2:
        cout << "目前物理页框中页面编号为：";
        for (int i = 0 ; i < pageFrameNum ; i++){
            cout << pageFrame[i].pageId << " ";
        }
        cout << endl;
        count++;
    }
    cout << endl;
    showResult(pageMissNum, pageFrontNum, pageDesert);
    return;
}

void OPT(int pageFrameNum, int pageFrontNum, int* pages) {
	stack<int> pageDesert; // 淘汰页面
    int pageFrame[pageFrameNum];    // 物理页框
    for(int i = 0 ; i < pageFrameNum ; i++){
        pageFrame[i] = 0;
    }

    int pageMissNum = 0;    // 缺页次数
    int count = 0;
    while (count < pageFrontNum)
    {
        cout << "第" << count+1 << "次：" << endl;
        
        bool isMiss = true;     // 判断本次是否缺页
        bool isEmpty = true;    // 判断物理页框是否有空位
        bool isExist = false;   // 判断物理页框中是否存在本次页面

        // 判断物理页框中是否已经存在本次页面
        for(int i = 0 ; i < pageFrameNum ; i++){
            if(pages[count] == pageFrame[i])
            {
                isExist = true;
                break;
            }
        }
        // 若本次页面在物理页框中已存在，则直接进入下次页面
        if(isExist){
            isMiss = false;
            goto label3;//直接打印结果
        }
        // 判断物理页框有无空位
        for(int i = 0 ; i < pageFrameNum ; i++){
            isEmpty = false;
            if(!pageFrame[i])
            {
                isEmpty = true;
                break;
            }
        }
        // 本次页面在物理页框中不存在，且有空位，按顺序放入
        if(!isExist && isEmpty){
            for(int i = 0 ; i < pageFrameNum ; i++){
                if(!pageFrame[i])
                {
                    pageFrame[i] = pages[count];
                    break;
                }
            }
        }
        // 本次页面在物理页框中不存在，且物理页框中没有空位了
        // 实现 OPT 算法
        if(!isExist && !isEmpty){
            int nextUse[pageFrameNum];
            for (int i = 0; i < pageFrameNum; i++)
            {
                nextUse[i] = pageFrontNum;
            }
            //记录所有页面的下一次访问的时间戳
            for (int i = 0; i < pageFrameNum; i++)
            {
                for (int j = count + 1; j < pageFrontNum; j++)
                {
                    if(pageFrame[i] == pages[j])
                    {
                        nextUse[i] = j;
                    }
                }
            }
            int which = 0;
            for (int i = 0; i < pageFrameNum; i++)
            {
                if(nextUse[i] > nextUse[which])//如果更晚被访问
                {
                    which = i;
                    continue;
                }
                if(nextUse[i] == nextUse[which] && pageFrame[i] < pageFrame[which])//如果同时被访问，但逻辑页面号更小
                {
                    which = i;
                    continue;
                }
            }
            pageDesert.push(pageFrame[which]);
            pageFrame[which] = pages[count];
        }

        // 计算缺页次数
        if(isMiss == true){
            pageMissNum++;
        }
        label3:
        cout << "目前物理页框中页面编号为：";
        for (int i = 0 ; i < pageFrameNum ; i++){
            cout << pageFrame[i] << " ";
        }
        cout << endl;
        count++;
    }
    cout << endl;

    showResult(pageMissNum, pageFrontNum, pageDesert);
    return;
}

int main(){
	int pageFrameNum;    // 物理页框数
    
    int pageFrontNum;    // 页面走向个数


    cout << "请输入分配给该作业的物理页框数：";
    cin >> pageFrameNum;

    cout << "请输入要访问的逻辑页面个数：";
    cin >> pageFrontNum;

    int pages[pageFrontNum]; // 页面走向
    // c++中数组必须赋初值
    for(int i = 0 ; i < pageFrontNum ; i++){
        pages[i] = 0;
    }

	cout << "请输入运行过程中对逻辑页面的访问顺序：";
    for(int i = 0 ; i < pageFrontNum ; i++){
        cin >> pages[i];    // 获取页面走向数组
    }

	cout << "FIFO begins: " << endl;
	FIFO(pageFrameNum, pageFrontNum, pages);
	cout << "FIFO ends: " << endl << endl;

	cout << "LRU begins: " << endl;
	LRU(pageFrameNum, pageFrontNum, pages);
	cout << "LRU ends: " << endl << endl;
	
	cout << "OPT begins: " << endl;
	OPT(pageFrameNum, pageFrontNum, pages);
	cout << "OPT ends: " << endl << endl;
	return 0;
}