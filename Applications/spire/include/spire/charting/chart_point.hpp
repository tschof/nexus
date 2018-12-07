#ifndef SPIRE_CHART_POINT_HPP
#define SPIRE_CHART_POINT_HPP
#include "spire/charting/charting.hpp"
#include "spire/charting/chart_value.hpp"

namespace Spire {

  /** Stores a single point on a chart. */
  struct ChartPoint {

    //! The position on the x-axis.
    ChartValue m_x;

    //! The position on the y-axis.
    ChartValue m_y;

    //! Constructs a point at position (0, 0).
    ChartPoint() = default;

    //! Constructs a ChartPoint.
    /*!
      \param x The position on the x-axis.
      \param y The position on the y-axis.
    */
    ChartPoint(ChartValue x, ChartValue y);
  };
}

#endif
