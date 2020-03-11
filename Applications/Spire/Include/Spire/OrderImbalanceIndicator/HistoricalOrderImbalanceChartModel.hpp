#ifndef SPIRE_HISTORICAL_ORDER_IMBALANCE_CHART_MODEL_HPP
#define SPIRE_HISTORICAL_ORDER_IMBALANCE_CHART_MODEL_HPP
#include "Nexus/Definitions/OrderImbalance.hpp"
#include "Spire/OrderImbalanceIndicator/OrderImbalanceIndicatorModel.hpp"

namespace Spire {

  class HistoricalOrderImbalanceChartModel {
    public:

      using PublishedSignal = Signal<void (const Nexus::OrderImbalance&)>;

      //! Constructs a HistoricalOrderImbalanceChartModel with a source model.
      HistoricalOrderImbalanceChartModel(
        std::shared_ptr<OrderImbalanceIndicatorModel> model);

      //! Expands or contracts the data in the given interval by the given
      //! count. A positive value expands the number of returned imbalances
      //! by count, and a negative value contracts the number of returned
      //! imbalances.
      /*
        \param interval The interval whose data to expand or contract.
        \param count The amount to expand or contract the interval's data by.
      */
      QtPromise<std::vector<Nexus::OrderImbalance>> resize(
        const TimeInterval& interval, int count);
  
      //! Returns the data in the given interval shifted left or right by the
      //! given value. A positive value shifts the data to the right, and a
      //! negative value shifts the data to the left.
      /*
        \param interval The interval whose data to shift.
        \param value The value to shift the interval's data by.
      */
      QtPromise<std::vector<Nexus::OrderImbalance>> shift(
        const TimeInterval& interval, int value);

      //! Signals that an order imbalance was published.
      boost::signals2::connection connect_published_signal(
        const PublishedSignal::slot_type& slot) const;

    private:
      std::shared_ptr<OrderImbalanceIndicatorModel> m_model;
      mutable PublishedSignal m_published_signal;
  };
}

#endif
