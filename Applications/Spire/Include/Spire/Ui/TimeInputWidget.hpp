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

      //! Represents a time format used by the TimeInputWidget.
      enum class TimeFormat {

        //! Represents the HH:MM time format.
        HM,

        //! Represents the HH:MM:SS time format.
        HMS
      };

      //! Constructs a TimeInputWidget.
      /*!
        \param format The time format to display.
        \param parent The parent widget.
      */
      explicit TimeInputWidget(TimeFormat format, QWidget* parent = nullptr);

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
      struct TimeInput {
        QLineEdit* m_input;
        int m_last_valid_value;
        const int m_min_value;
        const int m_max_value;
      };

      mutable TimeSignal m_time_signal;
      TimeFormat m_time_format;
      std::vector<TimeInput> m_inputs;
      std::vector<ColonWidget*> m_colon_widgets;

      void add_colon_widget();
      void add_input(const QString& text, Qt::AlignmentFlag alignment,
        const QRegularExpression& regex, int min_value, int max_value);
      void apply_border(QLineEdit* input, const QString& css_selector,
        const QColor& color);
      void apply_style(QLineEdit* input, const QColor& color);
      QString clamped_value(const QString& text, int min_value, int max_value);
      QString clamped_value(const QString& text, int min_value, int max_value,
        int addend);
      QString get_input_value(const QString& text, int key, int min_value,
        int max_value);
      void resize_inputs();
      void set_focused_style();
      void set_style(const QColor& color);
      void set_unfocused_style();
      void on_time_modified();
  };
}

#endif
