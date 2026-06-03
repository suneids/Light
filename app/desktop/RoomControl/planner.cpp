#include "planner.h"
#include "ui_planner.h"
#include "ui_TaskItem.h"

Planner::Planner(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Planner)
{
    ui->setupUi(this);

    ui->vlayout_current_tasks->setAlignment(Qt::AlignTop);
    ui->vlayout_finished_tasks->setAlignment(Qt::AlignTop);

    connect(ui->btn_add_task, &QPushButton::clicked,
            this, &Planner::addTask);
    connect(ui->btn_clear_tasks, &QPushButton::clicked,
            this, &Planner::clearTasks);

    connect(ui->btn_current_tasks, &QPushButton::clicked, this, [this](){
        ui->stackedWidget->setCurrentWidget(ui->pg_current_tasks);
    });
    connect(ui->btn_finished_tasks, &QPushButton::clicked, this, [this](){
        ui->stackedWidget->setCurrentWidget(ui->pg_finished_tasks);
    });
}


void Planner::addTask(){
    auto* root = new QWidget(this);
    auto* taskUi = new Ui::TaskItem;

    taskUi->setupUi(root);

    auto* task = new TaskView;
    task->root = root;
    task->ui = taskUi;

    current_tasks.append(task);
    connect(taskUi->btn_complete, &QPushButton::clicked, this, [this, task]() {
        completeTask(task);
    });

    connect(taskUi->btn_cancel, &QPushButton::clicked, this, [this, task]() {
        cancelTask(task);
    });

    ui->vlayout_current_tasks->addWidget(root);
}


void Planner::clearTasks()
{
    while (!current_tasks.isEmpty()) {
        deleteTask(current_tasks.first());
    }
}

void Planner::completeTask(TaskView* task){
    if (!task) {
        return;
    }

    if (!current_tasks.removeOne(task)) {
        return;
    }

    ui->vlayout_current_tasks->removeWidget(task->root);

    finished_tasks.append(task);
    ui->vlayout_finished_tasks->addWidget(task->root);

    task->ui->btn_complete->setEnabled(false);
    task->ui->ledit_description->setEnabled(false);
    task->ui->ledit_header->setEnabled(false);

    task->ui->btn_complete->setText("Выполнено");

    task->ui->btn_cancel->setText("Удалить");
}


void Planner::cancelTask(TaskView* task){
    if (!task) {
        return;
    }

    // потом сюда можно добавить статус cancelled

    deleteTask(task);
}


void Planner::deleteTask(TaskView* task){
    if (!task) {
        return;
    }
    if (!task) {
        return;
    }

    bool wasCurrent = current_tasks.removeOne(task);
    bool wasFinished = finished_tasks.removeOne(task);

    if (wasCurrent) {
        ui->vlayout_current_tasks->removeWidget(task->root);
    }

    if (wasFinished) {
        ui->vlayout_finished_tasks->removeWidget(task->root);
    }

    delete task->ui;
    task->root->deleteLater();
    delete task;
}


Planner::~Planner()
{
    for (TaskView* task : current_tasks) {
        delete task->ui;
        delete task->root;
        delete task;
    }
    for (TaskView* task : finished_tasks) {
        delete task->ui;
        delete task->root;
        delete task;
    }
    delete ui;
}
