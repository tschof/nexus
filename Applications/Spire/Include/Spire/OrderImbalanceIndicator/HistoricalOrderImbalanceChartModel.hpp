#ifndef SPIRE_HISTORICAL_ORDER_IMBALANCE_CHART_MODEL_HPP
#define SPIRE_HISTORICAL_ORDER_IMBALANCE_CHART_MODEL_HPP
#include "Nexus/Definitions/OrderImbalance.hpp"
#include "Spire/OrderImbalanceIndicator/OrderImbalanceIndicatorModel.hpp"

namespace Spire {

  //! Represents a model for loading order imbalances with specific data
  //! counts.
  class HistoricalOrderImbalanceChartModel {
    public:

      //! Signals that a new imbalance was published.
      using PublishedSignal = Signal<void (const Nexus::OrderImbalance&)>;

      //! Constructs a HistoricalOrderImbalanceChartModel with a source model.
      /*
        \param security The security to load order imbalances for.
        \param model The source order imbalance model.
      */
      HistoricalOrderImbalanceChartModel(const Nexus::Security& security,
        std::shared_ptr<OrderImbalanceIndicatorModel> model);

      //! Loads order imbalances for the model's security.
      /*
        \param interval The time interval to load.
      */
      QtPromise<std::vector<Nexus::OrderImbalance>> load(
        const TimeInterval& interval);

      //! Expands or contracts the loaded data in the given interval by the
      //! given count. A positive value expands the number of returned
      //! imbalances by count, and a negative value contracts the number of
      //! returned imbalances.
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

      //! If the last loaded interval includes the last published imbalance,
      //! signals that a new order imbalance was published.
      boost::signals2::connection connect_published_signal(
        const PublishedSignal::slot_type& slot) const;

    private:
      const Nexus::Security m_security;
      std::shared_ptr<OrderImbalanceIndicatorModel> m_model;
      mutable PublishedSignal m_published_signal;
      bool m_is_present_data_loaded;
  };
}

#endif
