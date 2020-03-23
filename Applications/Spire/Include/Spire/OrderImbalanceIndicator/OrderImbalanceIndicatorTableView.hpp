#ifndef SPIRE_ORDER_IMBALANCE_INDICATOR_TABLE_VIEW_HPP
#define SPIRE_ORDER_IMBALANCE_INDICATOR_TABLE_VIEW_HPP
#include <QHeaderView>
#include <QTableView>
#include <QTimer>
#include <QVBoxLayout>
#include "Spire/Ui/Ui.hpp"

namespace Spire {

  class OrderImbalanceIndicatorTableView : public QWidget {
    public:

      OrderImbalanceIndicatorTableView(QWidget* parent = nullptr);

    private:
      QVBoxLayout* m_layout;
      QHeaderView* m_header;
      QTableView* m_table;
      QTimer m_h_scroll_bar_timer;
      QTimer m_v_scroll_bar_timer;
      std::unique_ptr<TransitionWidget> m_transition_widget;
  };
}

#endif
