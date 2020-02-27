#ifndef SPIRE_HISTORICAL_ORDER_IMBALANCE_CHART_VIEW_HPP
#define SPIRE_HISTORICAL_ORDER_IMBALANCE_CHART_VIEW_HPP
#include <QPen>
#include <QWidget>
#include "Nexus/Definitions/OrderImbalance.hpp"
#include "Spire/OrderImbalanceIndicator/OrderImbalanceIndicator.hpp"
#include "Spire/Spire/Intervals.hpp"
#include "Spire/Spire/QtPromise.hpp"
#include "Spire/Spire/Scalar.hpp"

namespace Spire {

  class HistoricalOrderImbalanceChartView : public QWidget {
    public:

      enum class DISPLAY_TYPE {
        REFERENCE_PRICE,
        SIZE,
        NOTIONAL_VALUE
      };

      HistoricalOrderImbalanceChartView(
        const TimeInterval& interval,
        std::shared_ptr<OrderImbalanceIndicatorModel> model,
        QWidget* parent = nullptr);

      void set_display_type(DISPLAY_TYPE type);

    protected:
      void leaveEvent(QEvent* event) override;
      void mouseMoveEvent(QMouseEvent* event) override;
      void paintEvent(QPaintEvent* event) override;
      void resizeEvent(QResizeEvent* event) override;
      void showEvent(QShowEvent* event) override {
        on_data_loaded({});
      }

    private:
      std::shared_ptr<OrderImbalanceIndicatorModel> m_model;
      std::vector<Nexus::OrderImbalance> m_imbalances;
      std::vector<QPoint> m_data_points;
      TimeInterval m_interval;
      Scalar m_minimum_value;
      Scalar m_maximum_value;
      boost::optional<QPoint> m_crosshair_pos;
      QSize m_chart_size;
      QPen m_dashed_line_pen;
      QtPromise<std::vector<Nexus::OrderImbalance>> m_load_promise;

      void on_data_loaded(const std::vector<Nexus::OrderImbalance>& data);
  };
}

#endif
