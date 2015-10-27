// C / C++ʵ�ֵ� Dijkstra���·����ͼ���ڽӱ��ʾ,ʹ��binary heap�ӿ�����С�ڵ���ٶ�
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;
const int maxnum = 20000;
vector<int> prev[maxnum];  //��¼ǰ���ǰһ���ڵ�

int CountLines(char *filename)
{
    ifstream ReadFile;
    int n=0;
    char line[512];
    string temp;
    ReadFile.open(filename,ios::in);//ios::in ��ʾ��ֻ���ķ�ʽ��ȡ�ļ�
    if(ReadFile.fail())//�ļ���ʧ��:����0
    {
        return 0;
    }
    else//�ļ�����
    {
        while(getline(ReadFile,temp))
        {
            n++;
        }
        return n;
    }

    ReadFile.close();
}


// �ڽӱ�Ľڵ�
struct AdjListNode
{
    int dest;
    int weight;
    struct AdjListNode* next;
};

// �ڽӱ� �ṹ��
struct AdjList
{
    struct AdjListNode *head;  // ָ��ͷ�ڵ�
};

// ͼ�ṹ�壬VΪ���������arrayΪ���е��ڽӱ�
struct Graph
{
    int V;
    struct AdjList* array;
};

//�����ڽӱ�Ľڵ�
struct AdjListNode* newAdjListNode(int dest, int weight)
{
    struct AdjListNode* newNode = (struct AdjListNode*) malloc(
                                      sizeof(struct AdjListNode));
    newNode->dest = dest;
    newNode->weight = weight;
    newNode->next = NULL;
    return newNode;
}

//����һ��ͼ������V�Ķ���
struct Graph* createGraph(int V)
{
    struct Graph* graph = (struct Graph*) malloc(sizeof(struct Graph));
    graph->V = V;

    graph->array = (struct AdjList*) malloc(V * sizeof(struct AdjList));

    for (int i = 0; i < V; ++i)
        graph->array[i].head = NULL;

    return graph;
}

// ���һ����(����ͼ)
void addEdge(struct Graph* graph, int src, int dest, int weight)
{

    struct AdjListNode* newNode = newAdjListNode(dest, weight);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;

    newNode = newAdjListNode(src, weight);
    newNode->next = graph->array[dest].head;
    graph->array[dest].head = newNode;
}

// ��С�ѽڵ�
struct MinHeapNode
{
    int v;  //�±�
    int dist; //����
};

// ��С��
struct MinHeap
{
    int size;
    int capacity;
    int *pos;     // pos[i]��ʾ����i���ڵ��±�
    struct MinHeapNode **array;
};

// ����һ����С�ѽڵ�
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

// ����������С�ѵĽڵ�
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b)
{
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

//��λ�� idx ������
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
        // ��Ҫ�����Ľڵ�
        MinHeapNode *smallestNode = minHeap->array[smallest];
        MinHeapNode *idxNode = minHeap->array[idx];

        //�����±�
        minHeap->pos[smallestNode->v] = idx;
        minHeap->pos[idxNode->v] = smallest;

        //�����ڵ�
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);

        minHeapify(minHeap, smallest);
    }
}

// ���Ƿ�Ϊ��
int isEmpty(struct MinHeap* minHeap)
{
    return minHeap->size == 0;
}

// �����Ѷ��Ľڵ�(����С�Ľڵ�)
struct MinHeapNode* extractMin(struct MinHeap* minHeap)
{
    if (isEmpty(minHeap))
        return NULL;

    struct MinHeapNode* root = minHeap->array[0];

    struct MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;

    // �����±�
    minHeap->pos[root->v] = minHeap->size - 1;
    minHeap->pos[lastNode->v] = 0;

    // �ǵü��ٶѵĴ�С
    --minHeap->size;
    minHeapify(minHeap, 0);

    return root;
}

// ���ڵ�v�ľ�����º�(��С��)������
void decreaseKey(struct MinHeap* minHeap, int v, int dist)
{
    //��ȡ�ڵ� v �� ���е��±�
    int i = minHeap->pos[v];

    minHeap->array[i]->dist = dist;

    // ��Ϊ�Ǳ�С�ˣ��������ϵ����Ѽ��ɡ� O(Logn)
    while (i && minHeap->array[i]->dist < minHeap->array[(i - 1) / 2]->dist)
    {
        minHeap->pos[minHeap->array[i]->v] = (i - 1) / 2;
        minHeap->pos[minHeap->array[(i - 1) / 2]->v] = i;
        swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);

        i = (i - 1) / 2;
    }
}

// �жϽڵ�v�Ƿ��ڶ���
bool isInMinHeap(struct MinHeap *minHeap, int v)
{
    if (minHeap->pos[v] < minHeap->size)
        return true;
    return false;
}

// ��ӡ���
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

    // ��ʼ���Ѱ������еĶ���
    for (int v = 0; v < V; ++v)
    {
        dist[v] = INT_MAX;
        minHeap->array[v] = newMinHeapNode(v, dist[v]);
        minHeap->pos[v] = v;
    }

    // �� Դ�� src �ľ�������Ϊ0����һ��ȡ���ĵ㼴ΪԴ��
    minHeap->array[src] = newMinHeapNode(src, dist[src]);
    minHeap->pos[src] = src;
    dist[src] = 0;
    decreaseKey(minHeap, src, dist[src]);

    minHeap->size = V;

    // ���ѭ���У�minHeap������������δ��SPT�еĶ���
    while (!isEmpty(minHeap))
    {
        // ȡ�öѶ��ڵ㣬����С����Ķ���
        struct MinHeapNode* minHeapNode = extractMin(minHeap);
        int u = minHeapNode->v;

        // ֻ��Ҫ������u���ڵĶ�����и���
        struct AdjListNode* pCrawl = graph->array[u].head;
        while (pCrawl != NULL)
        {
            int v = pCrawl->dest;

            // �ɳڲ��������¾���
            if (isInMinHeap(minHeap, v) && dist[u] != INT_MAX
                    && pCrawl->weight + dist[u] <= dist[v])
            {
                if (pCrawl->weight + dist[u] < dist[v])
                {
                    prev[v].clear();
                    dist[v] = dist[u] + pCrawl->weight;
                    //���������֮��Ҫ������С��
                    decreaseKey(minHeap, v, dist[v]);
                }
                prev[v].push_back(u);
            }
            pCrawl = pCrawl->next;
        }
    }

    // ��ӡ
    //printArr(dist, V);
    cout<<"Vertex   Distance from Source"<<endl;
    cout<< 19999<<"    "<<dist[19999]<<endl;
}

// ���Ҵ�Դ��v���յ�u��·���������
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


// ����
int main()
{

    int V = 20000;
    struct Graph* graph = createGraph(V);
    ifstream file;
    int LINES;
    char filename[512];
    cout<<"������Ҫ�򿪵��ļ���:"<<endl;
    cin>>filename;
    file.open(filename,ios::in);
    if(file.fail())
    {
        cout<<"�ļ�������."<<endl;
        file.close();
        cin.get();
        cin.get();
    }
    else//�ļ�����
    {
        LINES=CountLines(filename);
        cout<<"�����ļ�������"<<LINES<<endl;
        int a=0,b=0,c=0;

        int i=0;
        while(!file.eof()) //��ȡ���ݵ�����
        {
            file>>a;
            file>>b;
            file>>c;
            i++;
            addEdge(graph, a,b,c);
        }
        file.close(); //�ر��ļ�

    }

    // ��ʼ���ڵ���Ϣ�ı����Ƚ����ڵ��ܹ�ֱ�ӵ���Ľڵ���ȥ��
    struct AdjListNode* pCrawl = graph->array[0].head;
    while (pCrawl != NULL)
    {
        int v = pCrawl->dest;
        prev[v].push_back(0);
        pCrawl = pCrawl->next;
    }


    dijkstra(graph, 0);

    cout << "Դ�㵽���һ�������·��Ϊ: "<<endl;;
    int sta[maxnum];
    searchPath(prev, 0, 19999, sta, 0);
    return 0;
}
