#ifndef SPIRE_QUOTE_PANEL_HPP
#define SPIRE_QUOTE_PANEL_HPP
#include <QLabel>
#include <QWidget>
#include "Nexus/Definitions/BboQuote.hpp"
#include "Nexus/Definitions/Side.hpp"
#include "spire/book_view/book_view.hpp"

namespace Spire {

  //! Displays one side of a BBO quote, with a a colored widget
  //! indicating the increase or decrease of the quote.
  class QuotePanel : public QWidget {
    public:

      //! Constructs a QuotePanel.
      /*
        \param model The model that supplies the BBO quote data.
        \param side Which side of a BBO quote this widget represents.
        \param parent The parent widget.
      */
      QuotePanel(const std::shared_ptr<BookViewModel>& model, Nexus::Side side,
        QWidget* parent = nullptr);

    private:
      Nexus::Side m_side;
      Nexus::BboQuote m_current_bbo;
      QWidget* m_indicator_widget;
      QLabel* m_price_label;
      QLabel* m_size_label;

      void set_indicator_color(const QColor& color);
      void set_quote_text(const Nexus::Money& price,
        const Nexus::Quantity& size);
      void on_bbo_quote(const Nexus::BboQuote& bbo);
  };
}

#endif
