#ifndef SPIRE_ORDER_IMBALANCE_INDICATOR_PROPERTIES_HPP
#define SPIRE_ORDER_IMBALANCE_INDICATOR_PROPERTIES_HPP
#include "Nexus/Definitions/Security.hpp"

namespace Spire {

  //! Stores the properties for an order imbalance indicator window.
  class OrderImbalanceIndicatorProperties {
    public:

      //! The available columns to display.
      enum class COLUMN {

        //! The security column.
        SECURITY_COLUMN,

        //! The side column.
        SIDE_COLUMN,

        //! The size column.
        SIZE_COLUMN,

        //! The reference price column.
        REFERENCE_PX_COLUMN,

        //! THe date column.
        DATE_COLUMN,

        //! The time column.
        TIME_COLUMN,

        //! The notional value column.
        NOTIONAL_VALUE_COLUMN
      };

      static const auto COLUMN_COUNT = 7;

      //! Constructs a default set or properties.
      OrderImbalanceIndicatorProperties();

      //! Returns true if the given column is visible.
      /*
        \param column The column to get the visibility of.
      */
      bool get_column_visible(COLUMN column);

      //! Sets the visibility of the given column.
      /*
        \param column The column to set the visibility of.
        \param is_visible True if the column is visible, false otherwise.
      */
      void set_column_visible(COLUMN column, bool is_visible);

      //! Sets the security for which the historical imbalance chart will be
      //! dispalyed.
      /*
        \security The security to show the historical imbalance chart for.
      */
      void set_displayed_chart(const Nexus::Security& security);

      //! Resets the display status of the historical imbalance chart.
      void reset_displayed_chart();
  };
}

#endif
