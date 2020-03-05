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

  //! Displays a chart of historical order imbalances.
  class HistoricalOrderImbalanceChartView : public QWidget {
    public:

      //! Indicates which order imbalance value to display.
      enum class DisplayType {

        //! Option to display the order imbalances' reference price.
        REFERENCE_PRICE,

        //! Option to display the order imbalances' size.
        SIZE,

        //! Optionto dislay the order imbalances' notional value.
        NOTIONAL_VALUE
      };

      //! Constructs a HistoricalOrderImbalanceChartView with an initial
      //! time interval and source model.
      /*
        \param interval The initial time interval the chart will display.
        \param model The source order imbalance model.
        \param parent The parent widget.
      */
      HistoricalOrderImbalanceChartView(
        const TimeInterval& interval,
        std::shared_ptr<OrderImbalanceIndicatorModel> model,
        QWidget* parent = nullptr);

      //! Sets the chart's DisplayType.
      /*
        \param type The value type to display in the chart.
      */
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
        Nexus::OrderImbalance* m_imbalance;
      };

      std::vector<Nexus::OrderImbalance> m_imbalances;
      std::vector<ChartPoint> m_chart_points;
      TimeInterval m_interval;
      Scalar m_minimum_value;
      Scalar m_maximum_value;
      boost::optional<QPoint> m_cursor_pos;
      ChartPoint m_crosshair_point;
      QFont m_label_font;
      QFontMetrics m_font_metrics;
      bool m_is_dragging;
      QPoint m_last_mouse_pos;
      QSize m_chart_size;
      QPen m_dashed_line_pen;
      CustomVariantItemDelegate* m_item_delegate;
      DisplayType m_display_type;
      QPolygon m_gradient_cover;

      void draw_gradient_cover(QPainter& painter) const;
      void draw_hover_widgets(QPainter& painter);
      void draw_line(QPainter& painter, const QPoint& point1,
        const QPoint& point2);
      void draw_point(QPainter& painter, const QPoint& point);
      void draw_x_axis_label(QPainter& painter, int pos_x,
        const boost::posix_time::ptime& timestamp);
      void draw_x_axis_label(QPainter& painter, int pos_x,
        const boost::posix_time::ptime& timestamp,
        const QColor& background_color, const QColor& text_color);
      void draw_y_axis_label(QPainter& painter, int pos_y,
        const QString& text);
      Scalar get_scalar(const Nexus::OrderImbalance& imbalance) const;
      QVariant get_value(Scalar value) const;
      QVariant get_value(const Nexus::OrderImbalance& imbalance) const;
      int left_margin() const;
      QString to_string(Scalar value) const;
      QString to_string(const Nexus::OrderImbalance& imbalance) const;
      void update_points();
  };
}

#endif
