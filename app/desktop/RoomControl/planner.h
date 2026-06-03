#ifndef PLANNER_H
#define PLANNER_H

#include <QWidget>
#include "ui_TaskItem.h"

namespace Ui {
class Planner;
class TaskItem;
}

struct TaskView
{
    QWidget* root = nullptr;
    Ui::TaskItem* ui = nullptr;
};

class Planner : public QWidget
{
    Q_OBJECT

public:
    explicit Planner(QWidget *parent = nullptr);
    ~Planner();

private:
    Ui::Planner *ui;
    QList<TaskView*> current_tasks;
    QList<TaskView*> finished_tasks;

    void addTask();
    void clearTasks();
    void completeTask(TaskView* task);
    void cancelTask(TaskView* task);
    void deleteTask(TaskView* task);

};

#endif // PLANNER_H
