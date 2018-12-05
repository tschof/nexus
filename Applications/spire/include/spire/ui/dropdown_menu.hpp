#ifndef SPIRE_DROPDOWN_MENU_HPP
#define SPIRE_DROPDOWN_MENU_HPP
#include <QComboBox>
#include "spire/ui/ui.hpp"

namespace Spire {

  //! Displays a Spire-styled QComboBox.
  class DropdownMenu : public QWidget {
    public:

      //! Constructs a DropdownMenu with specified items. The first item
      //! in the vector is the default selected item.
      /*
        \param items The items to display in the menu.
        \param parent The parent widget to the DropdownMenu.
      */
      explicit DropdownMenu(const std::vector<QString>& items,
        QWidget* parent = nullptr);

      //! Clears the items from the list and populates it with the specified
      //! items.
      /*
        \param items The items to display in the menu.
      */
      void set_items(const std::vector<QString>& items);

      //! Returns the selected item.
      const QString& get_text() const;

    protected:
      bool eventFilter(QObject* object, QEvent* event) override;
      void focusOutEvent(QFocusEvent* event) override;
      void keyPressEvent(QKeyEvent* event) override;
      void mousePressEvent(QMouseEvent* event) override;
      void paintEvent(QPaintEvent* event) override;

    private:
      QString m_current_text;
      QImage m_dropdown_image;
      DropdownMenuList* m_menu_list;

      void move_menu_list();
      void on_clicked();
      void on_item_selected(const QString& text);
  };
}

#endif
