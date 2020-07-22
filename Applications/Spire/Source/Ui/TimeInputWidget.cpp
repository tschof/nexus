#include "Spire/Ui/TimeInputWidget.hpp"
#include <QHBoxLayout>
#include <QRegularExpressionValidator>
#include "Spire/Spire/Dimensions.hpp"

using namespace boost::posix_time;
using namespace boost::signals2;
using namespace Spire;

TimeInputWidget::TimeInputWidget(
    const std::vector<QRegularExpression>& sections, QWidget* parent)
    : QWidget(parent) {
  auto layout = new QHBoxLayout(this);
  layout->setSpacing(0);
  layout->setContentsMargins({});
  std::for_each(sections.begin(), sections.end(), [&] (const auto& section) {
    auto section_input = new QLineEdit("00", this);
    section_input->setValidator(new RegularExpressionValidator(section, this));
    section_input->setAlignment(Qt::AlignLeft);
    section_input->setFixedSize(scale(23, 26));
    section_input->installEventFilter(this);
    layout->addWidget(section_input);
    m_time_inputs->push_back(section_input);
    if(section != sections.back()) {
      auto colon_widget = new ColonWidget(this);
      colon_widget->setFixedSize(scale(3, 26));
      layout->addWidget(colon_widget);
      m_colon_widgets->push_back(colon_widget);
    }
  });
  set_unfocused_style();
}

connection TimeInputWidget::connect_time_signal(
    const TimeSignal::slot_type& slot) const {
  return m_time_signal.connect(slot);
}

void TimeInputWidget::set_time(const time_duration& time) {
  
}

void TimeInputWidget::set_style(const QColor& color) {

}

void TimeInputWidget::set_focused_style() {
  set_style("#4B23A0");
  for(auto* colon_widget : m_colon_widgets) {
    colon_widget->set_active_style();
  }
}

void TimeInputWidget::set_unfocused_style() {
  set_style("#C8C8C8");
  for(auto* colon_widget : m_colon_widgets) {
    colon_widget->set_default_style();
  }
}
