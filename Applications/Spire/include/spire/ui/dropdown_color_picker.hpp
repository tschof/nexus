#ifndef SPIRE_DROPDOWN_COLOR_PICKER_HPP
#define SPIRE_DROPDOWN_COLOR_PICKER_HPP
#include <QWidget>
#include "spire/ui/ui.hpp"

namespace Spire {

  //! Displays a button that activates a pop-up color picker.
  class DropdownColorPicker : public QWidget {
    public:

      //! Signals that a color has been selected.
      using ColorSignal = Signal<void (const QColor& color)>;

      //! Constructs a DropdownColorPicker.
      /*
        \param parent The parent widget.
      */
      DropdownColorPicker(QWidget* parent = nullptr);

      boost::signals2::connection connect_color_signal(
        const ColorSignal::slot_type& slot) const;

    protected:
      void enterEvent(QEvent* event) override;
      bool eventFilter(QObject* watched, QEvent* event) override;
      void focusOutEvent(QFocusEvent* event) override;
      void leaveEvent(QEvent* event) override;
      void mousePressEvent(QMouseEvent* event) override;
      void paintEvent(QPaintEvent* event) override;
      void showEvent(QShowEvent* event) override;

    private:
      mutable ColorSignal m_color_signal;
      ColorPicker* m_color_picker;
      QColor m_current_color;
      QColor m_stored_color;
      bool m_has_mouse;

      void on_color_preview(const QColor& color);
      void on_color_selected(const QColor& color);
      void move_color_picker();
  };
}

#endif
