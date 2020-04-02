#include "Spire/KeyBindings/KeySequenceItemDelegate.hpp"
#include <QEvent>
#include <QPainter>
#include "Spire/Spire/Dimensions.hpp"

using namespace Spire;

namespace {
  auto TEXT_PADDING() {
    static auto padding = scale_width(5);
    return padding;
  }
}

KeySequenceItemDelegate::KeySequenceItemDelegate(QWidget* parent)
  : QStyledItemDelegate(parent) {}

QWidget* KeySequenceItemDelegate::createEditor(QWidget* parent,
    const QStyleOptionViewItem& option, const QModelIndex& index) const {
  auto editor = new KeySequenceEditor(
    index.data(Qt::DisplayRole).value<QKeySequence>(), {Qt::Key_Escape},
    parent);
  connect(editor, &KeySequenceEditor::editingFinished,
    this, &KeySequenceItemDelegate::on_editing_finished);
  return editor;
}

//bool KeySequenceItemDelegate::editorEvent(QEvent* event,
//    QAbstractItemModel* model, const QStyleOptionViewItem& option,
//    const QModelIndex& index) {
//  if(event->type() == QEvent::KeyRelease) {
//    
//  }
//  QStyledItemDelegate::editorEvent(event, model, option, index);
//}

void KeySequenceItemDelegate::paint(QPainter* painter,
    const QStyleOptionViewItem& option, const QModelIndex& index) const {
  painter->save();
  painter->fillRect(option.rect, Qt::white);
  if(option.state.testFlag(QStyle::State_Editing)) {
    painter->setPen(QColor("#4B23A0"));
    painter->drawRect(option.rect);
  } else {
    auto sequence = index.data(Qt::DisplayRole).value<QKeySequence>();
    if(!sequence.isEmpty()) {
      draw_key_sequence(sequence, option.rect, painter);
    }
  }
  painter->restore();
}

void KeySequenceItemDelegate::setModelData(QWidget* editor,
    QAbstractItemModel* model, const QModelIndex& index) const {
  auto key_editor = reinterpret_cast<KeySequenceEditor*>(editor);
  model->setData(index, key_editor->get_key_sequence(), Qt::DisplayRole);
}

QSize KeySequenceItemDelegate::sizeHint(const QStyleOptionViewItem& option,
    const QModelIndex& index) const {
  return QStyledItemDelegate::sizeHint(option, index);
}

void KeySequenceItemDelegate::draw_key_sequence(const QKeySequence& sequence,
    const QRect& rect, QPainter* painter) const {
  auto keys = sequence.toString(QKeySequence::NativeText).split(",",
    QString::SkipEmptyParts);
  auto pos = rect.bottomLeft();
  pos.setX(pos.x() + scale_width(8));
  auto font = QFont("Roboto");
  font.setPixelSize(scale_height(12));
  font.setWeight(55);
  painter->setFont(font);
  painter->setRenderHint(QPainter::Antialiasing);
  auto metrics = QFontMetrics(font);
  for(auto& key : keys) {
    auto text_size = QSize(metrics.horizontalAdvance(key), metrics.height());
    draw_key(key, text_size, pos, painter);
    pos.setX(pos.x() + TEXT_PADDING() * 2 + text_size.width() +
      scale_width(8));
  }
}

void KeySequenceItemDelegate::draw_key(const QString& text,
    const QSize& text_size, const QPoint& pos, QPainter* painter) const {
  auto path = QPainterPath();
  path.addRoundedRect(QRectF(pos.x(), pos.y() - scale_height(18) -
    scale_height(4), text_size.width() + TEXT_PADDING() * 2,
    scale_height(18)), scale_width(2), scale_height(2));
  painter->setPen({QColor("#E2C899"), static_cast<qreal>(scale_width(1))});
  painter->fillPath(path, QColor("#FFEDCD"));
  painter->drawPath(path);
  painter->setPen(Qt::black);
  painter->drawText(pos.x() + TEXT_PADDING(),
    pos.y() - (text_size.height() / 2) - scale_height(2), text);
}

void KeySequenceItemDelegate::on_editing_finished() {
  auto editor = reinterpret_cast<KeySequenceEditor*>(sender());
  emit commitData(editor);
  emit closeEditor(editor);
}
