#ifndef SPIRE_TIME_INPUT_WIDGET_HPP
#define SPIRE_TIME_INPUT_WIDGET_HPP
#include <boost/date_time/posix_time/posix_time.hpp>
#include <QLineEdit>
#include <QRegularExpression>
#include <QWidget>
#include "Spire/Spire/Spire.hpp"
#include "Spire/Ui/ColonWidget.hpp"

namespace Spire {

  //! Represents a generic widget for inputting time.
  class TimeInputWidget : public QWidget {
    public:

      //! Signals that the time was committed.
      /*!
        \param time The committed time.
      */
      using TimeSignal =
        Signal<void (const boost::posix_time::time_duration& time)>;

      //! Constructs a TimeInputWidget.
      /*!
        \param sections Dictates the number of sections to display, using the
                        provided regular expression to limit the values that
                        can be entered into that section.
        \param parent The parent widget.
      */
      explicit TimeInputWidget(const std::vector<QRegularExpression>& sections,
        QWidget* parent = nullptr);

      //! Sets the widget's time.
      /*!
        \param time The time to display.
      */
      void set_time(const boost::posix_time::time_duration& time);

      //! Connects a slot to the time signal.
      boost::signals2::connection connect_time_signal(
        const TimeSignal::slot_type& slot) const;

    protected:
      bool eventFilter(QObject* watched, QEvent* event) override;

    private:
      mutable TimeSignal m_time_signal;
      std::vector<QLineEdit*> m_time_inputs;
      std::vector<ColonWidget*> m_colon_widgets;

      void set_focused_style();
      void set_style(const QColor& color);
      void set_unfocused_style();
  };
}

#endif
