#include "Spire/Ui/TimeInputWidget.hpp"
#include <QEvent>
#include <QHBoxLayout>
#include <QRegularExpressionValidator>
#include "Spire/Spire/Dimensions.hpp"

using namespace boost::posix_time;
using namespace boost::signals2;
using namespace Spire;

TimeInputWidget::TimeInputWidget(TimeFormat format, QWidget* parent)
    : QWidget(parent),
      m_time_format(format) {
  auto layout = new QHBoxLayout(this);
  layout->setSpacing(0);
  layout->setContentsMargins({});
  add_input("00", Qt::AlignRight, QRegularExpression("^\\d\\d$"), 0, 99);
  add_colon_widget();
  add_input("00", Qt::AlignCenter, QRegularExpression("^\\d\\d$"), 0, 59);
  if(m_time_format == TimeFormat::HMS) {
    add_colon_widget();
    add_input("00", Qt::AlignLeft, QRegularExpression("^\\d\\d$"), 0, 59);
  }
  resize_inputs();
  set_unfocused_style();
}

bool TimeInputWidget::eventFilter(QObject* watched, QEvent* event) {
  if(event->type() == QEvent::FocusIn) {
    set_focused_style();
  } else if(event->type() == QEvent::FocusOut) {
    set_unfocused_style();
  }
  return QWidget::eventFilter(watched, event);
}

connection TimeInputWidget::connect_time_signal(
    const TimeSignal::slot_type& slot) const {
  return m_time_signal.connect(slot);
}

void TimeInputWidget::set_time(const time_duration& time) {
  //m_hour_input->setText(clamped_value(QString::number(time.hours()), 0, 99));
  //m_minute_input->setText(clamped_value(QString::number(time.minutes()), 0,
  //  59));
  //m_last_valid_hour = m_hour_input->text().toInt();
  //m_last_valid_minute = m_minute_input->text().toInt();
  //if(m_time_format == TimeFormat::HMS) {
  //  m_second_input->setText(clamped_value(QString::number(time.seconds()), 0,
  //    59));
  //  m_last_valid_second = m_second_input->text().toInt();
  //}
}

void TimeInputWidget::add_colon_widget() {
  auto colon_widget = new ColonWidget(this);
  colon_widget->setFixedSize(scale(3, 26));
  layout()->addWidget(colon_widget);
  m_colon_widgets.push_back(colon_widget);
}

void TimeInputWidget::add_input(const QString& text,
    Qt::AlignmentFlag alignment, const QRegularExpression& regex,
    int min_value, int max_value) {
  auto input = new QLineEdit(text, this);
  input->setAlignment(alignment);
  input->installEventFilter(this);
  layout()->addWidget(input);
  m_inputs.emplace_back(TimeInput{input, 0, min_value, max_value});
}

void TimeInputWidget::apply_border(QLineEdit* input,
    const QString& css_selector, const QColor& color) {
  input->setStyleSheet(input->styleSheet() + QString(R"(
      QLineEdit {
        %3: %1px solid %2;
      }
    )").arg(scale_width(1)).arg(color.name()).arg(css_selector));
}

void TimeInputWidget::apply_style(QLineEdit* input, const QColor& color) {
  input->setStyleSheet(QString(R"(
    QLineEdit {
      border-bottom: %1px solid %2;
      border-left: none;
      border-right: none;
      border-top: %1px solid %2;
      font-family: Roboto;
      font-size: %3px;
    })").arg(scale_height(1)).arg(color.name()).arg(scale_height(12)));
}

QString TimeInputWidget::clamped_value(const QString& text, int min_value,
    int max_value) {
  return clamped_value(text, min_value, max_value, 0);
}

QString TimeInputWidget::clamped_value(const QString& text, int min_value,
    int max_value, int addend) {
  auto ok = false;
  auto value = text.toInt(&ok);
  if(ok) {
    value += addend;
    value = std::min(max_value, std::max(min_value, value));
    if(value < 10) {
      return QString("0" + QString::number(value));
    }
    return QString::number(value);
  }
  return QString::number(min_value);
}

QString TimeInputWidget::get_input_value(const QString& text, int key,
    int min_value, int max_value) {
  if(key == Qt::Key_Up) {
    return clamped_value(text, min_value, max_value, 1);
  }
  return clamped_value(text, min_value, max_value, -1);
}

void TimeInputWidget::resize_inputs() {
  if(m_time_format == TimeFormat::HM) {
    setFixedSize(scale(50, 26));
    m_inputs[0].m_input->setFixedSize(scale(23, 26));
    m_inputs[1].m_input->setFixedSize(scale(24, 26));
    return;
  }
  setFixedSize(scale(88, 26));
  m_inputs[0].m_input->setFixedSize(scale(31, 26));
  m_inputs[1].m_input->setFixedSize(scale(20, 26));
  m_inputs[2].m_input->setFixedSize(scale(31, 26));
}

void TimeInputWidget::set_focused_style() {
  set_style("#4B23A0");
  for(auto* colon_widget : m_colon_widgets) {
    colon_widget->set_active_style();
  }
}

void TimeInputWidget::set_style(const QColor& color) {
  for(auto input : m_inputs) {
    apply_style(input.m_input, color);
  }
  apply_border(static_cast<QLineEdit*>(layout()->itemAt(0)->widget()),
    "border-left", color);
  apply_border(static_cast<QLineEdit*>(
    layout()->itemAt(layout()->count() - 1)->widget()), "border-right", color);
}

void TimeInputWidget::set_unfocused_style() {
  set_style("#C8C8C8");
  for(auto* colon_widget : m_colon_widgets) {
    colon_widget->set_default_style();
  }
}

void TimeInputWidget::on_time_changed() {
  //auto hour_ok = false;
  //auto minute_ok = false;
  //auto hour = m_hour_input->text().toInt(&hour_ok);
  //auto minute = m_minute_input->text().toInt(&minute_ok);
  //if(hour_ok && minute_ok) {
  //  m_last_valid_hour = hour;
  //  m_last_valid_minute = minute;
  //  if(m_time_format == TimeFormat::HM) {
  //    m_time_signal(hours(hour) + minutes(minute));
  //    return;
  //  }
  //  auto second_ok = false;
  //  auto second = m_second_input->text().toInt(&second_ok);
  //  if(second_ok) {
  //    m_last_valid_second = second;
  //    m_time_signal(hours(hour) + minutes(minute) + seconds(second));
  //  }
  //}
}
