#ifndef NEXUS_BACKTESTERMARKETDATACLIENT_HPP
#define NEXUS_BACKTESTERMARKETDATACLIENT_HPP
#include <Beam/IO/OpenState.hpp>
#include <Beam/Pointers/Ref.hpp>
#include <boost/noncopyable.hpp>
#include "Nexus/Backtester/Backtester.hpp"
#include "Nexus/Backtester/BacktesterMarketDataService.hpp"
#include "Nexus/MarketDataService/VirtualMarketDataClient.hpp"

namespace Nexus {

  /*! \class BacktesterMarketDataClient
      \brief Implements a MarketDataClient used for backtesting.
   */
  class BacktesterMarketDataClient : private boost::noncopyable {
    public:

      //! Constructs a BacktesterMarketDataClient.
      /*!
        \param service The BacktesterMarketDataService to connect to.
        \param marketDataClient The underlying MarketDataClient to submit
               queries to.
      */
      BacktesterMarketDataClient(
        Beam::RefType<BacktesterMarketDataService> service,
        std::unique_ptr<MarketDataService::VirtualMarketDataClient>
        marketDataClient);

      ~BacktesterMarketDataClient();

      void QueryOrderImbalances(
        const MarketDataService::MarketWideDataQuery& query,
        const std::shared_ptr<
        Beam::QueueWriter<SequencedOrderImbalance>>& queue);

      void QueryOrderImbalances(
        const MarketDataService::MarketWideDataQuery& query,
        const std::shared_ptr<Beam::QueueWriter<OrderImbalance>>& queue);

      void QueryBboQuotes(
        const MarketDataService::SecurityMarketDataQuery& query,
        const std::shared_ptr<Beam::QueueWriter<SequencedBboQuote>>& queue);

      void QueryBboQuotes(
        const MarketDataService::SecurityMarketDataQuery& query,
        const std::shared_ptr<Beam::QueueWriter<BboQuote>>& queue);

      void QueryBookQuotes(
        const MarketDataService::SecurityMarketDataQuery& query,
        const std::shared_ptr<
        Beam::QueueWriter<SequencedBookQuote>>& queue);

      void QueryBookQuotes(
        const MarketDataService::SecurityMarketDataQuery& query,
        const std::shared_ptr<Beam::QueueWriter<BookQuote>>& queue);

      void QueryMarketQuotes(
        const MarketDataService::SecurityMarketDataQuery& query,
        const std::shared_ptr<
        Beam::QueueWriter<SequencedMarketQuote>>& queue);

      void QueryMarketQuotes(
        const MarketDataService::SecurityMarketDataQuery& query,
        const std::shared_ptr<Beam::QueueWriter<MarketQuote>>& queue);

      void QueryTimeAndSales(
        const MarketDataService::SecurityMarketDataQuery& query,
        const std::shared_ptr<
        Beam::QueueWriter<SequencedTimeAndSale>>& queue);

      void QueryTimeAndSales(
        const MarketDataService::SecurityMarketDataQuery& query,
        const std::shared_ptr<Beam::QueueWriter<TimeAndSale>>& queue);

      MarketDataService::SecuritySnapshot LoadSecuritySnapshot(
        const Security& security);

      SecurityTechnicals LoadSecurityTechnicals(
        const Security& security);

      std::vector<SecurityInfo> LoadSecurityInfoFromPrefix(
        const std::string& prefix);

      void Open();

      void Close();

    private:
      BacktesterMarketDataService* m_service;
      std::unique_ptr<MarketDataService::VirtualMarketDataClient>
        m_marketDataClient;
      Beam::IO::OpenState m_openState;

      void Shutdown();
  };

  inline BacktesterMarketDataClient::BacktesterMarketDataClient(
      Beam::RefType<BacktesterMarketDataService> service,
      std::unique_ptr<MarketDataService::VirtualMarketDataClient>
      marketDataClient)
      : m_service{service.Get()},
        m_marketDataClient{std::move(marketDataClient)} {}

  inline BacktesterMarketDataClient::~BacktesterMarketDataClient() {
    Close();
  }

  inline void BacktesterMarketDataClient::QueryOrderImbalances(
      const MarketDataService::MarketWideDataQuery& query,
      const std::shared_ptr<
      Beam::QueueWriter<SequencedOrderImbalance>>& queue) {}

  inline void BacktesterMarketDataClient::QueryOrderImbalances(
      const MarketDataService::MarketWideDataQuery& query,
      const std::shared_ptr<Beam::QueueWriter<OrderImbalance>>& queue) {}

  inline void BacktesterMarketDataClient::QueryBboQuotes(
      const MarketDataService::SecurityMarketDataQuery& query,
      const std::shared_ptr<Beam::QueueWriter<SequencedBboQuote>>& queue) {
    m_service->QueryBboQuotes(query);
    m_marketDataClient->QueryBboQuotes(query, queue);
  }

  inline void BacktesterMarketDataClient::QueryBboQuotes(
      const MarketDataService::SecurityMarketDataQuery& query,
      const std::shared_ptr<Beam::QueueWriter<BboQuote>>& queue) {
    m_service->QueryBboQuotes(query);
    m_marketDataClient->QueryBboQuotes(query, queue);
  }

  inline void BacktesterMarketDataClient::QueryBookQuotes(
      const MarketDataService::SecurityMarketDataQuery& query,
      const std::shared_ptr<Beam::QueueWriter<SequencedBookQuote>>& queue) {}

  inline void BacktesterMarketDataClient::QueryBookQuotes(
      const MarketDataService::SecurityMarketDataQuery& query,
      const std::shared_ptr<Beam::QueueWriter<BookQuote>>& queue) {}

  inline void BacktesterMarketDataClient::QueryMarketQuotes(
      const MarketDataService::SecurityMarketDataQuery& query,
      const std::shared_ptr<
      Beam::QueueWriter<SequencedMarketQuote>>& queue) {}

  inline void BacktesterMarketDataClient::QueryMarketQuotes(
      const MarketDataService::SecurityMarketDataQuery& query,
      const std::shared_ptr<Beam::QueueWriter<MarketQuote>>& queue) {}

  inline void BacktesterMarketDataClient::QueryTimeAndSales(
      const MarketDataService::SecurityMarketDataQuery& query,
      const std::shared_ptr<Beam::QueueWriter<SequencedTimeAndSale>>& queue) {}

  inline void BacktesterMarketDataClient::QueryTimeAndSales(
      const MarketDataService::SecurityMarketDataQuery& query,
      const std::shared_ptr<Beam::QueueWriter<TimeAndSale>>& queue) {}

  inline MarketDataService::SecuritySnapshot
      BacktesterMarketDataClient::LoadSecuritySnapshot(
      const Security& security) {
    return m_marketDataClient->LoadSecuritySnapshot(security);
  }

  inline SecurityTechnicals BacktesterMarketDataClient::LoadSecurityTechnicals(
      const Security& security) {
    return m_marketDataClient->LoadSecurityTechnicals(security);
  }

  inline std::vector<SecurityInfo> BacktesterMarketDataClient::
      LoadSecurityInfoFromPrefix(const std::string& prefix) {
    return m_marketDataClient->LoadSecurityInfoFromPrefix(prefix);
  }

  inline void BacktesterMarketDataClient::Open() {
    if(m_openState.SetOpening()) {
      return;
    }
    try {
      m_marketDataClient->Open();
    } catch(const std::exception&) {
      m_openState.SetOpenFailure();
      Shutdown();
    }
    m_openState.SetOpen();
  }

  inline void BacktesterMarketDataClient::Close() {
    if(m_openState.SetClosing()) {
      return;
    }
    Shutdown();
  }

  inline void BacktesterMarketDataClient::Shutdown() {
    m_marketDataClient->Close();
    m_openState.SetClosed();
  }
}

#endif