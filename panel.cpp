// SPDX-License-Identifier: GPL-2.0-only
#include <LayerShellQt/shell.h>
#include <LayerShellQt/window.h>
#include <QPainter>
#include <QMetaEnum>
#include <QDebug>
#include <QPen>
#include <QMouseEvent>
#include "panel.h"
#include "taskbar.h"
#include "task.h"

Panel::Panel()
{
    LayerShellQt::Shell::useLayerShell();
    LayerShellQt::Window *layerShell = LayerShellQt::Window::get(this);

    // Panel margins
    layerShell->setMargins({ 0, 0, 0, 0 });

    // Panel layer
    const auto layer = QMetaEnum::fromType<LayerShellQt::Window::Layer>();
    layerShell->setLayer(LayerShellQt::Window::Layer(layer.keyToValue("LayerBottom")));

    // Panel position
    const QString panelAnchors = "AnchorTop|AnchorLeft|AnchorRight";
    const auto anchorEnums = QMetaEnum::fromType<LayerShellQt::Window::Anchor>();
    uint32_t anchors = 0;
    const auto stringList = panelAnchors.split(QLatin1Char('|'));
    for (const auto &value : stringList) {
        anchors |= anchorEnums.keyToValue(qPrintable(value));
    }
    layerShell->setAnchors((LayerShellQt::Window::Anchors)anchors);

    layerShell->setKeyboardInteractivity(LayerShellQt::Window::KeyboardInteractivity(0));

    // Panel size
    int height = 30;
    int width = 0;
    layerShell->setExclusiveZone(height);
    setHeight(height);
    setWidth(width);
}

Panel::~Panel() { }

void Panel::addPlugin(Taskbar *taskbar)
{
    m_taskbar = taskbar;
}

void Panel::mousePressEvent(QMouseEvent *event)
{
    // Forward the event to the task at the cursor position
    size_t num = m_taskbar->numTasks();
    for (auto i = 0; i < num; i++) {
        auto task = m_taskbar->task(i);
        const auto rect = task->rect();
        if (rect.contains(event->pos())) {
            task->mousePressEvent(event);
            return;
        }
    }
}

void Panel::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    const auto boundingRect = QRect(0, 0, width() - 1, height() - 1);

    // Background
    painter.fillRect(boundingRect, QColor("#222222"));

    // Border
    QPen pen(QColor("#404040"));
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(1.0);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(boundingRect);

    // Tasks
    QRect r;
    pen.setColor(QColor("#CCCCCC"));
    painter.setPen(pen);

    m_taskbar->updateTaskPositions();
    size_t num = m_taskbar->numTasks();
    for (auto i = 0; i < num; i++) {
        const auto task = m_taskbar->task(i);
        const auto rect = task->rect();

        // Icon
        QPoint pos = QPoint(rect.left() + 3, rect.top() + (30 - 22) / 2);
        painter.drawPixmap(pos, task->icon());

        // Text
        QString label = QString::fromStdString(task->app_id());
        painter.drawText(QRect(rect.left() + 28, rect.top(), rect.width(), rect.height()),
                         Qt::AlignVCenter, label, &r);
    }
}
