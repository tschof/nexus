#ifndef SPIRE_HISTORICAL_ORDER_IMBALANCE_CHART_VIEW_HPP
#define SPIRE_HISTORICAL_ORDER_IMBALANCE_CHART_VIEW_HPP
#include <QPen>
#include <QWidget>
#include "Nexus/Definitions/OrderImbalance.hpp"
#include "Spire/OrderImbalanceIndicator/OrderImbalanceIndicator.hpp"

namespace Spire {

  class HistoricalOrderImbalanceChartView : public QWidget {
    public:

      HistoricalOrderImbalanceChartView(
        std::shared_ptr<OrderImbalanceIndicatorModel> model,
        QWidget* parent = nullptr);

    protected:
      void leaveEvent(QEvent* event) override;
      void mouseMoveEvent(QMouseEvent* event) override;
      void paintEvent(QPaintEvent* event) override;
      void resizeEvent(QResizeEvent* event) override;

    private:
      std::shared_ptr<OrderImbalanceIndicatorModel> m_model;
      std::vector<Nexus::OrderImbalance> m_imbalances;
      boost::optional<QPoint> m_crosshair_pos;
      QSize m_chart_size;
      QPen m_dashed_line_pen;
  };
}

#endif
