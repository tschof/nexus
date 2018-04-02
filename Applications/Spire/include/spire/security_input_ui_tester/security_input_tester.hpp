#ifndef SPIRE_SECURITY_INPUT_TESTER
#define SPIRE_SECURITY_INPUT_TESTER
#include <QListWidget>
#include "Nexus/Definitions/Security.hpp"

namespace spire {

  //! \brief Tester class for security_input_dialog.
  class security_input_tester : public QListWidget {
    public:
      
      //! Constructs an empty security_input_tester.
      security_input_tester(QWidget* parent = nullptr);

      //! Adds a security to the list.
      /*!
        \param security The security to add.
      */
      void add_security(const Nexus::Security& security);

    private:
      void run_dialog();
  };
}

#endif
