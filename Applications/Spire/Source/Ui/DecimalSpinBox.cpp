#include "Spire/Ui/DecimalSpinBox.hpp"

using namespace boost::signals2;
using namespace Spire;

DecimalSpinBox::DecimalSpinBox(double value, QWidget* parent)
    : QAbstractSpinBox(parent) {
  m_spin_box = new RealSpinBox(static_cast<long double>(value),
    this);
  setFocusProxy(m_spin_box);
  m_spin_box->connect_change_signal([=] (auto value) {
    m_change_signal(value.extract_double());
  });
  connect(m_spin_box, &RealSpinBox::editingFinished, this,
    &DecimalSpinBox::on_editing_finished);
}

void DecimalSpinBox::resizeEvent(QResizeEvent* event) {
  m_spin_box->resize(size());
  QWidget::resizeEvent(event);
}

connection DecimalSpinBox::connect_change_signal(
    const ValueSignal::slot_type& slot) const {
  return m_change_signal.connect(slot);
}

void DecimalSpinBox::set_minimum(double minimum) {
  m_spin_box->set_minimum(static_cast<long double>(minimum));
}

void DecimalSpinBox::set_maximum(double maximum) {
  m_spin_box->set_maximum(static_cast<long double>(maximum));
}

double DecimalSpinBox::get_value() const {
  return m_spin_box->get_value().extract_double();
}

void DecimalSpinBox::set_value(double value) {
  m_spin_box->set_value(static_cast<long double>(value));
}

void DecimalSpinBox::on_editing_finished() {
  Q_EMIT editingFinished();
}
