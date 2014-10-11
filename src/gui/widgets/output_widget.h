#pragma once

#include <QWidget>

#include "core/events/events_info.h"

class QAction;
class QPushButton;

namespace fastoredis
{
    class FastoEditor;
    class FastoTreeView;
    class FastoTableView;
    class FastoTreeModel;
    class IconLabel;

    class OutputWidget
            : public QWidget
    {
        Q_OBJECT

    public:
        OutputWidget(QWidget* parent = 0);

    public Q_SLOTS:
        void startExecute(const EventsInfo::ExecuteInfoRequest &);
        void finishExecute(const EventsInfo::ExecuteInfoResponce &);

    private Q_SLOTS:
        void setTreeView();
        void setTableView();
        void setTextView();

    private:
        void syncWithSettings();
        IconLabel *_timeLabel;
        QPushButton *_treeButton;
        QPushButton *_tableButton;
        QPushButton *_textButton;

        FastoTreeModel *_treeModel;
        FastoTreeView *_treeView;
        FastoTableView *_tableView;
        FastoEditor *_textView;
    };
}