#include "Spire/OrderImbalanceIndicator/HistoricalOrderImbalanceChartModel.hpp"

using namespace boost::signals2;
using namespace Nexus;
using namespace Spire;

HistoricalOrderImbalanceChartModel::HistoricalOrderImbalanceChartModel(
  std::shared_ptr<OrderImbalanceIndicatorModel> model)
  : m_model(std::move(model)) {}

QtPromise<std::vector<OrderImbalance>>
    HistoricalOrderImbalanceChartModel::resize(const TimeInterval& interval,
    int count) {

}

QtPromise<std::vector<OrderImbalance>>
    HistoricalOrderImbalanceChartModel::shift(const TimeInterval& interval,
    int value) {

}

connection HistoricalOrderImbalanceChartModel::connect_published_signal(
    const PublishedSignal::slot_type& slot) const {
  return m_published_signal.connect(slot);
}
