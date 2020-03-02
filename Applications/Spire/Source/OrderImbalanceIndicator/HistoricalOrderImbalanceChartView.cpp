#include "Spire/OrderImbalanceIndicator/HistoricalOrderImbalanceChartView.hpp"
// TODO: remove
#include <random>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QWheelEvent>
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
      m_is_dragging(false),
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
  if(m_is_dragging) {
    //qDebug() << "chart duration: " << (m_interval.upper() - m_interval.lower()).hours();
    auto pixel_delta = static_cast<int>(
      static_cast<double>(event->x() - m_last_mouse_pos.x()) /
      static_cast<double>(m_chart_size.width()) * 100);
    //qDebug() << "pixel delta: " << pixel_delta;
    if(pixel_delta != 0) {
      auto chart_delta = (m_interval.upper() - m_interval.lower()) / 100 * pixel_delta;
      //qDebug() << "delta: " << chart_delta.hours();
      //qDebug() << "before lower day: " << m_interval.lower().date().day();
      //qDebug() << "before upper day: " << m_interval.upper().date().day();
      //qDebug() << "hours: " << chart_delta.hours();
      static auto num = 0;
      if(event->x() < m_last_mouse_pos.x()) {
          if(m_imbalances.back().m_timestamp >= m_interval.upper()) {
            qDebug() << "left: " << ++num;
            m_interval = {m_interval.lower() - chart_delta,
              m_interval.upper() - chart_delta};
          }
      } else {
        if(m_imbalances.front().m_timestamp <= m_interval.lower()) {
          qDebug() << "right: " << ++num;
          m_interval = {m_interval.lower() - chart_delta,
            m_interval.upper() - chart_delta};
        }
      }
      //qDebug() << "after range: " << (m_interval.upper() - m_interval.lower()).hours();
      //qDebug() << "after lower day: " << m_interval.lower().date().day();
      //qDebug() << "after upper day: " << m_interval.upper().date().day();
      m_last_mouse_pos = event->pos();
      on_data_loaded({});
    }
  }
  update();
}

void HistoricalOrderImbalanceChartView::mousePressEvent(QMouseEvent* event) {
  if(event->button() == Qt::LeftButton) {
    m_is_dragging = true;
    m_last_mouse_pos = event->pos();
  }
}

void HistoricalOrderImbalanceChartView::mouseReleaseEvent(QMouseEvent* event) {
  if(event->button() == Qt::LeftButton) {
    m_is_dragging = false;
  }
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
    painter.setPen(m_dashed_line_pen);
    painter.drawLine(LEFT_MARGIN(), m_crosshair_pos->y(), width(),
      m_crosshair_pos->y());
    painter.drawLine(m_crosshair_pos->x(), 0, m_crosshair_pos->x(),
      m_chart_size.height());
  }
  auto cover = QPolygon();
  cover << QPoint(LEFT_MARGIN() + scale_width(1), 0) << QPoint(width(), 0);
  if(m_data_points.size() == 1) {
    auto point1 = m_data_points.front();
    point1.setX(LEFT_MARGIN() + CHART_PADDING());
    auto point2 = m_data_points.front();
    point2.setX(point1.x() + m_chart_size.width() - (2 * CHART_PADDING()));
    draw_line(painter, point1, point2);
    draw_point(painter, point1);
    draw_point(painter, point2);
  } else {
    for(auto point = m_data_points.begin(); point != m_data_points.end();
        ++point) {
      if(std::next(point) != m_data_points.end()) {
        draw_line(painter, *point, *(point + 1));
      }
      draw_point(painter, *point);
    }
  }
}

void HistoricalOrderImbalanceChartView::resizeEvent(QResizeEvent* event) {
  m_chart_size.setWidth(width() - LEFT_MARGIN());
  m_chart_size.setHeight(height() - BOTTOM_MARGIN());
  on_data_loaded({});
}

void HistoricalOrderImbalanceChartView::wheelEvent(QWheelEvent* event) {
  auto chart_range = m_interval.upper() - m_interval.lower();
  if(event->angleDelta().y() < 0) {
    if(m_interval.lower() > m_imbalances.front().m_timestamp ||
        m_interval.upper() < m_imbalances.back().m_timestamp) {
      //qDebug() << "********************************";
      //qDebug() << "out";
      //qDebug() << "********************************";
      //qDebug() << "before";
      //qDebug() << "range: " << (m_interval.upper() - m_interval.lower()).hours();
      auto b = (chart_range * 110);
      //qDebug() << "b: " << b.hours();
      auto c = b / 100;
      //qDebug() << "c: " << c.hours();
      auto a = (chart_range - ((chart_range * 110) / 100)) / 2;
      //qDebug() << "a: " << a.hours();
      m_interval = {m_interval.lower() + a, m_interval.upper() - a};
      //qDebug() << "result: " << (m_interval.upper() - m_interval.lower()).hours();
    }
  } else {
    // TODO: limit zoom in
    if(m_data_points.size() > 1) {
      //qDebug() << "********************************";
      //qDebug() << "in";
      //qDebug() << "********************************";
      //qDebug() << "range: " << (m_interval.upper() - m_interval.lower()).hours();
      auto b = (chart_range * 100);
      //qDebug() << "b: " << b.hours();
      auto c = b / 110;
      //qDebug() << "c: " << c.hours();
      auto a = ((chart_range * 100) / 110 - chart_range) / 2;
      //qDebug() << "a: " << a.hours();
      m_interval = {m_interval.lower() - a, m_interval.upper() + a};
      //qDebug() << "result: " << (m_interval.upper() - m_interval.lower()).hours();
    }
  }
  on_data_loaded({});
  update();
}

void HistoricalOrderImbalanceChartView::draw_line(QPainter& painter,
    const QPoint& point1, const QPoint& point2) {
  painter.setPen({QColor("#4B23A0"), static_cast<qreal>(scale_width(3))});
  painter.drawLine(point1, point2);
}

void HistoricalOrderImbalanceChartView::draw_point(QPainter& painter,
    const QPoint& point) {
  painter.setPen(Qt::white);
  painter.setBrush(Qt::white);
  painter.drawEllipse(point, scale_width(6), scale_width(6));
  painter.setPen(QColor("#4B23A0"));
  painter.setBrush(QColor("#4b23A0"));
  painter.drawEllipse(point, scale_width(4), scale_width(4));
}

void HistoricalOrderImbalanceChartView::on_data_loaded(
    const std::vector<Nexus::OrderImbalance>& data) {
  //qDebug() << height();
  //qDebug() << "c h: " << m_chart_size.height();
  m_minimum_value = Scalar(std::numeric_limits<Nexus::Quantity>::max());
  m_maximum_value = Scalar(0);
  m_data_points.clear();
  if(m_imbalances.empty()) {
    auto rand = std::default_random_engine(std::random_device()());
    auto time = boost::posix_time::ptime({2005, 8, 1});
    for(auto i = 0; i < 506; ++i) {
      m_imbalances.emplace_back(
        Nexus::OrderImbalance(Nexus::Security("TEST", 0), Nexus::Side::BID,
        Nexus::Quantity(rand() % 10000), Nexus::Money(rand() % 100), time));
      time += boost::posix_time::hours(12);
    }
    //m_imbalances.emplace_back(
    //  Nexus::OrderImbalance(Nexus::Security("TEST", 0), Nexus::Side::BID,
    //  Nexus::Quantity(20), Nexus::Money(rand() % 100),
    //  boost::posix_time::ptime({2005, 9, 1})));
    //m_imbalances.emplace_back(
    //  Nexus::OrderImbalance(Nexus::Security("TEST", 0), Nexus::Side::BID,
    //  Nexus::Quantity(80), Nexus::Money(rand() % 100),
    //  boost::posix_time::ptime({2005, 9, 1}, boost::posix_time::hours(12))));
    //m_imbalances.emplace_back(
    //  Nexus::OrderImbalance(Nexus::Security("TEST", 0), Nexus::Side::BID,
    //  Nexus::Quantity(20), Nexus::Money(rand() % 100),
    //  boost::posix_time::ptime({2005, 9, 2})));
    //m_imbalances.emplace_back(
    //  Nexus::OrderImbalance(Nexus::Security("TEST", 0), Nexus::Side::BID,
    //  Nexus::Quantity(80), Nexus::Money(rand() % 100),
    //  boost::posix_time::ptime({2005, 9, 2}, boost::posix_time::hours(12))));
    //m_imbalances.emplace_back(
    //  Nexus::OrderImbalance(Nexus::Security("TEST", 0), Nexus::Side::BID,
    //  Nexus::Quantity(20), Nexus::Money(rand() % 100),
    //  boost::posix_time::ptime({2005, 9, 3})));
    //m_imbalances.emplace_back(
    //  Nexus::OrderImbalance(Nexus::Security("TEST", 0), Nexus::Side::BID,
    //  Nexus::Quantity(80), Nexus::Money(rand() % 100),
    //  boost::posix_time::ptime({2005, 9, 3}, boost::posix_time::hours(12))));
    //m_imbalances.emplace_back(
    //  Nexus::OrderImbalance(Nexus::Security("TEST", 0), Nexus::Side::BID,
    //  Nexus::Quantity(20), Nexus::Money(rand() % 100),
    //  boost::posix_time::ptime({2005, 9, 4})));
    //m_imbalances.emplace_back(
    //  Nexus::OrderImbalance(Nexus::Security("TEST", 0), Nexus::Side::BID,
    //  Nexus::Quantity(80), Nexus::Money(rand() % 100),
    //  boost::posix_time::ptime({2005, 9, 4}, boost::posix_time::hours(12))));
  }
  for(auto& imbalance : m_imbalances) {
    if(m_interval.lower() <= imbalance.m_timestamp &&
        imbalance.m_timestamp <= m_interval.upper()) {
      m_minimum_value = min(m_minimum_value, Scalar(imbalance.m_size));
      m_maximum_value = max(m_maximum_value, Scalar(imbalance.m_size));
    }
  }
  auto chart_drawable_width = m_chart_size.width() - (2 * CHART_PADDING());
  auto lower_index = std::lower_bound(m_imbalances.begin(), m_imbalances.end(),
    m_interval.lower(), [] (const auto& item, const auto& value) {
      return item.m_timestamp < value;
    });
  auto upper_index = std::upper_bound(m_imbalances.begin(), m_imbalances.end(),
    m_interval.upper(), [] (const auto& value, const auto& item) {
      return item.m_timestamp > value;
    });
  //qDebug() << "min: " << static_cast<int>(static_cast<Nexus::Quantity>(m_minimum_value));
  //qDebug() << "max: " << static_cast<int>(static_cast<Nexus::Quantity>(m_maximum_value));
  auto data_point_count = std::distance(lower_index, upper_index);
  if(data_point_count == 1) {
    m_data_points.emplace_back(LEFT_MARGIN() + CHART_PADDING(),
      m_chart_size.height() / 2);
    return;
  }
  //qDebug() << "count: " << data_point_count;
  auto start_index = lower_index;
  for(; lower_index != upper_index; ++lower_index) {
    //qDebug() << "index: " << std::distance(m_imbalances.begin(), lower_index);
    auto index = std::distance(m_imbalances.begin(), lower_index) -
      std::distance(m_imbalances.begin(), start_index);
    auto x = LEFT_MARGIN() + CHART_PADDING() +
      static_cast<int>(static_cast<double>(chart_drawable_width) /
      static_cast<double>(data_point_count - 1) * static_cast<double>(index));
    auto y = map_to(lower_index->m_size,
      static_cast<Nexus::Quantity>(m_maximum_value),
      static_cast<Nexus::Quantity>(m_minimum_value),
      0 + scale_height(5), m_chart_size.height() - scale_height(8));
    //qDebug() << x << ", " << y;
    m_data_points.push_back({x, y});
  }
  qDebug() << "size: " << m_data_points.size();
}
