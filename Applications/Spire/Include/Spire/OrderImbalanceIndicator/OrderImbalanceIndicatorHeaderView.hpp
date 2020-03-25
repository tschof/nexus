#ifndef SPIRE_ORDER_IMBALANCE_INDICATOR_HEADER_VIEW_HPP
#define SPIRE_ORDER_IMBALANCE_INDICATOR_HEADER_VIEW_HPP
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/time_duration.hpp>
#include <QHeaderView>
#include "Nexus/Definitions/Side.hpp"
#include "Spire/OrderImbalanceIndicator/OrderImbalanceIndicator.hpp"

namespace Spire {

  class OrderImbalanceIndicatorHeaderView : public QHeaderView {
    public:

      using SecurityFilterSignal = Signal<void (const QString& text)>;

      using SideFilterSignal = Signal<void (Nexus::Side side)>;

      using ScalarFilterSignal = Signal<void (Scalar min, Scalar max)>;

      explicit OrderImbalanceIndicatorHeaderView(QWidget* parent = nullptr);

      boost::signals2::connection connect_security_filter_signal(
        SecurityFilterSignal::slot_type& slot) const;

      boost::signals2::connection connect_side_filter_signal(
        SideFilterSignal::slot_type& slot) const;

      boost::signals2::connection connect_size_filter_signal(
        ScalarFilterSignal::slot_type& slot) const;

      boost::signals2::connection connect_reference_price_filter_signal(
        ScalarFilterSignal::slot_type& slot) const;

      boost::signals2::connection connect_date_filter_signal(
        ScalarFilterSignal::slot_type& slot) const;

      boost::signals2::connection connect_time_filter_signal(
        ScalarFilterSignal::slot_type& slot) const;

      boost::signals2::connection connect_notional_value_filter_signal(
        ScalarFilterSignal::slot_type& slot) const;

    private:
      mutable SecurityFilterSignal m_security_signal;
      mutable SideFilterSignal m_side_signal;
      mutable ScalarFilterSignal m_size_signal;
      mutable ScalarFilterSignal m_reference_price_signal;
      mutable ScalarFilterSignal m_date_signal;
      mutable ScalarFilterSignal m_time_signal;
      mutable ScalarFilterSignal m_notional_value_signal;
  };
}

#endif
