#include "spire/ui/transition_widget.hpp"
#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMovie>
#include "spire/spire/dimensions.hpp"

using namespace Spire;

TransitionWidget::TransitionWidget(QWidget* parent)
    : QWidget(parent) {
  setStyleSheet("background-color: transparent;");
  auto layout = new QHBoxLayout(this);
  layout->setContentsMargins({});
  auto backing_widget = new QLabel(this);
  auto logo = new QMovie(":/icons/pre-loader.gif", QByteArray(),
    backing_widget);
  logo->setScaledSize(scale(32, 32));
  backing_widget->setMovie(logo);
  backing_widget->setStyleSheet(
    QString("padding-top: %1px;").arg(scale_height(50)));
  backing_widget->setAlignment(Qt::AlignHCenter);
  backing_widget->movie()->start();
  layout->addWidget(backing_widget);
  parent->installEventFilter(this);
  align();
  show();
}

bool TransitionWidget::eventFilter(QObject* watched, QEvent* event) {
  if(watched == parent()) {
    if(event->type() == QEvent::Move || event->type() == QEvent::Resize) {
      align();
    }
  }
  return QWidget::eventFilter(watched, event);
}

void TransitionWidget::align() {
  auto parent_widget = static_cast<QWidget*>(parent());
  resize(parent_widget->size());
  move(parent_widget->mapTo(parent_widget, parent_widget->pos()));
}