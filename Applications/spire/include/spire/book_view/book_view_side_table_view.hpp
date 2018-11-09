#ifndef SPIRE_BOOK_VIEW_SIDE_TABLE_VIEW_HPP
#define SPIRE_BOOK_VIEW_SIDE_TABLE_VIEW_HPP
#include <QTableView>
#include "spire/book_view/book_view.hpp"

namespace Spire {

  //! Displays one side of a security's quotes in a table.
  class BookViewSideTableView : public QTableView {
    public:

      //! Constructs a BookViewSideTableView.
      /*
        \param parent The parent to this widget.
      */
      explicit BookViewSideTableView(QWidget* parent = nullptr);

      //! Sets the model for this table widget.
      /*
        \param model The model to get the table data from.
      */
      void set_model(std::unique_ptr<BookQuoteTableModel> model);

      //! Sets the properties of the table.
      /*
        \param properties The properties the table will be updated to have.
      */
      void set_properties(const BookViewProperties& properties);

    protected:
      void resizeEvent(QResizeEvent* event) override;

    private:
      std::unique_ptr<BookQuoteTableModel> m_model;
  };
}

#endif
