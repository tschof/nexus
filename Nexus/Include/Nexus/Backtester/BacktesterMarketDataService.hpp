#ifndef NEXUS_BACKTESTERMARKETDATASERVICE_HPP
#define NEXUS_BACKTESTERMARKETDATASERVICE_HPP
#include <memory>
#include <unordered_map>
#include <Beam/Pointers/Ref.hpp>
#include <Beam/Queries/Sequence.hpp>
#include <boost/noncopyable.hpp>
#include "Nexus/Backtester/Backtester.hpp"
#include "Nexus/Backtester/BacktesterEventHandler.hpp"
#include "Nexus/MarketDataService/SecurityMarketDataQuery.hpp"
#include "Nexus/MarketDataService/VirtualMarketDataClient.hpp"

namespace Nexus {
  class BacktesterMarketDataService : private boost::noncopyable {
    public:
      BacktesterMarketDataService(
        Beam::RefType<BacktesterEventHandler> eventHandler,
        Beam::RefType<MarketDataService::VirtualMarketDataClient>
        marketDataClient);

      void QueryBboQuotes(
        const MarketDataService::SecurityMarketDataQuery& query);

    private:
      friend class QueryBboQuotesEvent;
      BacktesterEventHandler* m_eventHandler;
      MarketDataService::VirtualMarketDataClient* m_marketDataClient;
  };

  class QueryBboQuotesEvent : public BacktesterEvent {
    public:
      QueryBboQuotesEvent(
        const MarketDataService::SecurityMarketDataQuery& query,
        Beam::RefType<BacktesterMarketDataService> marketDataClient);

      virtual void Execute() override;

    private:
      MarketDataService::SecurityMarketDataQuery m_query;
      BacktesterMarketDataService* m_service;
  };

  inline BacktesterMarketDataService::BacktesterMarketDataService(
      Beam::RefType<BacktesterEventHandler> eventHandler,
      Beam::RefType<MarketDataService::VirtualMarketDataClient>
      marketDataClient)
      : m_eventHandler{eventHandler.Get()},
        m_marketDataClient{marketDataClient.Get()} {}

  inline void BacktesterMarketDataService::QueryBboQuotes(
      const MarketDataService::SecurityMarketDataQuery& query) {
    auto event = std::make_shared<QueryBboQuotesEvent>(query,
      Beam::Ref(*this));
    m_eventHandler->Add(event);
  }

  inline QueryBboQuotesEvent::QueryBboQuotesEvent(
      const MarketDataService::SecurityMarketDataQuery& query,
      Beam::RefType<BacktesterMarketDataService> service)
      : BacktesterEvent{boost::posix_time::neg_infin},
        m_query{query},
        m_service{service.Get()} {}

  inline void QueryBboQuotesEvent::Execute() {
  }
}

#endif
