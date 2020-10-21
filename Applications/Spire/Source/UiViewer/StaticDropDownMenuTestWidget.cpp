#include "Spire/UiViewer/StaticDropDownMenuTestWidget.hpp"
#include "Spire/Spire/Dimensions.hpp"
#include "Spire/Spire/Utility.hpp"
#include "Spire/Ui/FlatButton.hpp"

using namespace Spire;

namespace {
  auto CONTROL_SIZE() {
    static auto size = scale(100, 26);
    return size;
  }

  auto get_variant_list(const QString& text) {
    auto strings = text.split(",", Qt::SkipEmptyParts);
    auto items = std::vector<QVariant>();
    items.reserve(strings.size());
    std::transform(strings.begin(), strings.end(),
      std::back_inserter(items),
      [] (auto string) {
        return QVariant::fromValue(string.replace(" ", ""));
      });
    return items;
  }
}

StaticDropDownMenuTestWidget::StaticDropDownMenuTestWidget(QWidget* parent)
    : QWidget(parent),
      m_menu(nullptr) {
  auto container = new QWidget(this);
  m_layout = new QGridLayout(this);
  m_status_label = new QLabel(this);
  m_layout->addWidget(m_status_label, 0, 1);
  m_item_input = new QPlainTextEdit(this);
  m_item_input->setPlainText("AA, AB, AC, BA, BB, BC, CA, CB, CC");
  m_item_input->setStyleSheet(QString(R"(
    QPlainTextEdit {
      background-color: #FFFFFF;
      border: %1px solid #C8C8C8 %2px solid #C8C8C8;
      color: #000000;
      font-family: Roboto;
      font-size: %3px;
    }

    QPlainTextEdit:focus {
      border: %1px solid #4B23A0 %2px solid #4B23A0;
    })").arg(scale_height(1)).arg(scale_width(1)).arg(scale_height(12)));
  m_item_input->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  m_layout->addWidget(m_item_input, 1, 0, 6, 2);
  m_label_input = new TextInputWidget(this);
  m_label_input->setPlaceholderText(tr("Fixed Label"));
  m_label_input->setFixedSize(CONTROL_SIZE());
  m_layout->addWidget(m_label_input, 7, 1);
  auto set_button = make_flat_button(tr("Set Items"), this);
  set_button->setFixedSize(CONTROL_SIZE());
  set_button->connect_clicked_signal([=] { on_set_button(); });
  m_layout->addWidget(set_button, 8, 0);
  auto reset_button = make_flat_button(tr("Reset"), this);
  reset_button->setFixedSize(CONTROL_SIZE());
  reset_button->connect_clicked_signal([=] { on_reset_button(); });
  m_layout->addWidget(reset_button, 8, 1);
  m_insert_item_input = new TextInputWidget(this);
  m_insert_item_input->setFixedSize(CONTROL_SIZE());
  m_layout->addWidget(m_insert_item_input, 9, 0);
  auto insert_item_button = make_flat_button(tr("Insert Item"), this);
  insert_item_button->setFixedSize(CONTROL_SIZE());
  insert_item_button->connect_clicked_signal([=] { on_insert_button(); });
  m_layout->addWidget(insert_item_button, 9, 1);
  m_index_input = new TextInputWidget(this);
  m_index_input->setFixedSize(CONTROL_SIZE());
  m_layout->addWidget(m_index_input, 10, 0);
  auto remove_item_button = make_flat_button(tr("Remove Index"), this);
  remove_item_button->setFixedSize(CONTROL_SIZE());
  remove_item_button->connect_clicked_signal([=] { on_remove_button(); });
  m_layout->addWidget(remove_item_button, 10, 1);
  m_next_item_check_box = make_check_box(tr("Activate Next"), this);
  m_next_item_check_box->setChecked(true);
  connect(m_next_item_check_box, &CheckBox::stateChanged, [=] (auto state) {
    m_menu->set_next_activated(m_next_item_check_box->isChecked());
  });
  m_layout->addWidget(m_next_item_check_box, 11, 0);
  on_reset_button();
}

void StaticDropDownMenuTestWidget::on_insert_button() {
  auto item = new DropDownItem(m_insert_item_input->text(), this);
  item->setFixedHeight(scale_height(20));
  m_menu->insert_item(item);
}

void StaticDropDownMenuTestWidget::on_set_button() {
  m_menu->set_items(get_variant_list(m_item_input->toPlainText()));
}

void StaticDropDownMenuTestWidget::on_remove_button() {
  auto index_ok = false;
  auto index = m_index_input->text().toInt(&index_ok);
  if(index_ok) {
    m_menu->remove_item(index);
  }
}

void StaticDropDownMenuTestWidget::on_reset_button() {
  delete_later(m_menu);
  m_menu = new StaticDropDownMenu(
    get_variant_list(m_item_input->toPlainText()), m_label_input->text(),
    this);
  m_menu->setFixedSize(CONTROL_SIZE());
  m_menu->connect_value_selected_signal([=] (const auto& item) {
    m_status_label->setText(item.toString());
  });
  m_layout->addWidget(m_menu, 0, 0);
}
