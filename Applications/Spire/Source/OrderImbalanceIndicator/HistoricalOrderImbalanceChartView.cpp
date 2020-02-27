#include "Spire/OrderImbalanceIndicator/HistoricalOrderImbalanceChartView.hpp"
// TODO: remove
#include <random>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include "Spire/OrderImbalanceIndicator/OrderImbalanceIndicatorModel.hpp"
#include "Spire/Spire/Dimensions.hpp"
#include "Spire/Spire/Utility.hpp"
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
    const TimeInterval& interval,
    std::shared_ptr<OrderImbalanceIndicatorModel> model, QWidget* parent)
    : QWidget(parent),
      m_interval(interval),
      m_minimum_value(std::numeric_limits<Nexus::Quantity>::max()),
      m_maximum_value(0),
      m_model(std::move(model)),
      m_dashed_line_pen(QColor("#333333"), scale_width(1),
        Qt::CustomDashLine) {
  setAttribute(Qt::WA_Hover);
  setMouseTracking(true);
  m_dashed_line_pen.setDashPattern({static_cast<double>(scale_width(3)),
    static_cast<double>(scale_width(3))});
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
  painter.setRenderHint(QPainter::Antialiasing);
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
    m_chart_size.width() - (2 * CHART_PADDING()),
    m_chart_size.height() - scale_height(1), gradient);
  if(m_crosshair_pos) {
    painter.save();
    painter.setPen(m_dashed_line_pen);
    painter.drawLine(LEFT_MARGIN(), m_crosshair_pos->y(), width(),
      m_crosshair_pos->y());
    painter.drawLine(m_crosshair_pos->x(), 0, m_crosshair_pos->x(),
      m_chart_size.height());
    painter.restore();
  }
  auto cover = QPolygon();
  cover << QPoint(LEFT_MARGIN() + scale_width(1), 0) << QPoint(width(), 0);
  for(auto point = m_data_points.begin(); point != m_data_points.end();
      ++point) {
    if(std::next(point) != m_data_points.end()) {
      painter.setPen({QColor("#4B23A0"), static_cast<qreal>(scale_width(3))});
      
      painter.drawLine(*point, *(point + 1));
    }
    painter.setPen(Qt::white);
    painter.setBrush(Qt::white);
    painter.drawEllipse(*point, scale_width(6), scale_width(6));
    painter.setPen(QColor("#4B23A0"));
    painter.setBrush(QColor("#4b23A0"));
    painter.drawEllipse(*point, scale_width(4), scale_width(4));
  }
}

void HistoricalOrderImbalanceChartView::resizeEvent(QResizeEvent* event) {
  m_chart_size.setWidth(width() - LEFT_MARGIN());
  m_chart_size.setHeight(height() - BOTTOM_MARGIN());
}

void HistoricalOrderImbalanceChartView::on_data_loaded(
    const std::vector<Nexus::OrderImbalance>& data) {
  m_imbalances.clear();
  m_data_points.clear();
  auto rand = std::default_random_engine(std::random_device()());
  auto time = boost::posix_time::ptime({2005, 10, 10});
  for(auto i = 0; i < 10; ++i) {
    m_imbalances.emplace_back(
      Nexus::OrderImbalance(Nexus::Security("TEST", 0), Nexus::Side::BID,
      Nexus::Quantity(rand() % 10000), Nexus::Money(rand() % 100), time));
    m_minimum_value = min(m_minimum_value, Scalar(m_imbalances.back().m_size));
    m_maximum_value = max(m_maximum_value, Scalar(m_imbalances.back().m_size));
    time += boost::posix_time::hours(12);
  }
  auto chart_drawable_width = m_chart_size.width() - (2 * CHART_PADDING());
  for(auto i = size_t(0); i < m_imbalances.size(); ++i) {
    qDebug() << "w: " << width();
    qDebug() << "cw: " << m_chart_size.width();
    auto x = LEFT_MARGIN() + CHART_PADDING() +
      static_cast<int>(static_cast<double>(chart_drawable_width) /
      static_cast<double>(m_imbalances.size() - 1) * static_cast<double>(i));
    auto y = map_to(m_imbalances[i].m_size,
      static_cast<Nexus::Quantity>(m_minimum_value),
      static_cast<Nexus::Quantity>(m_maximum_value),
      0 + scale_height(5), m_chart_size.height() - scale_height(8));
    m_data_points.push_back({x, y});
  }
}
