#include "spire/time_and_sales/time_and_sales_window.hpp"
#include <QFileDialog>
#include <QKeyEvent>
#include <QStandardPaths>
#include <QVBoxLayout>
#include "spire/security_input/security_input_dialog.hpp"
#include "spire/security_input/security_input_model.hpp"
#include "spire/time_and_sales/empty_time_and_sales_model.hpp"
#include "spire/time_and_sales/time_and_sales_properties_dialog.hpp"
#include "spire/time_and_sales/time_and_sales_table_view.hpp"
#include "spire/time_and_sales/time_and_sales_window_model.hpp"
#include "spire/spire/dimensions.hpp"
#include "spire/spire/export_model.hpp"
#include "spire/ui/custom_qt_variants.hpp"
#include "spire/ui/drop_shadow.hpp"
#include "spire/ui/overlay_widget.hpp"
#include "spire/ui/window.hpp"

using namespace boost;
using namespace boost::signals2;
using namespace Nexus;
using namespace spire;

time_and_sales_window::time_and_sales_window(
    const time_and_sales_properties& properties, 
    security_input_model& input_model, QWidget* parent)
    : QWidget(parent),
      m_input_model(&input_model),
      m_table(nullptr) {
  m_body = new QWidget(this);
  m_body->setMinimumSize(scale(40, 200));
  resize(scale_width(182), scale_height(452));
  m_body->setStyleSheet("background-color: #FFFFFF;");
  auto window_layout = new QHBoxLayout(this);
  window_layout->setContentsMargins({});
  auto window = new spire::window(m_body, this);
  setWindowTitle(tr("Time and Sales"));
  window->set_svg_icon(":/icons/time-sale-black.svg",
    ":/icons/time-sale-grey.svg");
  window_layout->addWidget(window);
  auto layout = new QVBoxLayout(m_body);
  layout->setContentsMargins({});
  layout->setSpacing(0);
  auto padding_widget = new QWidget(this);
  padding_widget->setFixedHeight(scale_height(4));
  padding_widget->setStyleSheet("background-color: #F5F5F5;");
  layout->addWidget(padding_widget);
  m_empty_window_label = new QLabel(tr("Enter a ticker symbol."), this);
  m_empty_window_label->setAlignment(Qt::AlignCenter);
  m_empty_window_label->setStyleSheet(QString(R"(
    font-family: Roboto;
    font-size: %1px;)").arg(scale_height(11)));
  layout->addWidget(m_empty_window_label);
  m_table = new time_and_sales_table_view(this);
  layout->addWidget(m_table);
  m_table->hide();
  m_volume_label = new QLabel(tr("Volume:"), this);
  m_volume_label->setFocusPolicy(Qt::NoFocus);
  m_volume_label->setFixedHeight(scale_height(20));
  m_volume_label->setStyleSheet(QString(R"(
    background-color: #F5F5F5;
    color: #333333;
    font-family: Roboto;
    font-size: %1px;
    font-weight: 550;
    padding-left: %2px;)").arg(scale_height(10)).arg(scale_width(8)));
  layout->addWidget(m_volume_label);
  set_model(std::make_shared<empty_time_and_sales_model>(Security()));
  set_properties(properties);
}

void time_and_sales_window::set_model(
    std::shared_ptr<time_and_sales_model> model) {
  if(m_model.is_initialized()) {
    if(m_empty_window_label != nullptr) {
      delete m_empty_window_label;
      m_empty_window_label = nullptr;
    }
    m_table->show();
  }
  model->connect_volume_signal([=] (const Quantity& v) { on_volume(v); });
  m_model.emplace(std::move(model), m_properties);
  m_table->set_model(m_model.get_ptr());
}

const time_and_sales_properties&
    time_and_sales_window::get_properties() const {
  return m_properties;
}

void time_and_sales_window::set_properties(
    const time_and_sales_properties& properties) {
  m_properties = properties;
  m_model->set_properties(m_properties);
  if(m_table == nullptr) {
    return;
  }
  m_table->set_properties(m_properties);
}

connection time_and_sales_window::connect_security_change_signal(
    const change_security_signal::slot_type& slot) const {
  return m_change_security_signal.connect(slot);
}

connection time_and_sales_window::connect_closed_signal(
    const closed_signal::slot_type& slot) const {
  return m_closed_signal.connect(slot);
}

void time_and_sales_window::closeEvent(QCloseEvent* event) {
  m_closed_signal();
}

void time_and_sales_window::contextMenuEvent(QContextMenuEvent* event) {
  auto contents = m_body->layout()->itemAt(1)->widget();
  QRect widget_geometry(contents->mapToGlobal(contents->geometry().topLeft()),
    contents->mapToGlobal(contents->geometry().bottomRight()));
  if(widget_geometry.contains(event->globalPos())) {
    QMenu context_menu(this);
    QAction properties_action(tr("Properties"), &context_menu);
    connect(&properties_action, &QAction::triggered, this,
      &time_and_sales_window::show_properties_dialog);
    context_menu.addAction(&properties_action);
    QAction export_action(tr("Export Table"), &context_menu);
    connect(&export_action, &QAction::triggered, this,
      &time_and_sales_window::export_table);
    export_action.setEnabled(m_table->isVisible());
    context_menu.addAction(&export_action);
    context_menu.setFixedWidth(scale_width(140));
    context_menu.setWindowFlag(Qt::NoDropShadowWindowHint);
    drop_shadow context_menu_shadow(true, true, &context_menu);
    context_menu.setStyleSheet(QString(R"(
      QMenu {
        background-color: #FFFFFF;
        border: %1px solid #A0A0A0 %2px solid #A0A0A0;
        color: #000000;
        font-family: Roboto;
        font-size: %3px;
        padding: %4px 0px;
      }

      QMenu::item {
        padding: %5px 0px %5px %6px;
      }

      QMenu::item:disabled,
      QMenu::item:disabled:selected,
      QMenu::item:disabled:hover {
        background-color: #FFFFFF;
        color: #C8C8C8;
      }

      QMenu::item:selected, QMenu::item:hover {
        background-color: #8D78EC;
        color: #FFFFFF;
      })")
      .arg(scale_height(1)).arg(scale_width(1))
      .arg(scale_height(12)).arg(scale_height(5))
      .arg(scale_height(3)).arg(scale_width(8)));
    context_menu.exec(event->globalPos());
  }
}

void time_and_sales_window::keyPressEvent(QKeyEvent* event) {
  if(event->key() == Qt::Key_PageUp) {
    if(m_current_security != Security()) {
      auto s = m_securities.push_front(m_current_security);
      if(s != Security()) {
        set_current(s);
      }
    }
    return;
  } else if(event->key() == Qt::Key_PageDown) {
    if(m_current_security != Security()) {
      auto s = m_securities.push_back(m_current_security);
      if(s != Security()) {
        set_current(s);
      }
    }
    return;
  }
  auto pressed_key = event->text();
  if(pressed_key[0].isLetterOrNumber()) {
    auto dialog = new security_input_dialog(*m_input_model, pressed_key, this);
    dialog->setWindowModality(Qt::NonModal);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog, &QDialog::accepted, this,
      [=] { on_security_input_accept(dialog); });
    connect(dialog, &QDialog::rejected, this,
      [=] { on_security_input_reject(dialog); });
    dialog->move(geometry().center().x() -
      dialog->width() / 2, geometry().center().y() - dialog->height() / 2);
    show_overlay_widget();
    dialog->show();
  }
}

void time_and_sales_window::export_table() {
  show_overlay_widget();
  auto filepath = QFileDialog::getSaveFileName(this, tr("Export As"),
    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) +
    tr("/time_and_sales"), "CSV (*.csv)");
  if(!filepath.isNull()) {
    export_model_as_csv(m_model.get(), std::ofstream(filepath.toStdString()));
  }
  m_overlay_widget.reset();
}

void time_and_sales_window::show_overlay_widget() {
  auto contents = m_body->layout()->itemAt(1)->widget();
  m_overlay_widget = std::make_unique<QLabel>(m_body);
  m_overlay_widget->setStyleSheet(
    "background-color: rgba(245, 245, 245, 153);");
  m_overlay_widget->resize(contents->size());
  m_overlay_widget->move(contents->mapTo(contents, contents->pos()));
  m_overlay_widget->show();
}

void time_and_sales_window::show_properties_dialog() {
  time_and_sales_properties_dialog dialog(m_properties, this);
  dialog.connect_apply_signal([=] (auto p) { set_properties(p); });
  show_overlay_widget();
  if(dialog.exec() == QDialog::Accepted) {
    set_properties(dialog.get_properties());
  }
  m_overlay_widget.reset();
}

void time_and_sales_window::set_current(const Security& s) {
  if(s == m_current_security) {
    return;
  }
  m_current_security = s;
  m_change_security_signal(s);
  m_volume_label->setText(tr("Volume:"));
  setWindowTitle(QString::fromStdString(ToString(s)) +
    tr(" - Time and Sales"));
}

void time_and_sales_window::on_security_input_accept(
    security_input_dialog* dialog) {
  auto s = dialog->get_security();
  if(s != Security() && s != m_current_security) {
    m_securities.push(m_current_security);
    set_current(s);
    activateWindow();
  }
  dialog->close();
  m_overlay_widget.reset();
}

void time_and_sales_window::on_security_input_reject(
    security_input_dialog* dialog) {
  dialog->close();
  m_overlay_widget.reset();
}

void time_and_sales_window::on_volume(const Quantity& volume) {
  m_volume_label->setText(tr("Volume:").append(" ") +
    Beam::ToString(volume).c_str());
}