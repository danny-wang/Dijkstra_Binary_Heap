// C / C++实现的 Dijkstra最短路径，图的邻接表表示,使用binary heap加快找最小节点的速度
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;
const int maxnum = 20000;
vector<int> prev[maxnum];  //记录前点的前一个节点

int CountLines(char *filename)
{
    ifstream ReadFile;
    int n=0;
    char line[512];
    string temp;
    ReadFile.open(filename,ios::in);//ios::in 表示以只读的方式读取文件
    if(ReadFile.fail())//文件打开失败:返回0
    {
        return 0;
    }
    else//文件存在
    {
        while(getline(ReadFile,temp))
        {
            n++;
        }
        return n;
    }

    ReadFile.close();
}


// 邻接表的节点
struct AdjListNode
{
    int dest;
    int weight;
    struct AdjListNode* next;
};

// 邻接表 结构体
struct AdjList
{
    struct AdjListNode *head;  // 指向头节点
};

// 图结构体，V为顶点个数。array为所有的邻接表
struct Graph
{
    int V;
    struct AdjList* array;
};

//创建邻接表的节点
struct AdjListNode* newAdjListNode(int dest, int weight)
{
    struct AdjListNode* newNode = (struct AdjListNode*) malloc(
                                      sizeof(struct AdjListNode));
    newNode->dest = dest;
    newNode->weight = weight;
    newNode->next = NULL;
    return newNode;
}

//创建一个图，包含V的顶点
struct Graph* createGraph(int V)
{
    struct Graph* graph = (struct Graph*) malloc(sizeof(struct Graph));
    graph->V = V;

    graph->array = (struct AdjList*) malloc(V * sizeof(struct AdjList));

    for (int i = 0; i < V; ++i)
        graph->array[i].head = NULL;

    return graph;
}

// 添加一个边(无向图)
void addEdge(struct Graph* graph, int src, int dest, int weight)
{

    struct AdjListNode* newNode = newAdjListNode(dest, weight);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;

    newNode = newAdjListNode(src, weight);
    newNode->next = graph->array[dest].head;
    graph->array[dest].head = newNode;
}

// 最小堆节点
struct MinHeapNode
{
    int v;  //下标
    int dist; //距离
};

// 最小堆
struct MinHeap
{
    int size;
    int capacity;
    int *pos;     // pos[i]表示顶点i所在的下标
    struct MinHeapNode **array;
};

// 创建一个最小堆节点
struct MinHeapNode* newMinHeapNode(int v, int dist)
{
    struct MinHeapNode* minHeapNode = (struct MinHeapNode*) malloc(
                                          sizeof(struct MinHeapNode));
    minHeapNode->v = v;
    minHeapNode->dist = dist;
    return minHeapNode;
}

// A utility function to create a Min Heap
struct MinHeap* createMinHeap(int capacity)
{
    struct MinHeap* minHeap = (struct MinHeap*) malloc(sizeof(struct MinHeap));
    minHeap->pos = (int *) malloc(capacity * sizeof(int));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (struct MinHeapNode**) malloc(
                         capacity * sizeof(struct MinHeapNode*));
    return minHeap;
}

// 交换两个最小堆的节点
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b)
{
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

//在位置 idx 调整堆
void minHeapify(struct MinHeap* minHeap, int idx)
{
    int smallest, left, right;
    smallest = idx;
    left = 2 * idx + 1;
    right = 2 * idx + 2;

    if (left < minHeap->size
            && minHeap->array[left]->dist < minHeap->array[smallest]->dist)
        smallest = left;

    if (right < minHeap->size
            && minHeap->array[right]->dist < minHeap->array[smallest]->dist)
        smallest = right;

    if (smallest != idx)
    {
        // 需要交换的节点
        MinHeapNode *smallestNode = minHeap->array[smallest];
        MinHeapNode *idxNode = minHeap->array[idx];

        //交换下标
        minHeap->pos[smallestNode->v] = idx;
        minHeap->pos[idxNode->v] = smallest;

        //交换节点
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);

        minHeapify(minHeap, smallest);
    }
}

// 推是否为空
int isEmpty(struct MinHeap* minHeap)
{
    return minHeap->size == 0;
}

// 弹出堆顶的节点(即最小的节点)
struct MinHeapNode* extractMin(struct MinHeap* minHeap)
{
    if (isEmpty(minHeap))
        return NULL;

    struct MinHeapNode* root = minHeap->array[0];

    struct MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;

    // 更新下标
    minHeap->pos[root->v] = minHeap->size - 1;
    minHeap->pos[lastNode->v] = 0;

    // 记得减少堆的大小
    --minHeap->size;
    minHeapify(minHeap, 0);

    return root;
}

// 当节点v的距离更新后(变小了)调整堆
void decreaseKey(struct MinHeap* minHeap, int v, int dist)
{
    //获取节点 v 在 堆中的下标
    int i = minHeap->pos[v];

    minHeap->array[i]->dist = dist;

    // 因为是变小了，自下向上调整堆即可。 O(Logn)
    while (i && minHeap->array[i]->dist < minHeap->array[(i - 1) / 2]->dist)
    {
        minHeap->pos[minHeap->array[i]->v] = (i - 1) / 2;
        minHeap->pos[minHeap->array[(i - 1) / 2]->v] = i;
        swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);

        i = (i - 1) / 2;
    }
}

// 判断节点v是否在堆中
bool isInMinHeap(struct MinHeap *minHeap, int v)
{
    if (minHeap->pos[v] < minHeap->size)
        return true;
    return false;
}

// 打印结果
void printArr(int dist[], int n)
{
    printf("Vertex   Distance from Source\n");
    for (int i = 0; i < n; ++i)
        printf("%d \t\t %d\n", i, dist[i]);
}

void dijkstra(struct Graph* graph, int src)
{
    int V = graph->V;
    int dist[V];

    struct MinHeap* minHeap = createMinHeap(V);

    // 初始化堆包含所有的顶点
    for (int v = 0; v < V; ++v)
    {
        dist[v] = INT_MAX;
        minHeap->array[v] = newMinHeapNode(v, dist[v]);
        minHeap->pos[v] = v;
    }

    // 把 源点 src 的距离设置为0，第一个取出的点即为源点
    minHeap->array[src] = newMinHeapNode(src, dist[src]);
    minHeap->pos[src] = src;
    dist[src] = 0;
    decreaseKey(minHeap, src, dist[src]);

    minHeap->size = V;

    // 这个循环中，minHeap包含的是所有未在SPT中的顶点
    while (!isEmpty(minHeap))
    {
        // 取得堆顶节点，即最小距离的顶点
        struct MinHeapNode* minHeapNode = extractMin(minHeap);
        int u = minHeapNode->v;

        // 只需要遍历和u相邻的顶点进行更新
        struct AdjListNode* pCrawl = graph->array[u].head;
        while (pCrawl != NULL)
        {
            int v = pCrawl->dest;

            // 松弛操作，更新距离
            if (isInMinHeap(minHeap, v) && dist[u] != INT_MAX
                    && pCrawl->weight + dist[u] <= dist[v])
            {
                if (pCrawl->weight + dist[u] < dist[v])
                {
                    prev[v].clear();
                    dist[v] = dist[u] + pCrawl->weight;
                    //距离更新了之后，要调整最小堆
                    decreaseKey(minHeap, v, dist[v]);
                }
                prev[v].push_back(u);
            }
            pCrawl = pCrawl->next;
        }
    }

    // 打印
    //printArr(dist, V);
    cout<<"Vertex   Distance from Source"<<endl;
    cout<< 19999<<"    "<<dist[19999]<<endl;
}

// 查找从源点v到终点u的路径，并输出
void searchPath(vector<int> *prev, int v, int u, int sta[], int len)
{

    if (u == v)
    {
        cout<<v;
        return ;
    }
    sta[len] = u;
    for (int i = 0 ; i < prev[u].size(); ++i )
    {
        if (i > 0)
        {
            for (int j = len - 1  ; j >= 0 ; --j)
            {
                cout << " -> " << sta[j];
            }
            cout<<endl;
        }
        searchPath(prev, v, prev[u][i], sta, len + 1);
        cout << " -> " << u;
    }
}


// 测试
int main()
{

    int V = 20000;
    struct Graph* graph = createGraph(V);
    ifstream file;
    int LINES;
    char filename[512];
    cout<<"请输入要打开的文件名:"<<endl;
    cin>>filename;
    file.open(filename,ios::in);
    if(file.fail())
    {
        cout<<"文件不存在."<<endl;
        file.close();
        cin.get();
        cin.get();
    }
    else//文件存在
    {
        LINES=CountLines(filename);
        cout<<"读入文件行数："<<LINES<<endl;
        int a=0,b=0,c=0;

        int i=0;
        while(!file.eof()) //读取数据到数组
        {
            file>>a;
            file>>b;
            file>>c;
            i++;
            addEdge(graph, a,b,c);
        }
        file.close(); //关闭文件

    }

    // 初始化节点信息的表，首先将根节点能够直接到达的节点存进去。
    struct AdjListNode* pCrawl = graph->array[0].head;
    while (pCrawl != NULL)
    {
        int v = pCrawl->dest;
        prev[v].push_back(0);
        pCrawl = pCrawl->next;
    }


    dijkstra(graph, 0);

    cout << "源点到最后一个顶点的路径为: "<<endl;;
    int sta[maxnum];
    searchPath(prev, 0, 19999, sta, 0);
    return 0;
}
