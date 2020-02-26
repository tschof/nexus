#include "Spire/OrderImbalanceIndicator/HistoricalOrderImbalanceChartView.hpp"
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include "Spire/Spire/Dimensions.hpp"
#include "Spire/Ui/Ui.hpp"

using namespace Spire;

namespace {
  auto BOTTOM_MARGIN() {
    return scale_height(34);
  }

  auto LEFT_MARGIN() {
    return scale_width(31);
  }

  const auto& CROSSHAIR_CURSOR() {
    // TODO: replace with proper cursor file
    static auto cursor = QCursor(QPixmap::fromImage(
      imageFromSvg(":/Icons/chart-cursor.svg", scale(18, 18))));
    return cursor;
  }
}

HistoricalOrderImbalanceChartView::HistoricalOrderImbalanceChartView(
    std::shared_ptr<OrderImbalanceIndicatorModel> model, QWidget* parent)
    : QWidget(parent),
      m_model(std::move(model)),
      m_dashed_line_pen(QColor("#333333"), scale_width(1),
        Qt::CustomDashLine) {
  setAttribute(Qt::WA_Hover);
  setMouseTracking(true);
  m_dashed_line_pen.setDashPattern({static_cast<double>(scale_width(3)),
    static_cast<double>(scale_width(3))});
}

void HistoricalOrderImbalanceChartView::leaveEvent(QEvent* event) {
  m_mouse_pos = QPoint();
}

void HistoricalOrderImbalanceChartView::mouseMoveEvent(QMouseEvent* event) {
  m_mouse_pos = event->pos();
  update();
}

void HistoricalOrderImbalanceChartView::paintEvent(QPaintEvent* event) {
  auto painter = QPainter(this);
  painter.fillRect(event->rect(), Qt::white);
  painter.setPen(Qt::black);
  painter.drawLine(LEFT_MARGIN(), 0, LEFT_MARGIN(),
    m_chart_size.height());
  painter.drawLine(LEFT_MARGIN(), m_chart_size.height(), width(),
    m_chart_size.height());
  if(QRect(LEFT_MARGIN(), 0, m_chart_size.width(), m_chart_size.height())
      .contains(m_mouse_pos)) {
    setCursor(CROSSHAIR_CURSOR());
    painter.setPen(m_dashed_line_pen);
    painter.drawLine(LEFT_MARGIN(), m_mouse_pos.y(), width(),
      m_mouse_pos.y());
    painter.drawLine(m_mouse_pos.x(), 0, m_mouse_pos.x(),
      m_chart_size.height());
  } else {
    setCursor(Qt::ArrowCursor);
  }
}

void HistoricalOrderImbalanceChartView::resizeEvent(QResizeEvent* event) {
  m_chart_size.setWidth(width() - LEFT_MARGIN());
  m_chart_size.setHeight(height() - BOTTOM_MARGIN());
}
