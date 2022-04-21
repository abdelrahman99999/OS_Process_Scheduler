#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QColor>
#include<QMessageBox>
#include"process.h"
#include<QtAlgorithms>
#include"fcfs_algorithm.h"
#include"sjf_algorithm.h"
#include"priority_algorithm.h"
#include "round_robin.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
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
    ui->quantum_value->setValue(2);
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

bool MainWindow::vaild_data(){
    QTableWidgetItem* item;
    QTableWidgetItem* item1;
    QTableWidgetItem* item2;
    QTableWidgetItem* item3;
    int no_of_processes=ui->no_of_process_value->value();
    for(int i=0;i<no_of_processes;i++){
        if(ui->algorithm_comboBox->currentText()=="3. Priority"){
             item = ui->data_table->item(i,3);
             if (!item || item->text().isEmpty() || item->text().toInt()<0)
             {
                 QMessageBox::critical(this,"error","please enter vaild data !!");
                return 0;
             }
        }
        item1 = ui->data_table->item(i,0);
        item2 = ui->data_table->item(i,1);
        item3 = ui->data_table->item(i,2);
        if (    (!item1 || item1->text().isEmpty()||item1->text().toInt()<0)    ||
                (!item2 || item2->text().isEmpty()||item2->text().toInt()<0)    ||
                (!item3 || item3->text().isEmpty()||item3->text().toInt()<0)   )
        {
            QMessageBox::critical(this,"error","please enter vaild data !!");
           return 0;
        }
    }
    return 1;

}

void MainWindow::on_simulate_button_clicked()
{
    if(ui->no_of_process_value->value()>0 && vaild_data()){
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
            processes=FCFS_Algorithm::fcfs(v,avg_waiting);
        }
        else if(algorithm=="2. SJF"){
            if(algo_type=="preemptive"){
                processes=sjf_algorithm::sjf_preemptive(v,avg_waiting);
            }else{
                processes=sjf_algorithm::sjf_non_preemptive(v,avg_waiting);
            }
        }
        else if(algorithm=="3. Priority"){
            if(algo_type=="preemptive"){
                processes=priority_algorithm::Priority_preemitive(v,avg_waiting);
            }else{
                processes=priority_algorithm::Priority_nonPreemptive(v,avg_waiting);
            }
        }
        else if(algorithm=="4. Round Robin"){
            int quantum=ui->quantum_value->value();
            if(quantum==0){
                QMessageBox::critical(this,"error","please enter vaild quantum !!");
                return;
            }
            processes=round_robin::RR(v,quantum,avg_waiting);
        }
        draw(processes,avg_waiting);
        set_process_time_line(processes);
    }
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
