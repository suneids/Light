#include "timetracker.h"
#include "ui_timetracker.h"
#include <QHeaderView>

static void initTimeTable(QTableWidget* table)
{
    table->setColumnCount(2);
    table->setHorizontalHeaderLabels({"Окно", "Время"});

    table->horizontalHeader()->setStretchLastSection(false);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    table->verticalHeader()->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
}


TimeTracker::TimeTracker(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TimeTracker)
{
    ui->setupUi(this);

    ui->tbl_total->setStyleSheet(ui->tbl_current->styleSheet());
    initTimeTable(ui->tbl_current);
    initTimeTable(ui->tbl_total);
    ui->tbl_current->setHorizontalHeaderLabels({"Окно", "Время"});
    ui->tbl_total->setHorizontalHeaderLabels({"Окно", "Время"});

    connect(&tickTimer, &QTimer::timeout, this, &TimeTracker::tick);
    connect(&periodTimer, &QTimer::timeout, this, &TimeTracker::resetPeriod);

    tickTimer.start(1000);
    periodTimer.start(5 * 60 * 1000);

    lastTick = QDateTime::currentDateTime();
    currentWindow = getActiveWindowName();

    ui->lbl_active_window->setText(currentWindow);
}


TimeTracker::~TimeTracker()
{
    delete ui;
}


void TimeTracker::tick()
{
    QDateTime now = QDateTime::currentDateTime();

    if (!lastTick.isValid()) {
        lastTick = now;
        currentWindow = getActiveWindowName();
        return;
    }

    qint64 dt = lastTick.secsTo(now);
    lastTick = now;

    if (dt <= 0 || dt > 10) {
        return;
    }

    QString activeWindow = getActiveWindowName();

    if (currentWindow.isEmpty()) {
        currentWindow = activeWindow;
    }

    addTime(currentWindow, dt);

    if (activeWindow != currentWindow) {
        currentWindow = activeWindow;
        ui->lbl_active_window->setText(currentWindow);
    }

    updateCurrentTable();
    updateTotalTable();
}


void TimeTracker::addTime(const QString& windowName, qint64 seconds)
{
    if (windowName.isEmpty()) {
        return;
    }

    currentPeriod[windowName] += seconds;
    total[windowName] += seconds;
}


void TimeTracker::resetPeriod()
{
    currentPeriod.clear();
    updateCurrentTable();
}


void TimeTracker::updateTotalTable()
{
    ui->tbl_total->setRowCount(0);

    int row = 0;

    for (auto it = total.begin(); it != total.end(); ++it) {
        ui->tbl_total->insertRow(row);

        ui->tbl_total->setItem(row, 0, new QTableWidgetItem(it.key()));
        ui->tbl_total->setItem(row, 1, new QTableWidgetItem(formatSeconds(it.value())));

        row++;
    }
}


void TimeTracker::updateCurrentTable(){
    ui->tbl_current->setRowCount(0);
    int row = 0;

    for (auto it = currentPeriod.begin(); it != currentPeriod.end(); ++it) {
        ui->tbl_current->insertRow(row);

        ui->tbl_current->setItem(row, 0, new QTableWidgetItem(it.key()));
        ui->tbl_current->setItem(row, 1, new QTableWidgetItem(formatSeconds(it.value())));

        row++;
    }
}


QString TimeTracker::getActiveWindowName()
{
#ifdef Q_OS_WIN
    HWND hwnd = GetForegroundWindow();

    if (!hwnd) {
        return "Unknown";
    }

    wchar_t title[512];
    int titleLength = GetWindowTextW(hwnd, title, 512);

    QString windowTitle;

    if (titleLength > 0) {
        windowTitle = QString::fromWCharArray(title, titleLength);
    } else {
        windowTitle = "No title";
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);

    QString processName;

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);

    if (process) {
        wchar_t path[MAX_PATH];
        DWORD size = MAX_PATH;

        if (QueryFullProcessImageNameW(process, 0, path, &size)) {
            QString fullPath = QString::fromWCharArray(path, size);
            processName = QFileInfo(fullPath).fileName();
        }

        CloseHandle(process);
    }

    if (!processName.isEmpty()) {
        return processName;
    }

    return windowTitle;
#else
    return "Unsupported OS";
#endif
}


QString TimeTracker::formatSeconds(qint64 seconds) const
{
    qint64 h = seconds / 3600;
    qint64 m = (seconds % 3600) / 60;
    qint64 s = seconds % 60;

    if (h > 0) {
        return QString("%1:%2:%3")
            .arg(h)
            .arg(m, 2, 10, QChar('0'))
            .arg(s, 2, 10, QChar('0'));
    }

    return QString("%1:%2")
        .arg(m, 2, 10, QChar('0'))
        .arg(s, 2, 10, QChar('0'));
}
