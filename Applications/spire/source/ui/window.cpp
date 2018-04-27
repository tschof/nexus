#include "spire/ui/window.hpp"
#include <QApplication>
#include <QEvent>
#include <QResizeEvent>
#include <QVBoxLayout>
#include "spire/spire/dimensions.hpp"
#include "spire/ui/drop_shadow.hpp"
#include "spire/ui/title_bar.hpp"

using namespace spire;

window::window(QWidget* body, QWidget* parent)
    : QWidget(parent),
      m_body(body),
      m_is_resizing(false),
      m_hovered(false) {
  this->::QWidget::window()->setWindowFlags(
    this->::QWidget::window()->windowFlags() | Qt::Window |
    Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
  this->::QWidget::window()->setAttribute(Qt::WA_TranslucentBackground);
  m_shadow = std::make_unique<drop_shadow>(this);
  m_shadow->setMouseTracking(true);
  m_shadow->installEventFilter(this);
  resize(m_body->width(), m_body->height());
  auto layout = new QVBoxLayout(this);
  layout->setContentsMargins({});
  layout->setSpacing(0);
  m_border = new QWidget(this);
  m_border->setMouseTracking(true);
  m_border->installEventFilter(this);
  m_border->setObjectName("window_border");
  m_border->resize(m_body->size() + scale(1, 1));
  set_border_stylesheet("#A0A0A0");
  layout->addWidget(m_border);
  auto border_layout = new QVBoxLayout(m_border);
  border_layout->setMargin(scale_width(1));
  border_layout->setSpacing(0);
  m_title_bar = new title_bar(m_border);
  border_layout->addWidget(m_title_bar);
  border_layout->addWidget(m_body, 1);
  this->::QWidget::window()->installEventFilter(this);
  qApp->setOverrideCursor(QCursor(Qt::ArrowCursor));
}

void window::set_icon(const QImage& icon) {
  m_title_bar->set_icon(icon);
}

void window::set_icon(const QImage& icon, const QImage& unfocused_icon) {
  m_title_bar->set_icon(icon, unfocused_icon);
}

bool window::eventFilter(QObject* watched, QEvent* event) {
  if(watched == this->::QWidget::window()) {
    if(event->type() == QEvent::WindowActivate) {
      set_border_stylesheet("#A0A0A0");
      m_shadow->raise();
    } else if(event->type() == QEvent::WindowDeactivate) {
      set_border_stylesheet("#C8C8C8");
    } else if(event->type() == QEvent::Resize) {
      auto e = static_cast<QResizeEvent*>(event);
      if(e->size().height() > height()) {
        this->::QWidget::window()->resize(size());
      }
      if(m_resize_boxes.is_initialized()) {
        update_resize_boxes();
      }
    } else if(event->type() == QEvent::Move) {
      if(m_resize_boxes.is_initialized()) {
        update_resize_boxes();
      }
    }
  } else if(watched == m_shadow.get()) {
    if(event->type() == QEvent::MouseMove) {
      if(m_is_resizing && m_hovered) {
        handle_resize();
      } else {
        update_resize_cursor();
      }
    } else if(event->type() == QEvent::MouseButtonPress) {
      if(static_cast<QMouseEvent*>(event)->button() == Qt::LeftButton) {
        if(m_current_active_rect != active_resize_rect::NONE) {
          m_is_resizing = true;
        }
      }
    } else if(event->type() == QEvent::MouseButtonRelease) {
      m_is_resizing = false;
      update_resize_cursor();
    } else if(event->type() == QEvent::Enter) {
      m_hovered = true;
    } else if(event->type() == QEvent::Leave) {
      m_hovered = false;
    }
  }
  return QWidget::eventFilter(watched, event);
}

void window::handle_resize() {
  if(m_current_active_rect == active_resize_rect::TOP) {
    auto difference = QCursor::pos() -
      QWidget::window()->geometry().topLeft();
    QWidget::window()->setGeometry(
      QWidget::window()->pos().x(),
      QWidget::window()->pos().y() + difference.y(),
      QWidget::window()->size().width(),
      QWidget::window()->size().height() - difference.y());
  } else if(m_current_active_rect == active_resize_rect::TOP_RIGHT) {
    auto difference = QCursor::pos() -
      QWidget::window()->geometry().topRight();
    QWidget::window()->setGeometry(
      QWidget::window()->pos().x(),
      QWidget::window()->pos().y() + difference.y(),
      QWidget::window()->size().width() + difference.x(),
      QWidget::window()->size().height() - difference.y());
  } else if(m_current_active_rect == active_resize_rect::RIGHT) {
    auto difference =  QCursor::pos() -
      QWidget::window()->geometry().topRight();
    QWidget::window()->resize(
      QWidget::window()->size().width() + difference.x(),
      QWidget::window()->size().height());
  } else if(m_current_active_rect == active_resize_rect::BOTTOM_RIGHT) {
    auto difference = QCursor::pos() -
      QWidget::window()->geometry().bottomRight();
    QWidget::window()->resize(
      QWidget::window()->size().width() + difference.x(),
      QWidget::window()->size().height() + difference.y());
  } else if(m_current_active_rect == active_resize_rect::BOTTOM) {
    auto difference = QCursor::pos() -
      QWidget::window()->geometry().bottomLeft();
    QWidget::window()->resize(
      QWidget::window()->size().width(),
      QWidget::window()->size().height() + difference.y());
  } else if(m_current_active_rect == active_resize_rect::BOTTOM_LEFT) {
    auto difference = QCursor::pos() -
      QWidget::window()->geometry().bottomLeft();
    QWidget::window()->setGeometry(
      QWidget::window()->pos().x() + difference.x(),
      QWidget::window()->pos().y(),
      QWidget::window()->size().width() - difference.x(),
      QWidget::window()->size().height() + difference.y());
  } else if(m_current_active_rect == active_resize_rect::LEFT) {
    auto difference = QCursor::pos() -
      QWidget::window()->geometry().topLeft();
    QWidget::window()->setGeometry(
      QWidget::window()->pos().x() + difference.x(),
      QWidget::window()->pos().y(),
      QWidget::window()->size().width() - difference.x(),
      QWidget::window()->size().height());
  } else if(m_current_active_rect == active_resize_rect::TOP_LEFT) {
    auto difference = QCursor::pos() -
      QWidget::window()->geometry().topLeft();
    QWidget::window()->setGeometry(
      QWidget::window()->pos().x() + difference.x(),
      QWidget::window()->pos().y() + difference.y(),
      QWidget::window()->size().width() - difference.x(),
      QWidget::window()->size().height() - difference.y());
  }
}

void window::set_border_stylesheet(const QColor& color) {
  m_border->setStyleSheet(QString(R"(
    #window_border {
      border: %1px solid %3 %2px solid %3;
    })").arg(scale_height(1)).arg(scale_width(1)).arg(color.name()));
}

void window::update_resize_boxes() {
  auto padding_size = scale(20, 20);
  auto top_left = mapToGlobal(geometry().topLeft());
  if(!m_resize_boxes.is_initialized()) {
    m_resize_boxes.emplace();
  }
  m_resize_boxes->m_top_left = QRect(top_left.x() - padding_size.width(),
    top_left.y() - padding_size.height(), padding_size.width(),
    padding_size.height());
  m_resize_boxes->m_top = QRect(top_left.x(),
    top_left.y() - padding_size.height(), width(), padding_size.height());
  m_resize_boxes->m_top_right = QRect(top_left.x() + width(),
    top_left.y() - padding_size.height(),
    padding_size.width(), padding_size.height());
  m_resize_boxes->m_right = QRect(top_left.x() + width(), top_left.y(),
    padding_size.width(), top_left.y() + height());
  m_resize_boxes->m_bottom_right = QRect(top_left.x() + width(),
    top_left.y() + height(), padding_size.width(), padding_size.height());
  m_resize_boxes->m_bottom = QRect(top_left.x(), top_left.y() + height(),
    width(), padding_size.height());
  m_resize_boxes->m_bottom_left = QRect(top_left.x() - padding_size.width(),
    top_left.y() + height(), padding_size.width(), padding_size.height());
  m_resize_boxes->m_left = QRect(top_left.x() - padding_size.width(),
    top_left.y(), padding_size.width(), top_left.y() + height());
}

void window::update_resize_cursor() {
  if(!m_resize_boxes.is_initialized()) {
    update_resize_boxes();
  }
  auto pos = QCursor::pos();
  auto cursor = qApp->overrideCursor()->shape();
  m_current_active_rect = active_resize_rect::NONE;
  if(m_body->sizePolicy().horizontalPolicy() != QSizePolicy::Fixed) {
    if(m_resize_boxes->m_right.contains(pos)) {
      cursor = Qt::SizeHorCursor;
      m_current_active_rect = active_resize_rect::RIGHT;
    } else if(m_resize_boxes->m_left.contains(pos)) {
      cursor = Qt::SizeHorCursor;
      m_current_active_rect = active_resize_rect::LEFT;
    }
  }
  if(m_body->sizePolicy().verticalPolicy() != QSizePolicy::Fixed) {
    if(m_resize_boxes->m_bottom.contains(pos)) {
      cursor = Qt::SizeVerCursor;
      m_current_active_rect = active_resize_rect::BOTTOM;
    } else if(m_resize_boxes->m_top.contains(pos)) {
      cursor = Qt::SizeVerCursor;
      m_current_active_rect = active_resize_rect::TOP;
    }
  }
  if(m_body->sizePolicy().verticalPolicy() != QSizePolicy::Fixed &&
      m_body->sizePolicy().horizontalPolicy() != QSizePolicy::Fixed) {
    if(m_resize_boxes->m_bottom_right.contains(pos)) {
      cursor = Qt::SizeFDiagCursor;
      m_current_active_rect = active_resize_rect::BOTTOM_RIGHT;
    } else if(m_resize_boxes->m_top_left.contains(pos)) {
      cursor = Qt::SizeFDiagCursor;
      m_current_active_rect = active_resize_rect::TOP_LEFT;
    } else if(m_resize_boxes->m_top_right.contains(pos)) {
      cursor = Qt::SizeBDiagCursor;
      m_current_active_rect = active_resize_rect::TOP_RIGHT;
    } else if(m_resize_boxes->m_bottom_left.contains(pos)) {
      cursor = Qt::SizeBDiagCursor;
      m_current_active_rect = active_resize_rect::BOTTOM_LEFT;
    }
  }
  if(m_current_active_rect == active_resize_rect::NONE) {
    cursor = Qt::ArrowCursor;
  }
  qApp->restoreOverrideCursor();
  qApp->setOverrideCursor(QCursor(cursor));
}
