#include "Spire/KeyBindings/InputFieldEditor.hpp"
#include <QEvent>
#include <QKeyEvent>
#include <QPainter>
#include "Spire/Spire/Dimensions.hpp"

using namespace Spire;

InputFieldEditor::InputFieldEditor(QString initial_value,
    std::vector<QString> items, QWidget* parent)
    : QLineEdit(parent),
      m_selected_item(std::move(initial_value)),
      m_items(std::move(items)) {
  setObjectName("input_field_editor");
  setContextMenuPolicy(Qt::NoContextMenu);
  setStyleSheet(QString(R"(
      #input_field_editor {
      background-color: #FFFFFF;
      border: none;
      color: #000000;
      font-family: Roboto;
      font-size: %1px;
      padding-left: %2px;
    })").arg(scale_height(12)).arg(scale_width(6)));
  connect(this, &QLineEdit::textEdited, this,
    &InputFieldEditor::on_text_edited);
  m_menu_list = new DropDownMenuList(m_items, this);
  m_menu_list->hide();
  m_menu_list->connect_highlighted_signal([=] (const auto& item) {
    on_item_highlighted(item);
  });
  m_menu_list->connect_selected_signal([=] (const auto& item) {
    on_item_clicked(item);
  });
  window()->installEventFilter(this);
  parent->installEventFilter(this);
}

const QString& InputFieldEditor::get_item() const {
  if(!m_highlighted_item.isEmpty()) {
    return m_highlighted_item;
  }
  return m_selected_item;
}

bool InputFieldEditor::eventFilter(QObject* watched, QEvent* event) {
  if(watched == window()) {
    if(event->type() == QEvent::Move) {
      if(m_menu_list->isVisible()) {
        move_menu_list();
      }
    }
  } else if(watched == parent()) {
    if(event->type() == QEvent::Wheel) {
      m_menu_list->hide();
    }
  }
  return QLineEdit::eventFilter(watched, event);
}

void InputFieldEditor::keyPressEvent(QKeyEvent* event) {
  if(event->key() == Qt::Key_Tab || event->key() == Qt::Key_Backtab) {
    event->ignore();
    return;
  }
  if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
    on_item_committed(text());
    return;
  }
  QLineEdit::keyPressEvent(event);
}

void InputFieldEditor::keyReleaseEvent(QKeyEvent* event) {
  if(event->key() == Qt::Key_Delete) {
    on_item_committed("");
  }
}

void InputFieldEditor::showEvent(QShowEvent* event) {
  move_menu_list();
  m_menu_list->setFixedWidth(width());
  m_menu_list->show();
  m_menu_list->raise();
}

void InputFieldEditor::move_menu_list() {
  auto x_pos = static_cast<QWidget*>(parent())->mapToGlobal(
    geometry().bottomLeft()).x();
  auto y_pos = static_cast<QWidget*>(parent())->mapToGlobal(
    frameGeometry().bottomLeft()).y();
  m_menu_list->move(x_pos, y_pos + 2);
  m_menu_list->raise();
}

void InputFieldEditor::on_item_clicked(const QString& item) {
  m_selected_item = item;
  on_item_committed(item);
}

void InputFieldEditor::on_item_committed(const QString& text) {
  m_menu_list->hide();
  Q_EMIT editingFinished();
}

void InputFieldEditor::on_item_highlighted(const QString& item) {
  setText(item);
  m_highlighted_item = item;
}

void InputFieldEditor::on_text_edited(const QString& text) {
  m_highlighted_item.clear();
  auto iter = std::find_if(m_items.begin(), m_items.end(),
    [&] (const auto& value) { return value.toLower() == text.toLower(); });
  if(iter != m_items.end()) {
    m_selected_item = *iter;
  }
  if(text.isEmpty()) {
    m_menu_list->set_items(m_items);
    m_menu_list->show();
    return;
  }
  auto displayed_items = [&] {
    auto items = std::vector<QString>();
    for(auto& item : m_items) {
      if(item.startsWith(text, Qt::CaseInsensitive)) {
        items.push_back(item);
      }
    }
    return items;
  }();
  if(displayed_items.empty()) {
    m_menu_list->hide();
    return;
  }
  m_menu_list->set_items(displayed_items);
  m_menu_list->show();
}