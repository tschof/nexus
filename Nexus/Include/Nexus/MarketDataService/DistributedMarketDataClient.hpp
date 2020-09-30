#ifndef NEXUS_DISTRIBUTED_MARKET_DATA_CLIENT_HPP
#define NEXUS_DISTRIBUTED_MARKET_DATA_CLIENT_HPP
#include <unordered_map>
#include <boost/range/adaptor/map.hpp>
#include <Beam/IO/OpenState.hpp>
#include "Nexus/Definitions/Country.hpp"
#include "Nexus/MarketDataService/MarketDataService.hpp"
#include "Nexus/MarketDataService/VirtualMarketDataClient.hpp"

namespace Nexus::MarketDataService {

  /**
   * Implements a MarketDataClient whose servers are distributed among
   * multiple instances.
   */
  class DistributedMarketDataClient {
    public:

      /**
       * Constructs a DistributedMarketDataClient.
       * @param countryToMarketDataClients Maps CountryCodes to
       *        MarketDataClients.
       * @param marketToMarketDataClients Maps MarketCodes to MarketDataClients.
       */
      DistributedMarketDataClient(std::unordered_map<
        CountryCode, std::shared_ptr<VirtualMarketDataClient>>
        countryToMarketDataClients, std::unordered_map<
        MarketCode, std::shared_ptr<VirtualMarketDataClient>>
        marketToMarketDataClients);

      ~DistributedMarketDataClient();

      void QueryOrderImbalances(const MarketWideDataQuery& query,
        Beam::ScopedQueueWriter<SequencedOrderImbalance> queue);

      void QueryOrderImbalances(const MarketWideDataQuery& query,
        Beam::ScopedQueueWriter<OrderImbalance> queue);

      void QueryBboQuotes(const SecurityMarketDataQuery& query,
        Beam::ScopedQueueWriter<SequencedBboQuote> queue);

      void QueryBboQuotes(const SecurityMarketDataQuery& query,
        Beam::ScopedQueueWriter<BboQuote> queue);

      void QueryBookQuotes(const SecurityMarketDataQuery& query,
        Beam::ScopedQueueWriter<SequencedBookQuote> queue);

      void QueryBookQuotes(const SecurityMarketDataQuery& query,
        Beam::ScopedQueueWriter<BookQuote> queue);

      void QueryMarketQuotes(const SecurityMarketDataQuery& query,
        Beam::ScopedQueueWriter<SequencedMarketQuote> queue);

      void QueryMarketQuotes(const SecurityMarketDataQuery& query,
        Beam::ScopedQueueWriter<MarketQuote> queue);

      void QueryTimeAndSales(const SecurityMarketDataQuery& query,
        Beam::ScopedQueueWriter<SequencedTimeAndSale> queue);

      void QueryTimeAndSales(const SecurityMarketDataQuery& query,
        Beam::ScopedQueueWriter<TimeAndSale> queue);

      SecuritySnapshot LoadSecuritySnapshot(const Security& security);

      SecurityTechnicals LoadSecurityTechnicals(const Security& security);

      boost::optional<SecurityInfo> LoadSecurityInfo(const Security& security);

      std::vector<SecurityInfo> LoadSecurityInfoFromPrefix(
        const std::string& prefix);

      void Close();

    private:
      std::unordered_map<CountryCode, std::shared_ptr<VirtualMarketDataClient>>
        m_countryToMarketDataClients;
      std::unordered_map<MarketCode, std::shared_ptr<VirtualMarketDataClient>>
        m_marketToMarketDataClients;
      VirtualMarketDataClient* FindMarketDataClient(MarketCode market);
      VirtualMarketDataClient* FindMarketDataClient(const Security& security);
      Beam::IO::OpenState m_openState;

      DistributedMarketDataClient(const DistributedMarketDataClient&) = delete;
      DistributedMarketDataClient& operator =(
        const DistributedMarketDataClient&) = delete;
  };

  inline DistributedMarketDataClient::DistributedMarketDataClient(
    std::unordered_map<CountryCode, std::shared_ptr<VirtualMarketDataClient>>
    countryToMarketDataClients, std::unordered_map<MarketCode,
    std::shared_ptr<VirtualMarketDataClient>> marketToMarketDataClients)
    : m_countryToMarketDataClients(std::move(countryToMarketDataClients)),
      m_marketToMarketDataClients(std::move(marketToMarketDataClients)) {}

  inline DistributedMarketDataClient::~DistributedMarketDataClient() {
    Close();
  }

  inline void DistributedMarketDataClient::QueryOrderImbalances(
      const MarketWideDataQuery& query,
      Beam::ScopedQueueWriter<SequencedOrderImbalance> queue) {
    if(auto marketDataClient = FindMarketDataClient(query.GetIndex())) {
      marketDataClient->QueryOrderImbalances(query, std::move(queue));
    }
  }

  inline void DistributedMarketDataClient::QueryOrderImbalances(
      const MarketWideDataQuery& query,
      Beam::ScopedQueueWriter<OrderImbalance> queue) {
    if(auto marketDataClient = FindMarketDataClient(query.GetIndex())) {
      marketDataClient->QueryOrderImbalances(query, std::move(queue));
    }
  }

  inline void DistributedMarketDataClient::QueryBboQuotes(
      const SecurityMarketDataQuery& query,
      Beam::ScopedQueueWriter<SequencedBboQuote> queue) {
    if(auto marketDataClient = FindMarketDataClient(query.GetIndex())) {
      marketDataClient->QueryBboQuotes(query, std::move(queue));
    }
  }

  inline void DistributedMarketDataClient::QueryBboQuotes(
      const SecurityMarketDataQuery& query,
      Beam::ScopedQueueWriter<BboQuote> queue) {
    if(auto marketDataClient = FindMarketDataClient(query.GetIndex())) {
      marketDataClient->QueryBboQuotes(query, std::move(queue));
    }
  }

  inline void DistributedMarketDataClient::QueryBookQuotes(
      const SecurityMarketDataQuery& query,
      Beam::ScopedQueueWriter<SequencedBookQuote> queue) {
    if(auto marketDataClient = FindMarketDataClient(query.GetIndex())) {
      marketDataClient->QueryBookQuotes(query, std::move(queue));
    }
  }

  inline void DistributedMarketDataClient::QueryBookQuotes(
      const SecurityMarketDataQuery& query,
      Beam::ScopedQueueWriter<BookQuote> queue) {
    if(auto marketDataClient = FindMarketDataClient(query.GetIndex())) {
      marketDataClient->QueryBookQuotes(query, std::move(queue));
    }
  }

  inline void DistributedMarketDataClient::QueryMarketQuotes(
      const SecurityMarketDataQuery& query,
      Beam::ScopedQueueWriter<SequencedMarketQuote> queue) {
    if(auto marketDataClient = FindMarketDataClient(query.GetIndex())) {
      marketDataClient->QueryMarketQuotes(query, std::move(queue));
    }
  }

  inline void DistributedMarketDataClient::QueryMarketQuotes(
      const SecurityMarketDataQuery& query,
      Beam::ScopedQueueWriter<MarketQuote> queue) {
    if(auto marketDataClient = FindMarketDataClient(query.GetIndex())) {
      marketDataClient->QueryMarketQuotes(query, std::move(queue));
    }
  }

  inline void DistributedMarketDataClient::QueryTimeAndSales(
      const SecurityMarketDataQuery& query,
      Beam::ScopedQueueWriter<SequencedTimeAndSale> queue) {
    if(auto marketDataClient = FindMarketDataClient(query.GetIndex())) {
      marketDataClient->QueryTimeAndSales(query, std::move(queue));
    }
  }

  inline void DistributedMarketDataClient::QueryTimeAndSales(
      const SecurityMarketDataQuery& query,
      Beam::ScopedQueueWriter<TimeAndSale> queue) {
    if(auto marketDataClient = FindMarketDataClient(query.GetIndex())) {
      marketDataClient->QueryTimeAndSales(query, std::move(queue));
    }
  }

  inline SecuritySnapshot DistributedMarketDataClient::LoadSecuritySnapshot(
      const Security& security) {
    if(auto marketDataClient = FindMarketDataClient(security)) {
      return marketDataClient->LoadSecuritySnapshot(security);
    }
    return SecuritySnapshot();
  }

  inline SecurityTechnicals DistributedMarketDataClient::
      LoadSecurityTechnicals(const Security& security) {
    if(auto marketDataClient = FindMarketDataClient(security)) {
      return marketDataClient->LoadSecurityTechnicals(security);
    }
    return SecurityTechnicals();
  }

  inline boost::optional<SecurityInfo>
      DistributedMarketDataClient::LoadSecurityInfo(const Security& security) {
    if(auto marketDataClient = FindMarketDataClient(security)) {
      return marketDataClient->LoadSecurityInfo(security);
    }
    return boost::none;
  }

  inline std::vector<SecurityInfo> DistributedMarketDataClient::
      LoadSecurityInfoFromPrefix(const std::string& prefix) {
    auto securityInfos = std::vector<SecurityInfo>();
    auto clients =
      std::unordered_set<std::shared_ptr<VirtualMarketDataClient>>();
    for(auto& client :
        m_countryToMarketDataClients | boost::adaptors::map_values) {
      clients.insert(client);
    }
    for(auto& client : clients) {
      auto result = client->LoadSecurityInfoFromPrefix(prefix);
      securityInfos.insert(securityInfos.end(), result.begin(), result.end());
    }
    return securityInfos;
  }

  inline void DistributedMarketDataClient::Close() {
    if(m_openState.SetClosing()) {
      return;
    }
    auto clients = std::vector<std::shared_ptr<VirtualMarketDataClient>>();
    for(auto& client :
        m_countryToMarketDataClients | boost::adaptors::map_values) {
      clients.push_back(client);
    }
    for(auto& client :
        m_marketToMarketDataClients | boost::adaptors::map_values) {
      clients.push_back(client);
    }
    while(!clients.empty()) {
      if(clients.back().use_count() == 2) {
        clients.back()->Close();
      }
      clients.pop_back();
    }
    m_openState.Close();
  }

  inline VirtualMarketDataClient* DistributedMarketDataClient::
      FindMarketDataClient(MarketCode market) {
    auto marketDataClientIterator = m_marketToMarketDataClients.find(market);
    if(marketDataClientIterator == m_marketToMarketDataClients.end()) {
      return nullptr;
    }
    return marketDataClientIterator->second.get();
  }

  inline VirtualMarketDataClient* DistributedMarketDataClient::
      FindMarketDataClient(const Security& security) {
    auto marketDataClientIterator = m_countryToMarketDataClients.find(
      security.GetCountry());
    if(marketDataClientIterator == m_countryToMarketDataClients.end()) {
      return FindMarketDataClient(security.GetMarket());
    }
    return marketDataClientIterator->second.get();
  }
}

#endif
