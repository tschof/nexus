#include <catch.hpp>
#include "Nexus/Definitions/DefaultCountryDatabase.hpp"
#include "Spire/OrderImbalanceIndicator/FilteredOrderImbalanceIndicatorModel.hpp"
#include "Spire/OrderImbalanceIndicator/LocalOrderImbalanceIndicatorModel.hpp"
#include "Spire/Spire/QtPromise.hpp"
#include "Spire/SpireTester/SpireTester.hpp"

using namespace boost::posix_time;
using namespace Nexus;
using namespace Spire;

namespace {
  auto make_imbalance(const std::string& symbol, const ptime& timestamp) {
    return OrderImbalance(Security(symbol, 0), Side::BID, 100,
      Money(1 * Money::ONE), timestamp);
  }

  auto make_imbalance(const std::string& symbol, const MarketCode& market,
      Side side, Quantity size, double ref_price, const ptime& timestamp) {
    return OrderImbalance(Security(symbol, market, 0), side, size,
      Money(Quantity(ref_price)), timestamp);
  }

  auto market_db = GetDefaultMarketDatabase();

  const auto A = make_imbalance("A", market_db.FromDisplayName("TSX").m_code,
    Side::BID, 100, 1.0, from_time_t(100));
  const auto B = make_imbalance("B", market_db.FromDisplayName("TSX").m_code,
    Side::BID, 1000, 10.0, from_time_t(200));
  const auto C = make_imbalance("C", market_db.FromDisplayName("TSX").m_code,
    Side::ASK, 10000, 100.0, from_time_t(300));
  const auto D = make_imbalance("D", market_db.FromDisplayName("NYSE").m_code,
    Side::ASK, 100000, 1000.0, from_time_t(400));
  const auto E = make_imbalance("E", market_db.FromDisplayName("NYSE").m_code,
    Side::ASK, 1000000, 10000.0, from_time_t(500));

  const auto market_database = GetDefaultMarketDatabase();

  auto make_local_model() {
    auto model = std::make_shared<LocalOrderImbalanceIndicatorModel>();
    model->insert(A);
    model->insert(B);
    model->insert(C);
    model->insert(D);
    model->insert(E);
    return model;
  }
}

TEST_CASE("test_unfiltered_loading",
    "[FilteredOrderImbalanceIndicatorModel]") {
  run_test([] {
    auto model = FilteredOrderImbalanceIndicatorModel(make_local_model(), {});
    auto promise1 = model.load(TimeInterval::closed(
      from_time_t(100), from_time_t(300)));
    auto data1 = wait(std::move(promise1));
    REQUIRE(data1 == std::vector<OrderImbalance>({A, B, C}));
    auto promise2 = model.load(TimeInterval::closed(
      from_time_t(200), from_time_t(400)));
    auto data2 = wait(std::move(promise2));
    REQUIRE(data2 == std::vector<OrderImbalance>({B, C, D}));
    auto promise3 = model.load(TimeInterval::closed(from_time_t(300),
      from_time_t(500)));
    auto data3 = wait(std::move(promise3));
    REQUIRE(data3 == std::vector<OrderImbalance>({C, D, E}));
  }, "test_unfiltered_loading");
}

TEST_CASE("test_security_list_filter",
    "[FilteredOrderImbalanceIndicatorModel]") {
  run_test([] {
    auto model1 = FilteredOrderImbalanceIndicatorModel(make_local_model(),
      {make_security_list_filter({A.m_security})});
    auto promise1 = model1.load(TimeInterval::closed(
      from_time_t(0), from_time_t(500)));
    auto data1 = wait(std::move(promise1));
    REQUIRE(data1 == std::vector<OrderImbalance>({A}));
    auto model2 = FilteredOrderImbalanceIndicatorModel(make_local_model(),
      {make_security_list_filter({A.m_security, C.m_security, E.m_security})});
    auto promise2 = model2.load(TimeInterval::closed(from_time_t(0),
      from_time_t(500)));
    auto data2 = wait(std::move(promise2));
    REQUIRE(data2 == std::vector<OrderImbalance>({A, C, E}));
    auto model3 = FilteredOrderImbalanceIndicatorModel(make_local_model(),
      {make_security_list_filter({})});
    auto promise3 = model3.load(TimeInterval::closed(from_time_t(0),
      from_time_t(500)));
    auto data3 = wait(std::move(promise3));
    REQUIRE(data3.empty());
  }, "test_security_list_filter");
}

TEST_CASE("test_security_list_filter_with_duplicate_symbols",
    "[FilteredOrderImbalanceIndicatorModel]") {
  run_test([] {
    auto country_db = GetDefaultCountryDatabase();
    auto first_imbalance = OrderImbalance(Security("A",
      market_db.FromDisplayName("TSX").m_code,
      ParseCountryCode("CA", country_db)), Side::BID, 100,
      Money(Quantity(10)), from_time_t(100));
    auto second_imbalance = OrderImbalance(Security("A",
      market_db.FromDisplayName("NYSE").m_code,
      ParseCountryCode("US", country_db)), Side::BID, 100,
      Money(Quantity(10)), from_time_t(100));
    auto local_model = std::make_shared<LocalOrderImbalanceIndicatorModel>();
    local_model->insert(first_imbalance);
    local_model->insert(second_imbalance);
    auto filtered_model = FilteredOrderImbalanceIndicatorModel(local_model,
      {make_security_list_filter({first_imbalance.m_security})});
    auto promise = filtered_model.load(TimeInterval::closed(
      from_time_t(0), from_time_t(500)));
    auto data = wait(std::move(promise));
    REQUIRE(data == std::vector<OrderImbalance>({first_imbalance}));
  }, "test_security_list_filter_with_duplicate_symbols");
}

TEST_CASE("test_security_filter", "[FilteredOrderImbalanceIndicatorModel]") {
  run_test([] {
    auto model1 = FilteredOrderImbalanceIndicatorModel(make_local_model(),
      {make_symbol_filter("A")});
    auto promise1 = model1.load(TimeInterval::closed(
      from_time_t(0), from_time_t(500)));
    auto data1 = wait(std::move(promise1));
    REQUIRE(data1 == std::vector<OrderImbalance>({A}));
    auto model2 = FilteredOrderImbalanceIndicatorModel(make_local_model(),
      {make_symbol_filter("AZ")});
    auto promise2 = model2.load(TimeInterval::closed(from_time_t(0),
      from_time_t(500)));
    auto data2 = wait(std::move(promise2));
    REQUIRE(data2.empty());
    auto model3 = FilteredOrderImbalanceIndicatorModel(make_local_model(),
      {make_symbol_filter({""})});
    auto promise3 = model3.load(TimeInterval::closed(from_time_t(0),
      from_time_t(500)));
    auto data3 = wait(std::move(promise3));
    REQUIRE(data3 == std::vector<OrderImbalance>({A, B, C, D, E}));
  }, "test_security_filter");
}

TEST_CASE("test_market_list_filter",
    "[FilteredOrderImbalanceIndicatorModel]") {
  run_test([] {
    auto model1 = FilteredOrderImbalanceIndicatorModel(make_local_model(),
      {make_market_list_filter({"TSX"}, GetDefaultMarketDatabase())});
    auto promise1 = model1.load(TimeInterval::closed(
      from_time_t(0), from_time_t(500)));
    auto data1 = wait(std::move(promise1));
    REQUIRE(data1 == std::vector<OrderImbalance>({A, B, C}));
    auto model2 = FilteredOrderImbalanceIndicatorModel(make_local_model(),
      {make_market_list_filter({"TSX", "NYSE"}, GetDefaultMarketDatabase())});
    auto promise2 = model2.load(TimeInterval::closed(from_time_t(0),
      from_time_t(500)));
    auto data2 = wait(std::move(promise2));
    REQUIRE(data2 == std::vector<OrderImbalance>({A, B, C, D, E}));
    auto model3 = FilteredOrderImbalanceIndicatorModel(make_local_model(),
      {make_market_list_filter({}, GetDefaultMarketDatabase())});
    auto promise3 = model3.load(TimeInterval::closed(from_time_t(0),
      from_time_t(500)));
    auto data3 = wait(std::move(promise3));
    REQUIRE(data3.empty());
  }, "test_market_list_filter");
}

TEST_CASE("test_market_filter", "[FilteredOrderImbalanceIndicatorModel]") {
  run_test([] {
    auto model1 = FilteredOrderImbalanceIndicatorModel(make_local_model(),
      {make_market_filter("T", GetDefaultMarketDatabase())});
    auto promise1 = model1.load(TimeInterval::closed(from_time_t(0),
      from_time_t(500)));
    auto data1 = wait(std::move(promise1));
    REQUIRE(data1 == std::vector<OrderImbalance>({A, B, C}));
    auto model2 = FilteredOrderImbalanceIndicatorModel(make_local_model(),
      {make_market_filter("N", GetDefaultMarketDatabase())});
    auto promise2 = model2.load(TimeInterval::closed(from_time_t(0),
      from_time_t(500)));
    auto data2 = wait(std::move(promise2));
    REQUIRE(data2 == std::vector<OrderImbalance>({D, E}));
    auto model3 = FilteredOrderImbalanceIndicatorModel(make_local_model(),
      {make_market_filter("", GetDefaultMarketDatabase())});
    auto promise3 = model3.load(TimeInterval::closed(from_time_t(0),
      from_time_t(500)));
    auto data3 = wait(std::move(promise3));
    REQUIRE(data3 == std::vector<OrderImbalance>({A, B, C, D, E}));
  }, "test_market_filter");
}

TEST_CASE("test_side_filter", "[FilteredOrderImbalanceIndicatorModel]") {
  run_test([] {
    auto model1 = FilteredOrderImbalanceIndicatorModel(make_local_model(),
      {make_side_filter(Side::BID)});
    auto promise1 = model1.load(TimeInterval::closed(from_time_t(0),
      from_time_t(500)));
    auto data1 = wait(std::move(promise1));
    REQUIRE(data1 == std::vector<OrderImbalance>({A, B}));
    auto model2 = FilteredOrderImbalanceIndicatorModel(make_local_model(),
      {make_side_filter(Side::ASK)});
    auto promise2 = model2.load(TimeInterval::closed(from_time_t(0),
      from_time_t(500)));
    auto data2 = wait(std::move(promise2));
    REQUIRE(data2 == std::vector<OrderImbalance>({C, D, E}));
  }, "test_side_filter");
}

TEST_CASE("test_size_filter", "[FilteredOrderImbalanceIndicatorModel]") {
  run_test([] {
    auto model1 = FilteredOrderImbalanceIndicatorModel(make_local_model(),
      {make_size_filter({0}, {500})});
    auto promise1 = model1.load(TimeInterval::closed(from_time_t(0),
      from_time_t(500)));
    auto data1 = wait(std::move(promise1));
    REQUIRE(data1 == std::vector<OrderImbalance>({A}));
    auto model2 = FilteredOrderImbalanceIndicatorModel(make_local_model(),
      {make_size_filter({0}, {10000})});
    auto promise2 = model2.load(TimeInterval::closed(from_time_t(0),
      from_time_t(500)));
    auto data2 = wait(std::move(promise2));
    REQUIRE(data2 == std::vector<OrderImbalance>({A, B, C}));
    auto model3 = FilteredOrderImbalanceIndicatorModel(make_local_model(),
      {make_size_filter({100000000}, {100000000})});
    auto promise3 = model3.load(TimeInterval::closed(from_time_t(0),
      from_time_t(500)));
    auto data3 = wait(std::move(promise3));
    REQUIRE(data3.empty());
  }, "test_size_filter");
}

TEST_CASE("test_reference_price_filter",
    "[FilteredOrderImbalanceIndicatorModel]") {
  run_test([] {
    auto model1 = FilteredOrderImbalanceIndicatorModel(make_local_model(),
      {make_reference_price_filter(Money(Money::ZERO), Money(Money::ONE))});
    auto promise1 = model1.load(TimeInterval::closed(from_time_t(0),
      from_time_t(500)));
    auto data1 = wait(std::move(promise1));
    REQUIRE(data1 == std::vector<OrderImbalance>({A}));
    auto model2 = FilteredOrderImbalanceIndicatorModel(make_local_model(),
      {make_reference_price_filter(Money(Money::ZERO),
      Money(100 * Money::ONE))});
    auto promise2 = model2.load(TimeInterval::closed(from_time_t(0),
      from_time_t(500)));
    auto data2 = wait(std::move(promise2));
    REQUIRE(data2 == std::vector<OrderImbalance>({A, B, C}));
    auto model3 = FilteredOrderImbalanceIndicatorModel(make_local_model(),
      {make_reference_price_filter(Money(100000000 * Money::ONE),
      Money(100000000 * Money::ONE))});
    auto promise3 = model3.load(TimeInterval::closed(from_time_t(0),
      from_time_t(500)));
    auto data3 = wait(std::move(promise3));
    REQUIRE(data3.empty());
  }, "test_reference_price_filter");
}

TEST_CASE("test_notional_value_filter",
    "[FilteredOrderImbalanceIndicatorModel]") {
  run_test([] {
    auto model1 = FilteredOrderImbalanceIndicatorModel(make_local_model(),
      {make_notional_value_filter(Money(Money::ONE), Money(100 * Money::ONE))});
    auto promise1 = model1.load(TimeInterval::closed(from_time_t(0),
      from_time_t(500)));
    auto data1 = wait(std::move(promise1));
    REQUIRE(data1 == std::vector<OrderImbalance>({A}));
    auto model2 = FilteredOrderImbalanceIndicatorModel(make_local_model(),
      {make_notional_value_filter(Money(Money::ONE),
      Money(1000000 * Money::ONE))});
    auto promise2 = model2.load(TimeInterval::closed(from_time_t(0),
      from_time_t(500)));
    auto data2 = wait(std::move(promise2));
    REQUIRE(data2 == std::vector<OrderImbalance>({A, B, C}));
    auto model3 = FilteredOrderImbalanceIndicatorModel(make_local_model(),
      {make_notional_value_filter(Money(Money::ONE), Money(10 * Money::ONE))});
    auto promise3 = model3.load(TimeInterval::closed(from_time_t(0),
      from_time_t(500)));
    auto data3 = wait(std::move(promise3));
    REQUIRE(data3.empty());
  }, "test_notional_value_filter");
}

TEST_CASE("test_unfiltered_signals",
    "[FilteredOrderImbalanceIndicatorModel]") {
  run_test([] {
    auto local_model = std::make_shared<LocalOrderImbalanceIndicatorModel>();
    auto model = FilteredOrderImbalanceIndicatorModel(local_model, {});
    auto slot_data = OrderImbalance();
    model.subscribe([&] (const auto& imbalance) { slot_data = imbalance; });
    local_model->publish(A);
    REQUIRE(slot_data == A);
    local_model->publish(B);
    REQUIRE(slot_data == B);
  }, "test_unfiltered_signals");
}

TEST_CASE("test_filtered_signals",
    "[FilteredOrderImbalanceIndicatorModel]") {
  run_test([] {
    auto local_model = std::make_shared<LocalOrderImbalanceIndicatorModel>();
    auto model = FilteredOrderImbalanceIndicatorModel(local_model,
      {make_market_list_filter({"TSX"}, GetDefaultMarketDatabase())});
    auto signal_data = OrderImbalance();
    auto [connection, promise] = model.subscribe([&] (auto& i) {
      signal_data = i; });
    wait(std::move(promise));
    REQUIRE(signal_data == OrderImbalance());
    local_model->publish(A);
    REQUIRE(signal_data == A);
    local_model->publish(B);
    REQUIRE(signal_data == B);
    local_model->publish(C);
    REQUIRE(signal_data == C);
    local_model->publish(D);
    REQUIRE(signal_data == C);
    local_model->publish(E);
    REQUIRE(signal_data == C);
  }, "test_filtered_signals");
}

// TODO: remove this
#include <mutex>

class TestOrderImbalanceIndicatorModel :
    public OrderImbalanceIndicatorModel {
  public:

    //! Stores a request to load from an OrderImbalanceModel.
    class LoadEntry {
      public:

        //! Constructs a LoadEntry.
        /*
          \param interval The time interval to load.
        */
        LoadEntry(const TimeInterval& interval);

        //! Returns the load entry's requested time interval.
        const TimeInterval& get_interval() const;

        //! Sets the result of the load operation.
        /*
          \param result The list of imbalances that the promise loading the
                  specified range should evalute to.
        */
        void set_result(std::vector<Nexus::OrderImbalance> result);

        //! Returns the order imbalances to load.
        std::vector<Nexus::OrderImbalance>& get_result();

      private:
        friend class TestOrderImbalanceIndicatorModel;
        mutable Beam::Threading::Mutex m_mutex;
        TimeInterval m_interval;
        bool m_is_loaded;
        std::vector<Nexus::OrderImbalance> m_result;
        Beam::Threading::ConditionVariable m_load_condition;
    };

    QtPromise<std::vector<Nexus::OrderImbalance>> load(
      const TimeInterval& interval) override;

    SubscriptionResult<boost::optional<Nexus::OrderImbalance>>
      subscribe(const OrderImbalanceSignal::slot_type& slot) override;

    std::shared_ptr<OrderImbalanceChartModel> get_chart_model(
      const Nexus::Security& security) override;

    //! Pops the oldest load request from this model's load
    //! operation stack.
    QtPromise<std::shared_ptr<LoadEntry>> pop_load();

    //! Returns the number of pending load entries.
    int get_load_entry_count() const;

  private:
    Beam::Threading::Mutex m_mutex;
    Beam::Threading::ConditionVariable m_load_condition;
    std::deque<std::shared_ptr<LoadEntry>> m_load_entries;
};

TestOrderImbalanceIndicatorModel::LoadEntry::LoadEntry(
  const TimeInterval& interval)
  : m_interval(interval),
    m_is_loaded(false) {}

const TimeInterval&
    TestOrderImbalanceIndicatorModel::LoadEntry::get_interval() const {
  return m_interval;
}

void TestOrderImbalanceIndicatorModel::LoadEntry::set_result(
    std::vector<OrderImbalance> result) {
  auto lock = std::lock_guard(m_mutex);
  m_is_loaded = true;
  m_load_condition.notify_one();
  m_result = std::move(result);
}

std::vector<Nexus::OrderImbalance>&
    TestOrderImbalanceIndicatorModel::LoadEntry::get_result() {
  return m_result;
}

QtPromise<std::vector<Nexus::OrderImbalance>>
    TestOrderImbalanceIndicatorModel::load(
    const TimeInterval& interval) {
  auto load_entry = std::make_shared<LoadEntry>(interval);
  {
    auto lock = std::lock_guard(m_mutex);
    m_load_entries.push_back(load_entry);
    m_load_condition.notify_all();
  }
  return QtPromise([=] {
    auto lock = std::unique_lock(load_entry->m_mutex);
    while(!load_entry->m_is_loaded) {
      load_entry->m_load_condition.wait(lock);
    }
    return std::move(load_entry->get_result());
  }, LaunchPolicy::ASYNC);
}

SubscriptionResult<boost::optional<Nexus::OrderImbalance>>
    TestOrderImbalanceIndicatorModel::subscribe(
    const OrderImbalanceSignal::slot_type& slot) {
  return {boost::signals2::connection(), QtPromise([] {
    return boost::optional<OrderImbalance>();
  })};
}

std::shared_ptr<OrderImbalanceChartModel>
    TestOrderImbalanceIndicatorModel::get_chart_model(
    const Nexus::Security& security) {
  throw std::runtime_error("method not implemented");
}

QtPromise<std::shared_ptr<TestOrderImbalanceIndicatorModel::LoadEntry>>
    TestOrderImbalanceIndicatorModel::pop_load() {
  return QtPromise([=] {
    auto lock = std::unique_lock(m_mutex);
    while(m_load_entries.empty()) {
      m_load_condition.wait(lock);
    }
    auto entry = m_load_entries.front();
    m_load_entries.pop_front();
    return entry;
  }, LaunchPolicy::ASYNC);
}

int TestOrderImbalanceIndicatorModel::get_load_entry_count() const {
  return static_cast<int>(m_load_entries.size());
}

TEST_CASE("test_filtered_model_crashes_when_filtered_model_destroyed",
    "[FilteredOrderImbalanceIndicatorModel]") {
  run_test([] {
    auto T = std::make_shared<TestOrderImbalanceIndicatorModel>();
    auto F = std::make_unique<FilteredOrderImbalanceIndicatorModel>(T,
      std::vector<FilteredOrderImbalanceIndicatorModel::Filter>());
    F->load(TimeInterval::closed(from_time_t(0), from_time_t(1000)));
    F.reset();
    auto O = wait(T->pop_load());
    O->set_result({});
    O->get_result();
  }, "test_filtered_model_crashes_when_filtered_model_destroyed");
}

TEST_CASE("asdf", "[FilteredOrderImbalanceIndicatorModel]") {
  run_test([] {
    auto L = std::make_shared<LocalOrderImbalanceIndicatorModel>();
    auto F = std::make_unique<FilteredOrderImbalanceIndicatorModel>(L,
      std::vector<FilteredOrderImbalanceIndicatorModel::Filter>());
    F->subscribe([=] (const auto& imbalance) {});
    F.reset();
    F->subscribe([=] (const auto& imbalance) {});
    L->publish(A);
  }, "asfd");
}
