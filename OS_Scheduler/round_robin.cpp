#include "round_robin.h"

round_robin::round_robin()
{

}


int n;
QVector<int> q(100);
int front = -1, rear = -1;

void enqueue(int i)
{
    //  if (rear == 100)
    //     printf("overflow");
    rear++;
    q[rear] = i;
    if (front == -1)
        front = 0;
}

int dequeue()
{
    // if (front == -1)
    //     printf("underflow");
    int temp = q[front];
    if (front == rear)
        front = rear = -1;
    else
        front++;
    return temp;
}

int isInQueue(int i)
{
    int k;
    for (k = 0; k <= rear; k++)
    {
        if (q[k] == i)
            return 1;
    }
    return 0;
}

void sortByArrival(QVector<Process> p)
{
    Process temp(0,0,0,0);

    int i, j;
    for (i = 0; i < n - 1; i++)
    {
        for (j = i + 1; j < n; j++)
        {
            if (p[i].get_process_arrival_time() > p[j].get_process_arrival_time())
            {
                temp = p[i];
                p[i] = p[j];
                p[j] = temp;
            }
        }
    }
}

QVector<Process> round_robin::RR(QVector<Process> p, int tq,float &w_time) {
    int count = 0;
    QVector< Process > re;

    int n = p.size();

    int i, j, time = 0, sum_bt = 0;
    float avgwt = 0;
    float avgtt = 0;

    QVector<int> rt(n);
    QVector<int> completed(n);
    QVector<int> tt(n);
    QVector<int> wt(n);

    int r = 0 ;
    int st = 0;



    for (i = 0; i < n; i++)
    {

        rt[i] = p[i].get_process_burst_time();
        completed[i] = 0;
        sum_bt += p[i].get_process_burst_time();
    }

    sortByArrival(p);
    enqueue(0);

    for (time = p[0].get_process_arrival_time(); time < sum_bt;)
    {
        i = dequeue();
        if (rt[i] <= tq)
        {
            time += rt[i];
            r = rt[i];
            rt[i] = 0;
            completed[i] = 1;

            re.push_back(Process(p[i].get_process_Id(), r,0,0,st, st+r));
            st = st + r;

            count++;
            wt[i] = time - p[i].get_process_arrival_time() - p[i].get_process_burst_time();
            tt[i] = time - p[i].get_process_arrival_time();
            for (j = 0; j < n; j++)
            {
                if (p[j].get_process_arrival_time() <= time && completed[j] != 1 && isInQueue(j) != 1)
                {
                    enqueue(j);
                }
            }
        }
        else
        {

            time += tq;
            rt[i] -= tq;

            re.push_back(Process(p[i].get_process_Id(), tq, 0, 0,st,st+tq));
            st = st + tq;



            for (j = 0; j < n; j++)
            {
                if (p[j].get_process_arrival_time() <= time && completed[j] != 1 && i != j && isInQueue(j) != 1)
                {
                    enqueue(j);
                }
            }
            enqueue(i);
        }
    }

    for (i = 0; i < n; i++)
    {
        avgwt += wt[i];
        avgtt += tt[i];

    }
    //printf("\n\nTurnAround Time:%f \n", avgtt / n);

     w_time = avgwt / n;

    return re;

}

