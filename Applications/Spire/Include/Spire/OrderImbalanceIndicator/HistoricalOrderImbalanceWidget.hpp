#ifndef SPIRE_HISTORICAL_ORDER_IMBALANCE_WIDGET_HPP
#define SPIRE_HISTORICAL_ORDER_IMBALANCE_WIDGET_HPP
#include <QWidget>
#include "Nexus/Definitions/OrderImbalance.hpp"
#include "Spire/OrderImbalanceIndicator/HistoricalOrderImbalanceChartView.hpp"
#include "Spire/OrderImbalanceIndicator/OrderImbalanceIndicator.hpp"
#include "Spire/Spire/Intervals.hpp"
#include "Spire/Ui/Ui.hpp"

namespace Spire {

  //! Represents a widget for displaying a historical order imbalance chart
  //! with a dropdown for selecting dislayed values.
  class HistoricalOrderImbalanceWidget : public QWidget {
    public:

      //! Constructs a HistoricalOrderImbalanceWidget with an initial time
      //! interval and a source imbalance model.
      /*
        \param interval The initial time interval the chart will display.
        \param model The source order imbalance model.
        \param parent The parent widget.
      */
      HistoricalOrderImbalanceWidget(const TimeInterval& interval,
        std::shared_ptr<OrderImbalanceIndicatorModel> model,
        QWidget* parent = nullptr);

    private:
      DropDownMenu* m_data_dropdown;
      HistoricalOrderImbalanceChartView* m_chart_widget;
      std::unordered_map<QString,
        HistoricalOrderImbalanceChartView::DisplayType> m_display_options;

      void on_dropdown_changed(const QString& text);
  };
}

#endif
