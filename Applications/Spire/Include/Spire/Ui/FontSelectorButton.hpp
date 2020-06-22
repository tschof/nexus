#ifndef SPIRE_FONT_SELECTOR_BUTTON_HPP
#define SPIRE_FONT_SELECTOR_BUTTON_HPP
#include <QWidget>
#include "Spire/Spire/Spire.hpp"

namespace Spire {

  class FontSelectorButton : public QWidget {
    public:

      using ClickedSignal = Signal<void ()>;

      FontSelectorButton(QImage icon, QImage toggle_icon, QImage hover_icon,
        QWidget* parent = nullptr);

      boost::signals2::connection connect_clicked_signal(
        const ClickedSignal::slot_type& slot) const;

    protected:
      void keyPressEvent(QKeyEvent* event) override;
      void mousePressEvent(QMouseEvent* event) override;
      void paintEvent(QPaintEvent* event) override;

    private:
      mutable ClickedSignal m_clicked_signal;
      QImage m_icon;
      QImage m_toggle_icon;
      QImage m_hover_icon;
      bool m_is_toggled;

      void swap_toggle();
  };
}

#endif
