#include "Spire/OrderImbalanceIndicator/CachedOrderImbalanceIndicatorModel.hpp"

using namespace Beam::Queries;
using namespace boost;
using namespace boost::icl;
using namespace boost::posix_time;
using namespace Nexus;
using namespace Spire;

CachedOrderImbalanceIndicatorModel::CachedOrderImbalanceIndicatorModel(
    std::shared_ptr<OrderImbalanceIndicatorModel> source)
    : m_source_model(std::move(source)) {
  auto [connection, promise] = m_source_model->subscribe(
    [=] (const auto& imbalance) {
      on_imbalance_published(imbalance);
    });
  m_subscription_connection = std::move(connection);
  m_subscription_promise = std::move(promise);
}

QtPromise<std::vector<OrderImbalance>>
    CachedOrderImbalanceIndicatorModel::load(const TimeInterval& interval) {
  if(boost::icl::contains(m_intervals, interval)) {
    return m_cache.load(interval);
  }
  return load_from_model(interval).then([=] (const auto& loaded_imbalances) {
    return m_cache.load(interval);
  });
}

QtPromise<std::vector<OrderImbalance>>
    CachedOrderImbalanceIndicatorModel::load(const Nexus::Security& security,
    const TimeInterval& interval) {
  if(boost::icl::contains(m_security_intervals[security], interval)) {
    return m_cache.load(security, interval);
  }
  return load_from_model(security, interval).then([=] (auto i) {
    return m_cache.load(security, interval);
  });
}

QtPromise<std::vector<OrderImbalance>>
    CachedOrderImbalanceIndicatorModel::load(const Security& security,
    const ptime& timestamp, const SnapshotLimit& limit) {
  if(limit.GetSize() == 0) {
    return QtPromise([] { return std::vector<OrderImbalance>(); });
  }
  auto info = LoadInfo{security, timestamp, limit, limit};
  if(boost::icl::contains(m_intervals, {timestamp, timestamp})) {
    return load_from_cache(info);
  }
  return load_with_limit(info);
}

SubscriptionResult<optional<Nexus::OrderImbalance>>
    CachedOrderImbalanceIndicatorModel::subscribe(
    const OrderImbalanceSignal::slot_type& slot) {
  return m_source_model->subscribe(slot);
}

QtPromise<std::vector<OrderImbalance>>
    CachedOrderImbalanceIndicatorModel::load_from_cache(const LoadInfo& info) {
  return m_cache.load(info.m_security, info.m_timestamp,
    info.m_requested_limit).then([=] (auto result) {
        auto imbalances = result.Get();
        if(imbalances.size() == info.m_requested_limit.GetSize() ||
            info.m_current_limit.GetSize() == 0) {
          return QtPromise<std::vector<OrderImbalance>>(
            [imbalances = std::move(imbalances)] {
                return std::move(imbalances);
              });
        }
        auto updated_info = info;
        updated_info.m_current_limit = {info.m_current_limit.GetType(),
          imbalances.size()};
        return load_with_limit(updated_info);
      });
}

// TODO: reorder declaration in alphabetical order
QtPromise<std::vector<OrderImbalance>>
    CachedOrderImbalanceIndicatorModel::load_with_limit(const LoadInfo& info) {
  return m_source_model->load(info.m_security, info.m_timestamp,
    info.m_requested_limit).then([=] (auto result) {
        auto imbalances = std::move(result.Get());
        auto loaded_interval = [&] {
            if(info.m_current_limit.GetType() == SnapshotLimit::Type::HEAD) {
              return TimeInterval(info.m_timestamp,
                imbalances.back().m_timestamp);
            }
            return TimeInterval(imbalances.front().m_timestamp,
              info.m_timestamp);
          }();
        auto updated_info = info;
        updated_info.m_current_limit = {updated_info.m_current_limit.GetType(),
          updated_info.m_current_limit.GetSize() - imbalances.size()};
        on_imbalances_loaded(info.m_security, {loaded_interval},
          std::move(imbalances));
        return load_from_cache(info);
      });
}

QtPromise<void> CachedOrderImbalanceIndicatorModel::load_from_model(
    const TimeInterval& interval) {
  auto unloaded_intervals = interval_set<ptime>(interval) - m_intervals;
  auto promises = std::vector<QtPromise<void>>();
  for(auto& interval : unloaded_intervals) {
    auto promise = m_source_model->load(TimeInterval::closed(interval.lower(),
      interval.upper())).then([=] (auto& imbalances) {
        on_imbalances_loaded(interval, imbalances.Get());

        // TODO:
        return QtPromise([] {});
      });
    promises.push_back(std::move(promise));
  }
  return all(std::move(promises));
}

QtPromise<void> CachedOrderImbalanceIndicatorModel::load_from_model(
    const Security& security, const TimeInterval& interval) {
  auto unloaded_intervals = interval_set<ptime>(interval) -
    m_security_intervals[security];
  auto promises = std::vector<QtPromise<void>>();
  for(const auto& interval : unloaded_intervals) {
    auto promise = m_source_model->load(security,
      TimeInterval::closed(interval.lower(), interval.upper())).then(
      [=] (const auto& imbalances) {
        on_imbalances_loaded(security, interval, imbalances.Get());

        //TODO:
        return QtPromise([] {});
      });
    promises.push_back(std::move(promise));
  }
  return all(std::move(promises));
}

void CachedOrderImbalanceIndicatorModel::on_imbalance_published(
    const OrderImbalance& imbalance) {
  auto interval = TimeInterval::closed(imbalance.m_timestamp,
    imbalance.m_timestamp);
  m_intervals.add(interval);
  m_security_intervals[imbalance.m_security].add(interval);
  m_cache.insert(imbalance);
}

void CachedOrderImbalanceIndicatorModel::on_imbalances_loaded(
    const TimeInterval& interval,
    const std::vector<Nexus::OrderImbalance>& imbalances) {
  for(auto& imbalance : imbalances) {
    if(!boost::icl::contains(m_security_intervals[imbalance.m_security],
        TimeInterval::closed(imbalance.m_timestamp, imbalance.m_timestamp))) {
      m_cache.insert(imbalance);
    }
  }
  for(auto& pair : m_security_intervals) {
    pair.second.add(interval);
  }
  m_intervals.add(interval);
}

void CachedOrderImbalanceIndicatorModel::on_imbalances_loaded(
    const Security& security, const TimeInterval& interval,
    const std::vector<OrderImbalance>& imbalances) {
  auto& intervals = m_security_intervals[security];
  if(intervals.empty()) {
    for(auto& imbalance : imbalances) {
      m_cache.insert(imbalance);
    }
  } else {
    if(!imbalances.empty()) {
      auto unique_intervals = interval_set<ptime>({
        imbalances.front().m_timestamp, imbalances.back().m_timestamp}) -
        intervals;
      for(auto& unique_interval : unique_intervals) {
        auto first = [&] {
          auto lower = unique_interval.lower();
          if(boost::icl::contains(intervals, TimeInterval::closed(lower,
              lower))) {
            return std::upper_bound(imbalances.begin(), imbalances.end(), lower,
              [] (const auto& timestamp, const auto& imbalance) {
                return imbalance.m_timestamp > timestamp;
              });
          }
          return imbalances.begin();
        }();
        auto last = [&] {
          auto upper =  unique_interval.upper();
          if(boost::icl::contains(intervals, TimeInterval::closed(upper,
              upper))) {
            return std::lower_bound(first, imbalances.end(), upper,
              [] (const auto& imbalance, const auto& timestamp) {
                return timestamp < imbalance.m_timestamp;
              });
          }
          return imbalances.end();
        }();
        if(first == last && !boost::icl::contains(intervals,
            TimeInterval::closed(first->m_timestamp, first->m_timestamp))) {
          m_cache.insert(std::move(*first));
        } else {
          for(auto i = first; i < last; ++i) {
            m_cache.insert(*i);
          }
        }
      }
    }
  }
  intervals.add(interval);
}
