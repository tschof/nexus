#ifndef SPIRE_WINDOW_HPP
#define SPIRE_WINDOW_HPP
#include <QAbstractNativeEventFilter>
#include <QWidget>
#include "spire/ui/ui.hpp"

namespace Spire {

  //! A customized window container for top-level windows.
  class Window : public QWidget, QAbstractNativeEventFilter {
    public:

      //! Constructs a Window.
      /*!
        \param body The widget displayed within the window.
        \param parent The parent widget to the window.
      */
      Window(QWidget* body, QWidget* parent = nullptr);

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

      //! Sets the icon to display using an SVG file with default size, and
      //! sets the task bar icon.
      /*
        \param path Path to the SVG file to display, and the task bar icon.
      */
      void set_svg_icon(const QString& icon_path);

      //! Sets the icons to display using an SVG file with default sizes,
      //! and sets the task bar icon.
      /*
        \param icon_path Path to the SVG file to display when the window has
               focus, and the task bar icon.
        \param unfocused_icon_path Path to the SVG file to display when the
               window lacks focus.
      */
      void set_svg_icon(const QString& icon_path,
        const QString& unfocused_icon_path);

      bool nativeEventFilter(const QByteArray& event_type, void* message,
        long* result) override;

    protected:
      bool event(QEvent* e) override;
      bool eventFilter(QObject* watched, QEvent* event) override;
      void mousePressEvent(QMouseEvent* event) override;
      void mouseReleaseEvent(QMouseEvent* event) override;
      void paintEvent(QPaintEvent* event) override;

    private:
      enum ActiveResizeRect {
        NONE = 0,
        TOP = 1,
        RIGHT = 2,
        BOTTOM = 4,
        LEFT = 8,
        TOP_LEFT = TOP | LEFT,
        TOP_RIGHT = TOP | RIGHT,
        BOTTOM_RIGHT = BOTTOM | RIGHT,
        BOTTOM_LEFT = BOTTOM | LEFT
      };
      int m_current_active_rect;
      QWidget* m_border;
      QWidget* m_body;
      TitleBar* m_title_bar;
      bool m_is_resizing;
      QSize m_normal_size;

      void handle_resize();
      void set_border_stylesheet(const QColor& color);
      void update_resize_cursor(const QPoint& pos);
  };
}

#endif
