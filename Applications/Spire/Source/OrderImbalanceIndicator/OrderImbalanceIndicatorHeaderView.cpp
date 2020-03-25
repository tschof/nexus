#include "Spire/OrderImbalanceIndicator/OrderImbalanceIndicatorHeaderView.hpp"
#include "Spire/Spire/Dimensions.hpp"

using namespace boost::signals2;
using namespace Spire;

OrderImbalanceIndicatorHeaderView::OrderImbalanceIndicatorHeaderView(
    QWidget* parent)
    : QHeaderView(Qt::Horizontal, parent) {
  setSectionsMovable(true);
  setFirstSectionMovable(false);
  setSectionsClickable(true);
  setMinimumSectionSize(scale_width(70));
  setDefaultAlignment(Qt::AlignLeft | Qt::AlignTop);
  setStyleSheet(QString(R"(
    QHeaderView::section {
      background-color: #FFFFFF;
      background-image: url(:/Icons/column-border.png);
      background-position: left;
      background-repeat: repeat;
      border: none;
      color: #4B23A0;
      font-family: Roboto;
      font-weight: 550;
      padding-left: %1px;
      padding-right: %1px;
    }

    QHeaderView::section::first {
      background: none;
      background-color: #FFFFFF;
    })").arg(scale_width(8)));
}

connection OrderImbalanceIndicatorHeaderView::connect_security_filter_signal(
    SecurityFilterSignal::slot_type& slot) const {
  return m_security_signal.connect(slot);
}

connection OrderImbalanceIndicatorHeaderView::connect_side_filter_signal(
    SideFilterSignal::slot_type& slot) const {
  return m_side_signal.connect(slot);
}

connection OrderImbalanceIndicatorHeaderView::connect_size_filter_signal(
    ScalarFilterSignal::slot_type& slot) const {
  return m_size_signal.connect(slot);
}

connection
    OrderImbalanceIndicatorHeaderView::connect_reference_price_filter_signal(
    ScalarFilterSignal::slot_type& slot) const {
  return m_reference_price_signal.connect(slot);
}

connection OrderImbalanceIndicatorHeaderView::connect_date_filter_signal(
    ScalarFilterSignal::slot_type& slot) const {
  return m_date_signal.connect(slot);
}

connection OrderImbalanceIndicatorHeaderView::connect_time_filter_signal(
    ScalarFilterSignal::slot_type& slot) const {
  return m_time_signal.connect(slot);
}

connection
    OrderImbalanceIndicatorHeaderView::connect_notional_value_filter_signal(
    ScalarFilterSignal::slot_type& slot) const {
  return m_notional_value_signal.connect(slot);
}
