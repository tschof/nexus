#ifndef SPIRE_TIME_OF_DAY_TEST_WIDGET_HPP
#define SPIRE_TIME_OF_DAY_TEST_WIDGET_HPP
#include <QLabel>
#include <QWidget>
#include "Spire/Ui/TextInputWidget.hpp"
#include "Spire/Ui/TimeOfDayInputWidget.hpp"

namespace Spire {

  //! Represents a widget for testing a TimeOfDayInputwidget.
  class TimeOfDayTestWidget : public QWidget {
    public:

      //! Constructs a TimeOfDayTestWidget.
      /*!
        \param parent The parent widget.
      */
      explicit TimeOfDayTestWidget(QWidget* parent = nullptr);

    private:
      QLabel* m_status_label;
      TimeOfDayInputWidget* m_time_input;
      TextInputWidget* m_hour_input;
      TextInputWidget* m_minute_input;

      void on_set_button();
      void on_time_modified(boost::posix_time::time_duration time);
  };
}

#endif