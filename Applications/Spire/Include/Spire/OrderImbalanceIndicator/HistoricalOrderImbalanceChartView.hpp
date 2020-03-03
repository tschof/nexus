#ifndef SPIRE_HISTORICAL_ORDER_IMBALANCE_CHART_VIEW_HPP
#define SPIRE_HISTORICAL_ORDER_IMBALANCE_CHART_VIEW_HPP
#include <QPen>
#include <QWidget>
#include "Nexus/Definitions/OrderImbalance.hpp"
#include "Spire/OrderImbalanceIndicator/OrderImbalanceIndicator.hpp"
#include "Spire/Spire/Intervals.hpp"
#include "Spire/Spire/Scalar.hpp"
#include "Spire/Ui/CustomQtVariants.hpp"

namespace Spire {

  class HistoricalOrderImbalanceChartView : public QWidget {
    public:

      enum class DisplayType {
        REFERENCE_PRICE,
        SIZE,
        NOTIONAL_VALUE
      };

      HistoricalOrderImbalanceChartView(
        const TimeInterval& interval,
        const std::vector<Nexus::OrderImbalance>& imbalances,
        QWidget* parent = nullptr);

      void set_display_type(DisplayType type);

    protected:
      void leaveEvent(QEvent* event) override;
      void mouseMoveEvent(QMouseEvent* event) override;
      void mousePressEvent(QMouseEvent* event) override;
      void mouseReleaseEvent(QMouseEvent* event) override;
      void paintEvent(QPaintEvent* event) override;
      void resizeEvent(QResizeEvent* event) override;
      void wheelEvent(QWheelEvent* event) override;

    private:
      struct ChartPoint {
        QPoint m_point;
        const Nexus::OrderImbalance& m_imbalance;

        ChartPoint(const QPoint& point,
          Nexus::OrderImbalance& imbalance)
          : m_point(point),
            m_imbalance(imbalance) {}
      };

      std::vector<Nexus::OrderImbalance> m_imbalances;
      std::vector<ChartPoint> m_chart_points;
      TimeInterval m_interval;
      Scalar m_minimum_value;
      Scalar m_maximum_value;
      boost::optional<QPoint> m_crosshair_pos;
      QFont m_label_font;
      QFontMetrics m_font_metrics;
      bool m_is_dragging;
      QPoint m_last_mouse_pos;
      QSize m_chart_size;
      QPen m_dashed_line_pen;
      CustomVariantItemDelegate* m_item_delegate;
      DisplayType m_display_type;

      void draw_line(QPainter& painter, const QPoint& point1,
        const QPoint& point2);
      void draw_point(QPainter& painter, const QPoint& point);
      QString scalar_to_string(Scalar value) const;
      void update_points();
  };
}

#endif
