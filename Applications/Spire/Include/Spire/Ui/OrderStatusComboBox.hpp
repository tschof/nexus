#ifndef SPIRE_ORDER_STATUS_COMBO_BOX_HPP
#define SPIRE_ORDER_STATUS_COMBO_BOX_HPP
#include <QWidget>
#include "Nexus/Definitions/OrderStatus.hpp"
#include "Spire/Spire/Spire.hpp"

namespace Spire {

  //! Represents a combo box for selecting OrderStatus values.
  class OrderStatusComboBox : public QWidget {
    public:

      //! Signals that an OrderStatus was selected.
      /*!
        \param status The selected OrderStatus.
      */
      using SelectedSignal = Signal<void (Nexus::OrderStatus status)>;

      //! Constructs a OrderStatusComboBox.
      /*!
        \param parent The parent widget.
      */
      explicit OrderStatusComboBox(QWidget* parent = nullptr);

      //! Connects a slot to the OrderStatus selection signal.
      boost::signals2::connection connect_selected_signal(
        const SelectedSignal::slot_type& slot) const;

    private:
      mutable SelectedSignal m_selected_signal;
      boost::signals2::scoped_connection m_value_connection;
  };
}

#endif