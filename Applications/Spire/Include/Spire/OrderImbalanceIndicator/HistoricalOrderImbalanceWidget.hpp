#ifndef SPIRE_HISTORICAL_ORDER_IMBALANCE_WIDGET_HPP
#define SPIRE_HISTORICAL_ORDER_IMBALANCE_WIDGET_HPP
#include <QWidget>
#include "Spire/Ui/Ui.hpp"

namespace Spire {

  class HistoricalOrderImbalanceWidget : public QWidget {
    public:

      HistoricalOrderImbalanceWidget(QWidget* parent = nullptr);

    private:
      DropDownMenu* m_data_dropdown;
      QWidget* m_chart_widget;
  };
}

#endif
