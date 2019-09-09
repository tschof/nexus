#include "Nexus/BacktesterTests/BacktesterMarketDataClientTester.hpp"
#include <Beam/Threading/ConditionVariable.hpp>
#include <Beam/Threading/Mutex.hpp>
#include "Nexus/Backtester/BacktesterServiceClients.hpp"
#include "Nexus/MarketDataService/MarketDataService.hpp"
#include "Nexus/ServiceClients/TestServiceClients.hpp"
#include "Nexus/ServiceClients/VirtualServiceClients.hpp"

using namespace Beam;
using namespace Beam::Queries;
using namespace Beam::Threading;
using namespace boost;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace Nexus;
using namespace Nexus::MarketDataService;
using namespace Nexus::Tests;

void BacktesterMarketDataClientTester::TestRealTimeQuery() {
  auto startTime = ptime(date(2016, 5, 6), seconds(0));
  auto dataStore = std::make_shared<LocalHistoricalDataStore>();
  auto security = Security("TST", DefaultMarkets::NYSE(),
    DefaultCountries::US());
  auto COUNT = 6;
  for(auto i = 0; i < COUNT; ++i) {
    auto timestamp = startTime + seconds(i - 3);
    auto bboQuote = SequencedValue(IndexedValue(
      BboQuote(Quote(Money::ONE, 100, Side::BID),
      Quote(Money::ONE, 100, Side::ASK), timestamp), security),
      EncodeTimestamp(timestamp, Beam::Queries::Sequence(
      static_cast<Beam::Queries::Sequence::Ordinal>(i))));
    dataStore->Store(bboQuote);
  }
  auto testEnvironment = TestEnvironment(
    MakeVirtualHistoricalDataStore(dataStore));
  testEnvironment.Open();
  auto testServiceClients = MakeVirtualServiceClients<TestServiceClients>(
    Initialize(Ref(testEnvironment)));
  auto backtesterEnvironment = BacktesterEnvironment(startTime,
    Ref(*testServiceClients));
  backtesterEnvironment.Open();
  auto serviceClients = BacktesterServiceClients(Ref(backtesterEnvironment));
  serviceClients.Open();
  auto routines = RoutineTaskQueue();
  auto& marketDataClient = serviceClients.GetMarketDataClient();
  auto query = BuildRealTimeQuery(security);
  auto expectedTimestamp = startTime;
  auto finalTimestamp = startTime + seconds(COUNT - 4);
  auto queryCompleteMutex = Mutex();
  auto queryCompleteCondition = ConditionVariable();
  auto testSucceeded = boost::optional<bool>();
  marketDataClient.QueryBboQuotes(query, routines.GetSlot<SequencedBboQuote>(
    [&] (const auto& bboQuote) {
      auto lock = boost::lock_guard(queryCompleteMutex);
      if(bboQuote->m_timestamp != expectedTimestamp) {
        testSucceeded = false;
        queryCompleteCondition.notify_one();
      } else if(expectedTimestamp == finalTimestamp) {
        testSucceeded = true;
        queryCompleteCondition.notify_one();
      } else {
        expectedTimestamp = expectedTimestamp + seconds(1);
      }
    }));
  auto lock = boost::unique_lock(queryCompleteMutex);
  while(!testSucceeded.is_initialized()) {
    queryCompleteCondition.wait(lock);
  }
  CPPUNIT_ASSERT(*testSucceeded);
}

void BacktesterMarketDataClientTester::TestHistoricalQuery() {
  auto startTime = ptime(date(2016, 5, 6), seconds(0));
  auto dataStore = std::make_shared<LocalHistoricalDataStore>();
  auto security = Security("TST", DefaultMarkets::NYSE(),
    DefaultCountries::US());
  auto COUNT = 6;
  for(auto i = 0; i < COUNT; ++i) {
    auto timestamp = startTime + seconds(i - 3);
    auto bboQuote = SequencedValue(IndexedValue(
      BboQuote(Quote(Money::ONE, 100, Side::BID),
      Quote(Money::ONE, 100, Side::ASK), timestamp), security),
      EncodeTimestamp(timestamp, Beam::Queries::Sequence(
      static_cast<Beam::Queries::Sequence::Ordinal>(i))));
    dataStore->Store(bboQuote);
  }
  auto testEnvironment = TestEnvironment(
    MakeVirtualHistoricalDataStore(dataStore));
  testEnvironment.Open();
  auto testServiceClients = MakeVirtualServiceClients<TestServiceClients>(
    Initialize(Ref(testEnvironment)));
  auto backtesterEnvironment = BacktesterEnvironment(startTime,
    Ref(*testServiceClients));
  backtesterEnvironment.Open();
  auto serviceClients = BacktesterServiceClients(Ref(backtesterEnvironment));
  serviceClients.Open();
  auto& marketDataClient = serviceClients.GetMarketDataClient();
  auto snapshot = std::make_shared<Queue<SequencedBboQuote>>();
  auto query = SecurityMarketDataQuery();
  query.SetIndex(security);
  query.SetRange(Range::Historical());
  query.SetSnapshotLimit(SnapshotLimit::Unlimited());
  marketDataClient.QueryBboQuotes(query, snapshot);
  auto received = std::vector<SequencedBboQuote>();
  FlushQueue(snapshot, std::back_inserter(received));
  CPPUNIT_ASSERT(received.size() == 3);
  CPPUNIT_ASSERT(received[0]->m_timestamp == startTime - seconds(3));
  CPPUNIT_ASSERT(received[1]->m_timestamp == startTime - seconds(2));
  CPPUNIT_ASSERT(received[2]->m_timestamp == startTime - seconds(1));
}
