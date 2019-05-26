#include "spire/charting/charting_window.hpp"
#include <climits>
#include <QApplication>
#include <QHBoxLayout>
#include <QIcon>
#include <QIntValidator>
#include <QKeyEvent>
#include <QListView>
#include <QVBoxLayout>
#include "spire/charting/chart_view.hpp"
#include "spire/charting/charting_technicals_panel.hpp"
#include "spire/security_input/security_input_dialog.hpp"
#include "spire/security_input/security_input_model.hpp"
#include "spire/spire/dimensions.hpp"
#include "spire/spire/local_technicals_model.hpp"
#include "spire/ui/custom_qt_variants.hpp"
#include "spire/ui/dropdown_menu.hpp"
#include "spire/ui/security_widget.hpp"
#include "spire/ui/toggle_button.hpp"
#include "spire/ui/window.hpp"

using namespace boost;
using namespace boost::signals2;
using namespace Beam;
using namespace Nexus;
using namespace Spire;

namespace {
  const auto ZOOM_FACTOR = 1.1;
}

ChartingWindow::ChartingWindow(Ref<SecurityInputModel> input_model,
    QWidget* parent)
    : Window(parent),
      m_is_mouse_dragging(false),
      m_security_widget_container(nullptr),
      m_technicals_panel(nullptr),
      m_chart(nullptr),
      m_is_chart_auto_scaled(true) {
  setMinimumSize(scale(400, 320));
  resize_body(scale(400, 320));
  set_svg_icon(":/icons/chart-black.svg",
    ":/icons/chart-grey.svg");
  setWindowIcon(QIcon(":/icons/chart-icon-256x256.png"));
  setWindowTitle(tr("Chart"));
  auto body = new QWidget(this);
  body->setStyleSheet("background-color: #FFFFFF;");
  auto layout = new QVBoxLayout(body);
  layout->setContentsMargins({});
  layout->setSpacing(0);
  m_button_header_widget = new QWidget(body);
  m_button_header_widget->setFixedHeight(scale_height(46));
  m_button_header_widget->setStyleSheet("background-color: #F5F5F5;");
  auto button_header_layout = new QHBoxLayout(m_button_header_widget);
  button_header_layout->setSpacing(0);
  button_header_layout->setContentsMargins(0, scale_height(10), 0,
    scale_height(10));
  button_header_layout->addSpacing(scale_width(8));
  m_period_line_edit = new QLineEdit(m_button_header_widget);
  connect(m_period_line_edit, &QLineEdit::editingFinished, this,
    &ChartingWindow::on_period_line_edit_changed);
  m_period_line_edit->setFixedSize(scale(36, 26));
  m_period_line_edit->setValidator(new QIntValidator(1, INT_MAX,
    m_period_line_edit));
  m_period_line_edit->setText("1");
  m_period_line_edit->setAlignment(Qt::AlignCenter);
  m_period_line_edit->setStyleSheet(QString(R"(
    QLineEdit {
      background-color: #FFFFFF;
      border: %1px solid #C8C8C8 %2px solid #C8C8C8;
      color: #000000;
      font-family: Roboto;
      font-size: %3px;
    }

    QLineEdit:focus {
      border: %1px solid #4B23A0 %2px solid #4B23A0;
    })").arg(scale_height(1)).arg(scale_width(1)).arg(scale_height(12)));
  button_header_layout->addWidget(m_period_line_edit);
  button_header_layout->addSpacing(scale_width(4));
  m_period_dropdown = new DropdownMenu(
    {tr("second"), tr("minute"), tr("hour")}, m_button_header_widget);
  m_period_dropdown->setFixedSize(scale(80, 26));
  button_header_layout->addWidget(m_period_dropdown);
  button_header_layout->addSpacing(scale_width(18));
  auto button_image_size = scale(16, 16);
  auto lock_grid_button = new ToggleButton(
    imageFromSvg(":/icons/lock-grid-purple.svg", button_image_size),
    imageFromSvg(":/icons/lock-grid-green.svg", button_image_size),
    imageFromSvg(":/icons/lock-grid-purple.svg", button_image_size),
    imageFromSvg(":/icons/lock-grid-grey.svg", button_image_size),
    m_button_header_widget);
  lock_grid_button->setFixedSize(scale(26, 26));
  lock_grid_button->setToolTip(tr("Lock Grid"));
  button_header_layout->addWidget(lock_grid_button);
  button_header_layout->addSpacing(scale_width(10));
  m_auto_scale_button = new ToggleButton(
    imageFromSvg(":/icons/auto-scale-purple.svg", button_image_size),
    imageFromSvg(":/icons/auto-scale-green.svg", button_image_size),
    imageFromSvg(":/icons/auto-scale-purple.svg", button_image_size),
    imageFromSvg(":/icons/auto-scale-grey.svg", button_image_size),
    m_button_header_widget);
  m_auto_scale_button->setFixedSize(scale(26, 26));
  m_auto_scale_button->setToolTip(tr("Auto Scale"));
  m_auto_scale_button->set_toggled(true);
  m_auto_scale_button->connect_clicked_signal([=] {
    on_auto_scale_button_click();
  });
  button_header_layout->addWidget(m_auto_scale_button);
  button_header_layout->addSpacing(scale_width(10));
  auto seperator = new QWidget(m_button_header_widget);
  seperator->setFixedSize(scale(1, 16));
  seperator->setStyleSheet("background-color: #D0D0D0;");
  button_header_layout->addWidget(seperator);
  button_header_layout->addSpacing(scale_width(10));
  auto draw_line_button = new ToggleButton(
    imageFromSvg(":/icons/draw-purple.svg", button_image_size),
    imageFromSvg(":/icons/draw-green.svg", button_image_size),
    imageFromSvg(":/icons/draw-purple.svg", button_image_size),
    imageFromSvg(":/icons/draw-grey.svg", button_image_size),
    m_button_header_widget);
  draw_line_button->setFixedSize(scale(26, 26));
  draw_line_button->setToolTip(tr("Draw Line"));
  button_header_layout->addWidget(draw_line_button);
  button_header_layout->addStretch(1);
  layout->addWidget(m_button_header_widget);
  m_security_widget = new SecurityWidget(input_model,
    SecurityWidget::Theme::DARK, this);
  m_security_widget->connect_change_security_signal(
      [=] (const auto& security) {
    on_security_change(security);
  });
  layout->addWidget(m_security_widget);
  setTabOrder(m_period_line_edit, m_period_dropdown);
  setTabOrder(m_period_dropdown, lock_grid_button);
  setTabOrder(lock_grid_button, m_auto_scale_button);
  setTabOrder(m_auto_scale_button, draw_line_button);
  setTabOrder(draw_line_button, m_period_line_edit);
  m_security_widget->setFocus();
  Window::layout()->addWidget(body);
}

void ChartingWindow::set_models(std::shared_ptr<ChartModel> chart_model,
    std::shared_ptr<TechnicalsModel> technicals_model) {
  m_model = std::move(chart_model);
  m_technicals_model = std::move(technicals_model);
  delete m_technicals_panel;
  delete m_chart;
  delete m_security_widget_container;
  m_security_widget_container = new QWidget(this);
  auto container_layout = new QVBoxLayout(m_security_widget_container);
  container_layout->setContentsMargins({});
  m_technicals_panel = new ChartingTechnicalsPanel(*m_technicals_model);
  container_layout->addWidget(m_technicals_panel);
  m_chart = new ChartView(*m_model, m_security_widget_container);
  m_chart->set_auto_scale(m_is_chart_auto_scaled);
  container_layout->addWidget(m_chart);
  m_security_widget->set_widget(m_security_widget_container);
  m_chart->installEventFilter(this);
}

connection ChartingWindow::connect_security_change_signal(
    const ChangeSecuritySignal::slot_type& slot) const {
  return m_security_widget->connect_change_security_signal(slot);
}

bool ChartingWindow::eventFilter(QObject* object, QEvent* event) {
  if(object == m_chart) {
    if(event->type() == QEvent::MouseMove) {
      auto e = static_cast<QMouseEvent*>(event);
      if(m_is_mouse_dragging) {
        auto chart_delta = m_chart->convert_pixels_to_chart(e->pos());
        auto last_pos = m_chart->convert_pixels_to_chart(
          m_last_chart_mouse_pos);
        auto [top_left, bottom_right] = m_chart->get_region();
        top_left.m_x -= chart_delta.m_x - last_pos.m_x;
        top_left.m_y -= chart_delta.m_y - last_pos.m_y;
        bottom_right.m_x -= chart_delta.m_x - last_pos.m_x;
        bottom_right.m_y -= chart_delta.m_y - last_pos.m_y;
        m_chart->set_region(top_left, bottom_right);
        m_last_chart_mouse_pos = e->pos();
      }
      m_chart->set_crosshair(e->pos());
    } else if(event->type() == QEvent::MouseButtonPress) {
      auto e = static_cast<QMouseEvent*>(event);
      if(!m_is_mouse_dragging && e->button() == Qt::LeftButton) {
        m_is_mouse_dragging = true;
        m_last_chart_mouse_pos = e->pos();
      }
    } else if(event->type() == QEvent::MouseButtonRelease) {
      auto e = static_cast<QMouseEvent*>(event);
      if(e->button() == Qt::LeftButton) {
        m_is_mouse_dragging = false;
      }
    } else if(event->type() == QEvent::Wheel) {
      auto e = static_cast<QWheelEvent*>(event);
      auto [top_left, bottom_right] = m_chart->get_region();
      auto old_height = top_left.m_y - bottom_right.m_y;
      auto old_width = top_left.m_x - bottom_right.m_x;
      auto [new_width, new_height] = [&] {
        if(e->angleDelta().y() < 0) {
          return std::make_tuple(ZOOM_FACTOR * old_width,
            ZOOM_FACTOR * old_height);
        }
        return std::make_tuple(old_width / ZOOM_FACTOR,
          old_height / ZOOM_FACTOR);
      }();
      auto width_change = (new_width - old_width) / 2;
      auto height_change = (new_height - old_height) / 2;
      m_chart->set_region({top_left.m_x + width_change,
        top_left.m_y + height_change},
        {bottom_right.m_x - width_change, bottom_right.m_y - height_change});
    } else if(event->type() == QEvent::HoverLeave) {
      m_chart->reset_crosshair();
    } else if(event->type() == QEvent::HoverEnter) {
      auto e = static_cast<QHoverEvent*>(event);
      m_chart->set_crosshair(e->pos());
    }
  }
  return QWidget::eventFilter(object, event);
}

void ChartingWindow::keyPressEvent(QKeyEvent* event) {
  QApplication::sendEvent(m_security_widget, event);
}

void ChartingWindow::on_auto_scale_button_click() {
  m_is_chart_auto_scaled = !m_is_chart_auto_scaled;
  if(m_chart != nullptr) {
    m_chart->set_auto_scale(m_is_chart_auto_scaled);
  }
}

void ChartingWindow::on_period_line_edit_changed() {
  if(m_period_line_edit->text().toInt() == 1) {
    m_period_dropdown->set_items({tr("second"), tr("minute"), tr("hour")});
  } else {
    m_period_dropdown->set_items({tr("seconds"), tr("minutes"), tr("hours")});
  }
}

void ChartingWindow::on_security_change(const Security& security) {
  setWindowTitle(CustomVariantItemDelegate().displayText(
    QVariant::fromValue(security), QLocale()) + QObject::tr(" - Chart"));
}