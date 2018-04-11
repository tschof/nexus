#ifndef SPIRE_TIME_AND_SALES_PROPERTIES_DIALOG_HPP
#define SPIRE_TIME_AND_SALES_PROPERTIES_DIALOG_HPP
#include <QDialog>
#include "spire/time_and_sales/time_and_sales.hpp"
#include "spire/time_and_sales/time_and_sales_properties.hpp"

namespace spire {

  //! Displays the properties for a time and sales window.
  class time_and_sales_properties_dialog : public QDialog {
    public:

      //! Signals that properties should be applied to the parent window.
      using apply_signal = signal<
        void (const time_and_sales_properties& properties)>;

      //! Signals that properties should be applied to all windows.
      using apply_all_signal = signal<
        void (const time_and_sales_properties& properties)>;

      //! Signals that properties should be saved as the default.
      using save_default_signal = signal<
        void (const time_and_sales_properties& properties)>;

      //! Constructs a time and sales properties dialog.
      /*!
        \param properties The properties to modify.
        \param parent The parent widget.
        \param flags Qt flags passed to the parent widget.
      */
      time_and_sales_properties_dialog(
        const time_and_sales_properties& properties, QWidget* parent = nullptr,
        Qt::WindowFlags flags = 0);

      //! Returns the properties represented by this dialog.
      const time_and_sales_properties& get_properties() const;

      //! Connects a slot to the apply signal.
      boost::signals2::connection connect_apply_signal(
        const apply_signal::slot_type& slot) const;

      //! Connects a slot to the apply all signal.
      boost::signals2::connection connect_apply_all_signal(
        const apply_all_signal::slot_type& slot) const;

      //! Connects a slot to the save default signal.
      boost::signals2::connection connect_save_default_signal(
        const save_default_signal::slot_type& slot) const;

    private:
      mutable apply_signal m_apply_signal;
      mutable apply_all_signal m_apply_all_signal;
      mutable save_default_signal m_save_default_signal;
      time_and_sales_properties m_properties;
  };
}

#endif