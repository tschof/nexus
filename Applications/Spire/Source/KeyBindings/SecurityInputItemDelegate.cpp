#include "Spire/KeyBindings/SecurityInputItemDelegate.hpp"
#include <QKeyEvent>
#include "Spire/SecurityInput/SecurityInputLineEdit.hpp"

using namespace Beam;
using namespace boost::signals2;
using namespace Nexus;
using namespace Spire;

SecurityInputItemDelegate::SecurityInputItemDelegate(
  Ref<SecurityInputModel> model, QWidget* parent)
  : KeyBindingItemDelegate(parent),
    m_model(model.Get()) {}

QWidget* SecurityInputItemDelegate::createEditor(QWidget* parent,
    const QStyleOptionViewItem& option, const QModelIndex& index) const {
  auto editor = new SecurityInputLineEdit(index.data().value<Security>(),
    Ref<SecurityInputModel>(*m_model), false,
    static_cast<QWidget*>(this->parent()));
  editor->set_style(TextInputWidget::Style::CELL);
  connect(editor, &SecurityInputLineEdit::editingFinished, this,
    &SecurityInputItemDelegate::on_editing_finished);
  return editor;
}

void SecurityInputItemDelegate::setModelData(QWidget* editor,
    QAbstractItemModel* model, const QModelIndex& index) const {
  auto line_edit = static_cast<SecurityInputLineEdit*>(editor);
  model->setData(index,
    QVariant::fromValue<Security>(line_edit->get_security()), Qt::DisplayRole);
}