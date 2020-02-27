#include "Spire/OrderImbalanceIndicator/HistoricalOrderImbalanceChartView.hpp"
// TODO: remove
#include <random>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include "Spire/OrderImbalanceIndicator/OrderImbalanceIndicatorModel.hpp"
#include "Spire/Spire/Dimensions.hpp"
#include "Spire/Spire/Intervals.hpp"
#include "Spire/Ui/Ui.hpp"

using namespace Spire;

namespace {
  auto BOTTOM_MARGIN() {
    return scale_height(34);
  }

  auto LEFT_MARGIN() {
    return scale_width(31);
  }

  auto CHART_PADDING() {
    return scale_width(13);
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
  auto rand = std::default_random_engine(std::random_device()());
  auto time = boost::posix_time::ptime({2005, 10, 10});
  for(auto i = 0; i < 10; ++i) {
    m_imbalances.emplace_back(
      Nexus::OrderImbalance(Nexus::Security("TEST", 0), Nexus::Side::BID,
      Nexus::Quantity(rand() % 10000), Nexus::Money(rand() % 100), time));
    time += boost::posix_time::hours(12);
  }
}

void HistoricalOrderImbalanceChartView::leaveEvent(QEvent* event) {
  m_crosshair_pos = boost::none;
}

void HistoricalOrderImbalanceChartView::mouseMoveEvent(QMouseEvent* event) {
  if(QRect(LEFT_MARGIN(), 0, m_chart_size.width(), m_chart_size.height())
      .contains(event->pos())) {
    m_crosshair_pos = event->pos();
    setCursor(CROSSHAIR_CURSOR());
  } else {
    m_crosshair_pos = boost::none;
    setCursor(Qt::ArrowCursor);
  }
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
  auto gradient = QLinearGradient(0, m_chart_size.height(), 0, 0);
  gradient.setColorAt(0, QColor("#E2E0FF"));
  gradient.setColorAt(1, Qt::white);
  painter.fillRect(LEFT_MARGIN() + CHART_PADDING(), 0,
    m_chart_size.width() - (2 * CHART_PADDING()), m_chart_size.height(),
    gradient);
  if(m_crosshair_pos) {
    painter.setPen(m_dashed_line_pen);
    painter.drawLine(LEFT_MARGIN(), m_crosshair_pos->y(), width(),
      m_crosshair_pos->y());
    painter.drawLine(m_crosshair_pos->x(), 0, m_crosshair_pos->x(),
      m_chart_size.height());
  }
  auto cover = QPolygon();
  cover << QPoint(LEFT_MARGIN() + scale_width(1), 0) << QPoint(width(), 0);
  for(auto& imbalance : m_imbalances) {
    
  }
}

void HistoricalOrderImbalanceChartView::resizeEvent(QResizeEvent* event) {
  m_chart_size.setWidth(width() - LEFT_MARGIN());
  m_chart_size.setHeight(height() - BOTTOM_MARGIN());
}
