#ifndef SPIRE_KEY_SEQUENCE_TEST_WIDGET_HPP
#define SPIRE_KEY_SEQUENCE_TEST_WIDGET_HPP
#include <QWidget>

namespace Spire {

  //! Represents a widget for testing a KeySequenceEditor.
  class KeySequenceTestWidget : public QWidget {
    public:

      //! Constructs a KeySequenceTestWidget.
      /*!
        \param parent The parent widget.
      */
      KeySequenceTestWidget(QWidget* parent = nullptr);
  };
}

#endif
