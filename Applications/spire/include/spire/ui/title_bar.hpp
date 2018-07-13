#ifndef SPIRE_TITLE_BAR_HPP
#define SPIRE_TITLE_BAR_HPP
#include <QAbstractNativeEventFilter>
#include <QLabel>
#include <QWidget>
#include "spire/ui/ui.hpp"

namespace spire {

  //! Displays a window's title bar.
  class title_bar : public QWidget, public QAbstractNativeEventFilter {
    public:

      //! Constructs a title bar.
      /*!
        \param body The body content widget. Used to determine maximum size
                    when changing the window state.
        \param parent The parent widget.
      */
      title_bar(QWidget* body, QWidget* parent = nullptr);

      //! Constructs a title bar.
      /*!
        \param icon The window icon to display at the top-left corner.
        \param body The body content widget. Used to determine maximum size
                    when changing the window state.
        \param parent The parent widget.
      */
      title_bar(const QImage& icon, QWidget* body, QWidget* parent = nullptr);

      //! Constructs a title bar.
      /*!
        \param icon The window icon to display at the top-left corner.
        \param unfocused_icon The icon to display when the window lacks focus.
        \param body The body content widget. Used to determine maximum size
                    when changing the window state.
        \param parent The parent widget.
      */
      title_bar(const QImage& icon, const QImage& unfocused_icon,
        QWidget* body, QWidget* parent = nullptr);

      //! Sets the icon to display.
      /*!
        \param icon The icon to display when the window has focus.
      */
      void set_icon(const QImage& icon);

      //! Sets the icon to display.
      /*!
        \param icon The icon to display when the window has focus.
        \param icon The icon to display when the window lacks focus.
      */
      void set_icon(const QImage& icon, const QImage& unfocused_icon);

      bool nativeEventFilter(const QByteArray& event_type, void* message,
        long* result) override;

    protected:
      bool eventFilter(QObject* watched, QEvent* event) override;
      void mouseDoubleClickEvent(QMouseEvent* event) override;
      void mouseMoveEvent(QMouseEvent* event) override;
      void mousePressEvent(QMouseEvent* event) override;
      void mouseReleaseEvent(QMouseEvent* event) override;
      void resizeEvent(QResizeEvent* event) override;

    private:
      icon_button* m_icon;
      QImage m_default_icon;
      QImage m_unfocused_icon;
      QLabel* m_title_label;
      icon_button* m_minimize_button;
      icon_button* m_maximize_button;
      icon_button* m_restore_button;
      icon_button* m_close_button;
      bool m_is_dragging;
      QPoint m_last_mouse_pos;
      QWidget* m_body;
      QRect m_restore_geometry;

      void drag_restore(const QPoint& pos);
      void on_window_title_change(const QString& title);
      void on_minimize_button_press();
      void on_maximize_button_press();
      void on_restore_button_press();
      void on_close_button_press();
      void set_title_text_stylesheet(const QColor& font_color);
  };
}

#endif