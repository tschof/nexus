#include "spire/ui/flat_button.hpp"
#include <QHBoxLayout>
#include <QMouseEvent>

using namespace boost;
using namespace boost::signals2;
using namespace spire;

flat_button::flat_button(const QString& label, QWidget* parent)
    : QWidget(parent) {
  m_label = new QLabel(label, this);
  auto layout = new QHBoxLayout(this);
  layout->setMargin(0);
  layout->addWidget(m_label);
}

connection flat_button::connect_clicked_signal(
    const clicked_signal::slot_type& slot) const {
  return m_clicked_signal.connect(slot);
}

void flat_button::changeEvent(QEvent* event) {
  if(event->type() == QEvent::FontChange) {
    m_label->setFont(font());
  }
}

void flat_button::mouseReleaseEvent(QMouseEvent* event) {
  if(event->button() == Qt::LeftButton) {
    m_clicked_signal();
  }
}