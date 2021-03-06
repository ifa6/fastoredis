#include "gui/widgets/log_widget.h"

#include <QEvent>
#include <QScrollBar>
#include <QTime>
#include <QMenu>
#include <QAction>
#include <QHBoxLayout>
#include <QTextEdit>

#include "translations/global.h"

namespace fastoredis
{
    LogWidget::LogWidget(QWidget* parent) 
        : QWidget(parent), logTextEdit_(new QTextEdit)
    {
        logTextEdit_->setReadOnly(true);
        logTextEdit_->setContextMenuPolicy(Qt::CustomContextMenu);
        VERIFY(connect(logTextEdit_, &QTextEdit::customContextMenuRequested, this, &LogWidget::showContextMenu));

        QHBoxLayout *hlayout = new QHBoxLayout;
        hlayout->setContentsMargins(0,0,0,0);
        hlayout->addWidget(logTextEdit_);
        clear_ = new QAction(this);
        VERIFY(connect(clear_, &QAction::triggered, logTextEdit_, &QTextEdit::clear));
        setLayout(hlayout);
        retranslateUi();
    }

    void LogWidget::addLogMessage(const QString& message, common::logging::LEVEL_LOG level)
    {
        QTime time = QTime::currentTime();
        logTextEdit_->setTextColor(level == common::logging::L_CRITICAL ? QColor(Qt::red):QColor(Qt::black));
        logTextEdit_->append(time.toString("h:mm:ss AP: ") + message);
        QScrollBar *sb = logTextEdit_->verticalScrollBar();
        sb->setValue(sb->maximum());
    }

    void LogWidget::showContextMenu(const QPoint& pt)
    {
        QMenu *menu = logTextEdit_->createStandardContextMenu();
        menu->addAction(clear_);
        clear_->setEnabled(!logTextEdit_->toPlainText().isEmpty());

        menu->exec(logTextEdit_->mapToGlobal(pt));
        delete menu;
    }

    void LogWidget::changeEvent(QEvent* ev)
    {
        if(ev->type() == QEvent::LanguageChange){
            retranslateUi();
        }

        QWidget::changeEvent(ev);
    }

    void LogWidget::retranslateUi()
    {
        using namespace translations;
        clear_->setText(trClearAll);
    }
}
