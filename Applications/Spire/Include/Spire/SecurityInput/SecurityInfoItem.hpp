#ifndef SPIRE_SECURITY_INFO_ITEM_HPP
#define SPIRE_SECURITY_INFO_ITEM_HPP
#include <QLabel>
#include "Nexus/Definitions/SecurityInfo.hpp"
#include "Spire/SecurityInput/SecurityInput.hpp"
#include "Spire/Ui/DropDownItem.hpp"

namespace Spire {

  //! A convenience widget to display a security's name and ticker symbol.
  class SecurityInfoItem : public DropDownItem {
    public:

      //! Constructs a SecurityInfoItem.
      /*!
        \param info SecurityInfo to display.
        \param parent Parent to this widget.
      */
      explicit SecurityInfoItem(Nexus::SecurityInfo info,
        QWidget* parent = nullptr);

      //! Returns the security info represented.
      const Nexus::SecurityInfo& get_info() const;

    protected:
      void paintEvent(QPaintEvent* event) override;
      void resizeEvent(QResizeEvent* event) override;

    private:
      Nexus::SecurityInfo m_info;
      QLabel* m_security_name_label;
      QLabel* m_company_name_label;
      QLabel* m_icon_label;

      void display_company_name();
  };
}

#endif