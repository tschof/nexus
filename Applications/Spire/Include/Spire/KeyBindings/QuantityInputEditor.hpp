#ifndef SPIRE_QUANTITY_INPUT_EDITOR_HPP
#define SPIRE_QUANTITY_INPUT_EDITOR_HPP
#include "Spire/Ui/TextInputWidget.hpp"

namespace Spire {

  //! Represents a table editor for editing Quantities.
  class QuantityInputEditor : public TextInputWidget {
    public:

      //! Constructs a QuantityInputEditor.
      /*
        \param initial_value Value submitted if the editor is closed with no
                             value.
        \param parent The parent widget.
      */
      explicit QuantityInputEditor(int initial_value,
        QWidget* parent = nullptr);

    protected:
      void keyPressEvent(QKeyEvent* event) override;

    private:
      int m_initial_value;
  };
}

#endif