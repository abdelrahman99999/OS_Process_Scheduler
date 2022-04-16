#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QItemDelegate>
#include<QColor>
#include<QDebug>
#include"process.h"
#include<QtAlgorithms>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString init="1. FCFS";
    on_algorithm_comboBox_currentTextChanged(init);

    ui->data_table->setColumnCount(4);
    ui->data_table->setRowCount(0);
    ui->data_table->hideColumn(3);
    QStringList h;
    h<<"process id"<<"burst time"<<"arrival time"<<"priority";
    ui->data_table->setHorizontalHeaderLabels(h);
    ui->data_table->setItem(0,0, new QTableWidgetItem);
}


MainWindow::~MainWindow()
{
    delete ui;

}

void MainWindow::on_algorithm_comboBox_currentTextChanged(const QString &arg1)
{
    if( arg1=="3. Priority"){
        ui->preemptive->show();
        ui->Non_preemptive->show();
        ui->algo_type->show();

        ui->quantum_label->hide();
        ui->quantum_value->hide();

        ui->data_table->showColumn(3);
    }
    else if(arg1=="2. SJF" ){
        ui->preemptive->show();
        ui->Non_preemptive->show();
        ui->algo_type->show();

        ui->quantum_label->hide();
        ui->quantum_value->hide();

        ui->data_table->hideColumn(3);
    }
    else if(arg1=="4. Round Robin" ){
        ui->preemptive->hide();
        ui->Non_preemptive->hide();
        ui->algo_type->hide();

        ui->quantum_label->show();
        ui->quantum_value->show();

        ui->data_table->hideColumn(3);

    }
    else if("1. FCFS"){
        ui->preemptive->hide();
        ui->Non_preemptive->hide();
        ui->algo_type->hide();

        ui->quantum_label->hide();
        ui->quantum_value->hide();

        ui->data_table->hideColumn(3);
    }
}


void MainWindow::on_no_of_process_value_valueChanged(int arg1)
{
    ui->data_table->setRowCount(arg1);
}



QVector<Process> MainWindow::get_data_from_table(){
    int no_of_processes=ui->no_of_process_value->value();
    QVector<Process>v;

    for(int i=0;i<no_of_processes;i++){
       int priority=0;
       if(ui->algorithm_comboBox->currentText()=="3. Priority"){
           priority = ui->data_table->item(i,3)->text().toInt();
       }
       Process p(ui->data_table->item(i,0)->text().toInt(),
                 ui->data_table->item(i,1)->text().toInt(),
                 ui->data_table->item(i,2)->text().toInt(),
                 priority);
       v.push_back(p);
    }
    return v;
}

void MainWindow::on_simulate_button_clicked()
{
    if(ui->no_of_process_value->value()>0){
        QVector<Process>v = get_data_from_table();
        QString algorithm =ui->algorithm_comboBox->currentText();
        QString algo_type;
        if(algorithm =="3. Priority" || algorithm=="2. SJF"){
            if(ui->preemptive->isChecked()){
                algo_type ="preemptive";
            }else{
                algo_type="non_preemptive";
            }
        }
        float avg_waiting=0;
        QVector<Process>processes;
        if(algorithm=="1. FCFS"){
            processes=fcfs(v,avg_waiting);
        }
        else if(algorithm=="2. SJF"){
            if(algo_type=="preemptive"){
                processes=sjf_preemptive(v,avg_waiting);
            }else{
                processes=sjf_non_preemptive(v,avg_waiting);
            }
        }
        else if(algorithm=="3. Priority"){
            if(algo_type=="preemptive"){
                //processes=priority_preemptive(v,avg_waiting);
            }else{
                //processes=priority_preemptive(v,avg_waiting);
            }
        }
        else if(algorithm=="4. Round Robin"){
            int quantum=ui->quantum_value->value();
            //processes=RR(v,quantum);
        }
        draw(processes,avg_waiting);
        set_process_time_line(processes);
    }
}

struct compare_process
{
    inline bool operator() (const Process& lhs,const  Process& rhs)
    {
        return (lhs.get_process_arrival_time() < rhs.get_process_arrival_time());
    }
};

QVector<Process>  MainWindow::fcfs(QVector<Process>v,float &avg_waiting){
    std::stable_sort(v.begin(),v.end(),compare_process());
    int n=v.size();
    v[0].set_start_time(v[0].get_process_arrival_time());
    v[0].set_end_time(v[0].get_process_arrival_time()+v[0].get_process_burst_time());

    int waiting_time=0;
    for(int i=1;i<n;i++){
        v[i].set_start_time(v[i-1].get_end_time());
        v[i].set_end_time(v[i].get_start_time()+v[i].get_process_burst_time());
        waiting_time+=v[i].get_start_time()-v[i].get_process_arrival_time();
    }

    avg_waiting=(waiting_time*1.0)/n;
    return v;
}

void MainWindow::draw(QVector<Process>v,float avg_w){
    int n=v.size();
    ui->gantt_chart->setColumnCount(n);
    ui->gantt_chart->setRowCount(1);
    QStringList h;
    h<<"time";
    ui->gantt_chart->setVerticalHeaderLabels(h);

    QStringList h2;
    int total_time=0;
    for(int i=0;i<n;i++){
        total_time+=v[i].get_process_burst_time();
        h2<<"p"+QString::number(v[i].get_process_Id());
        QString s=QString::number(v[i].get_process_burst_time());
        ui->gantt_chart->setItem(0,i, new QTableWidgetItem);
        ui->gantt_chart->item(0,i)->setText(s);
        ui->gantt_chart->item(0,i)->setTextAlignment(Qt::AlignCenter);
        ui->gantt_chart->item(0,i)->setBackground(Qt::gray);
    }
    for(int i=0;i<n;i++){
        ui->gantt_chart->setColumnWidth(i,  v[i].get_process_burst_time()*1080/total_time);
    }
    ui->gantt_chart->setHorizontalHeaderLabels(h2);
    ui->avg_wait->setText("Average Waiting Time: "+QString::number(avg_w));
}

void MainWindow::set_process_time_line(QVector<Process>v){
    int n=v.size();
    ui->timeline->setColumnCount(3);
    ui->timeline->setRowCount(n);
    QStringList h;
    h<<"process Id"<<"start time"<<"end time";
    ui->timeline->setHorizontalHeaderLabels(h);

    for(int i=0;i<n;i++){
        ui->timeline->setItem(i,0, new QTableWidgetItem);
        ui->timeline->setItem(i,1, new QTableWidgetItem);
        ui->timeline->setItem(i,2, new QTableWidgetItem);
        ui->timeline->item(i,0)->setText(QString::number(v[i].get_process_Id()));
        ui->timeline->item(i,1)->setText(QString::number(v[i].get_start_time()));
        ui->timeline->item(i,2)->setText(QString::number(v[i].get_end_time()));
    }
}

QVector<Process> MainWindow::sjf_non_preemptive (QVector<Process>v, float& avg_waiting) {
    int n = v.size();
    QVector<bool>taken;
    for (int i = 0; i < n; i++) {
        taken.push_back(false);
    }
    QVector<Process>v2;
    QVector<Process>v3;
    int curr_time = 0;
    bool x = true;
    while (x) {
        for (int i = 0; i < n; i++) {
            if (v[i].get_process_arrival_time() <= curr_time && taken[i] == false) {
                v2.push_back(v[i]);
                taken[i] = true;
            }
        }
        int min_burst_index = 0;
        for (int i = 1; i < v2.size(); i++) {
            if (v2[i].get_process_burst_time() < v2[min_burst_index].get_process_burst_time()) {
                min_burst_index = i;
            }
        }
        if (v3.empty()) {
            v3.push_back(v2[min_burst_index]);
            v3[0].set_start_time(0);
            v3[0].set_end_time(v3[0].get_process_burst_time());
            curr_time = v3[0].get_process_burst_time();
            v2.erase(v2.begin() + min_burst_index);
        }
        else {
            int v3_size = v3.size();
            v3.push_back(v2[min_burst_index]);
            v3[v3_size].set_start_time(v3[v3_size - 1].get_end_time());
            v3[v3_size].set_end_time(v3[v3_size - 1].get_end_time() + v3[v3_size].get_process_burst_time());
            curr_time += v3[v3_size].get_process_burst_time();

            v2.erase(v2.begin()+ min_burst_index);

        }
        if (v3.size() == v.size()) {
            x = false;
        }

    }
    int waiting_time = 0;
    for (int i = 0; i < v3.size(); i++) {
        waiting_time += v3[i].get_start_time() - v3[i].get_process_arrival_time();
    }

    avg_waiting = (waiting_time * 1.0) / n;
    return v3;

}


QVector<Process> MainWindow::sjf_preemptive(QVector<Process> processes, float &avr_wait)
{

    int n = processes.size();
    int current_time = 0;
    int completed = 0;
    int total_waiting_time = 0;
    QVector<Process> ordered_processes; // ordered of processes in Gantt chart

    QVector<int> remaining_time(n);
    for (int i = 0; i < n; i++)
    {
        remaining_time[i] = processes[i].get_process_burst_time();
        // if process has no burst time
        if (remaining_time[i] <= 0)
            completed++;
    }

    int shortest_process = 1e9; // track the process that has shortest burst time at that point
    while (completed != n)
    {
        int indx = -1; // save indx of shortest process

        // get indx of shortest process
        for (int i = 0; i < n; i++)
        {
            if ((processes[i].get_process_arrival_time() <= current_time) && (remaining_time[i] <= shortest_process) && (remaining_time[i] > 0))
            {
                shortest_process = remaining_time[i];
                indx = i;
            }
        }

        if (indx != -1)
        {
            Process curr_proc = processes[indx];

            // decrementing the remaining time
            remaining_time[indx]--;
            shortest_process = remaining_time[indx];

            if (ordered_processes.empty() || curr_proc.get_process_Id() != ordered_processes.back().get_process_Id())
            {
                curr_proc.set_start_time(current_time);
                ordered_processes.push_back(curr_proc);
            }
            ordered_processes.back().set_end_time(current_time + 1);

            // If a process gets completely
            if (remaining_time[indx] == 0)
            {
                int finish_time = current_time + 1;

                int wait = finish_time - curr_proc.get_process_arrival_time() - curr_proc.get_process_burst_time();
                if (wait < 0)
                    wait = 0;

                total_waiting_time += wait;
                completed++;
            }
            if (shortest_process == 0)
                shortest_process = 1e9;
        }

        // Increment time
        current_time++;
    }
    for(int i=0;i<ordered_processes.size();i++){
            ordered_processes[i].set_process_burst_time(ordered_processes[i].get_end_time()-ordered_processes[i].get_start_time());
    }

    // outputs
    avr_wait = (float)total_waiting_time / n;
    return ordered_processes;
}
