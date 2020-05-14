#include "Spire/KeyBindings/CustomTabWidget.hpp"
#include <QPainter>
#include <QTabBar>
#include "Spire/Spire/Dimensions.hpp"

using namespace Spire;

CustomTabWidget::CustomTabWidget(QWidget* parent)
    : QTabWidget(parent) {
  tabBar()->setFixedHeight(scale_height(40));
  setStyleSheet(QString(R"(
    QWidget {
      outline: none;
    }

    QTabWidget::tab-bar {
      left: %7px;
    }

    QTabWidget::pane {
      border: none;
    }

    QTabBar::tab {
      background-color: #EBEBEB;
      font-family: Roboto;
      font-size: %1px;
      height: %2px;
      margin: %3px %4px %3px 0px;
      width: %5px;
    }

    QTabBar::tab:focus {
      border: %6px solid #4B23A0;
      padding: -%6px 0px 0px -%6px;
    }

    QTabBar::tab:hover {
      color: #4B23A0;
    }

    QTabBar::tab:selected {
      background-color: #F5F5F5;
      color: #4B23A0;
    })").arg(scale_height(12)).arg(scale_height(20)).arg(scale_height(10))
        .arg(scale_width(2)).arg(scale_width(80)).arg(scale_width(1))
        .arg(scale_width(8)));
}

void CustomTabWidget::paintEvent(QPaintEvent* event) {
  auto painter = QPainter(this);
  painter.setPen(QColor("#E0E0E0"));
  painter.drawLine(tabBar()->width(), tabBar()->height() - scale_height(11),
    width(), tabBar()->height() - scale_height(11));
}
