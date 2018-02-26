#ifndef NEXUS_REPLAY_MARKET_DATA_FEED_CLIENT_HPP
#define NEXUS_REPLAY_MARKET_DATA_FEED_CLIENT_HPP
#include <deque>
#include <Beam/IO/OpenState.hpp>
#include <Beam/Pointers/LocalPtr.hpp>
#include <Beam/Queues/RoutineTaskQueue.hpp>
#include <Beam/Threading/Timer.hpp>
#include <Beam/TimeService/TimeClient.hpp>
#include <Beam/Utilities/VariantLambdaVisitor.hpp>
#include <boost/noncopyable.hpp>
#include <boost/variant.hpp>
#include "Nexus/Definitions/Security.hpp"
#include "Nexus/MarketDataService/MarketDataFeedClient.hpp"

namespace Nexus {
namespace Details {
  struct SecurityVisitor : boost::static_visitor<const Security&> {
    const Security& operator ()(const SecurityBboQuote& message) const {
      return message.GetIndex();
    }

    template<typename T>
    const Security& operator ()(const T& message) const {
      throw std::runtime_error("Invalid message.");
    }
  };

  const Security& GetSecurity(
      const MarketDataService::MarketDataFeedMessage& message) {
    return boost::apply_visitor(SecurityVisitor(), message);
  }

  const boost::posix_time::ptime& GetTimestamp(
      const MarketDataService::MarketDataFeedMessage& message) {
    auto visitor = [] (const auto& value) {
      return value->m_timestamp;
    };
    return boost::apply_visitor(visitor, message);
  }
}

  /*! \class ReplayMarketDataFeedClient
      \brief Replays market data from a MySQL dump to the MarketDataFeedServer.
      \tparam MarketDataFeedClientType The type of MarketDataFeedClient
              connected to the MarketDataFeedServer.
      \tparam TimeClientType The type of TimeClient used for timestamps.
      \tparam TimerType Used to manage successive time increments.
   */
  template<typename MarketDataFeedClientType, typename TimeClientType,
    typename TimerType>
  class ReplayMarketDataFeedClient : private boost::noncopyable {
    public:

      //! The type of MarketDataFeedClient connected to the
      //! MarketDataFeedServer.
      using MarketDataFeedClient = Beam::GetTryDereferenceType<
        MarketDataFeedClientType>;

      //! The type of TimeClient used for timestamps.
      using TimeClient = Beam::GetTryDereferenceType<TimeClientType>;

      //! Used to manage successive time increments.
      using Timer = Beam::GetTryDereferenceType<TimerType>;

      //! Constructs a ReplayMarketDataFeedClient.
      /*!
        \param marketDataFeedClient Initializes the MarketDataFeedClient.
        \param timeClient Initializes the TimeClient.
        \param timer Initializes the Timer.
        \param messages The messages to replay.
      */
      template<typename MarketDataFeedClientForward, typename TimeClientForward,
        typename TimerForward>
      ReplayMarketDataFeedClient(
        MarketDataFeedClientForward&& marketDataFeedClient,
        TimeClientForward&& timeClient, TimerForward&& timer,
        std::deque<MarketDataService::MarketDataFeedMessage> messages);

      ~ReplayMarketDataFeedClient();

      void Open();

      void Close();

    private:
      Beam::GetOptionalLocalPtr<MarketDataFeedClientType> m_feedClient;
      Beam::GetOptionalLocalPtr<TimeClientType> m_timeClient;
      Beam::GetOptionalLocalPtr<TimerType> m_timer;
      boost::posix_time::ptime m_lastTimestamp;
      boost::posix_time::ptime m_referenceTimestamp;
      std::deque<MarketDataService::MarketDataFeedMessage> m_messages;
      Beam::IO::OpenState m_openState;
      Beam::RoutineTaskQueue m_callbacks;

      void Shutdown();
      void OnTimerExpired(Beam::Threading::Timer::Result result);
  };

  template<typename MarketDataFeedClientType, typename TimeClientType,
    typename TimerType>
  template<typename MarketDataFeedClientForward, typename TimeClientForward,
    typename TimerForward>
  ReplayMarketDataFeedClient<MarketDataFeedClientType, TimeClientType,
      TimerType>::ReplayMarketDataFeedClient(
      MarketDataFeedClientForward&& marketDataFeedClient,
      TimeClientForward&& timeClient, TimerForward&& timer,
      std::deque<MarketDataService::MarketDataFeedMessage> messages)
      : m_feedClient(std::forward<MarketDataFeedClientForward>(
          marketDataFeedClient)),
        m_timeClient(std::forward<TimeClientForward>(timeClient)),
        m_timer(std::forward<TimerForward>(timer)),
        m_messages(std::move(messages)) {}

  template<typename MarketDataFeedClientType, typename TimeClientType,
    typename TimerType>
  ReplayMarketDataFeedClient<MarketDataFeedClientType, TimeClientType,
      TimerType>::~ReplayMarketDataFeedClient() {
    Close();
  }

  template<typename MarketDataFeedClientType, typename TimeClientType,
    typename TimerType>
  void ReplayMarketDataFeedClient<MarketDataFeedClientType, TimeClientType,
      TimerType>::Open() {
    if(m_openState.SetOpening()) {
      return;
    }
    try {
      m_feedClient->Open();
      m_timeClient->Open();
      m_timer->GetPublisher().Monitor(
        m_callbacks.GetSlot<Beam::Threading::Timer::Result>(
        std::bind(&ReplayMarketDataFeedClient::OnTimerExpired, this,
        std::placeholders::_1)));
      m_lastTimestamp = m_timeClient->GetTime();
      m_referenceTimestamp = Details::GetTimestamp(m_messages.front());
      m_timer->Start();
    } catch(std::exception&) {
      m_openState.SetOpenFailure();
      Shutdown();
    }
    m_openState.SetOpen();
  }

  template<typename MarketDataFeedClientType, typename TimeClientType,
    typename TimerType>
  void ReplayMarketDataFeedClient<MarketDataFeedClientType, TimeClientType,
      TimerType>::Close() {
    if(m_openState.SetClosing()) {
      return;
    }
    Shutdown();
  }

  template<typename MarketDataFeedClientType, typename TimeClientType,
    typename TimerType>
  void ReplayMarketDataFeedClient<MarketDataFeedClientType, TimeClientType,
      TimerType>::Shutdown() {
    m_feedClient->Close();
    m_timer->Cancel();
    m_callbacks.Break();
    m_openState.SetClosed();
  }

  template<typename MarketDataFeedClientType, typename TimeClientType,
    typename TimerType>
  void ReplayMarketDataFeedClient<MarketDataFeedClientType, TimeClientType,
      TimerType>::OnTimerExpired(Beam::Threading::Timer::Result result) {
    auto timestamp = m_timeClient->GetTime();
    auto delta = timestamp - m_lastTimestamp;
    m_referenceTimestamp += delta;
    auto visitor = Beam::MakeVariantLambdaVisitor<void>(
      [&] (const SecurityBboQuote& bboQuote) {
        std::cout << bboQuote.GetIndex() << " " <<
          bboQuote->m_bid.m_price << " " <<
          bboQuote->m_ask.m_price << std::endl;
      },
      [&] (const SecurityMarketQuote& marketQuote) {
      },
      [&] (const SecurityBookQuote& bookQuote) {
      },
      [&] (const SecurityTimeAndSale& timeAndSale) {
      },
      [&] (const MarketOrderImbalance& orderImbalance) {
      });
    while(!m_messages.empty() &&
        Details::GetTimestamp(m_messages.front()) < m_referenceTimestamp) {
      boost::apply_visitor(visitor, m_messages.front());
      m_messages.pop_front();
    }
    std::cout << delta << std::endl;
    m_lastTimestamp = timestamp;
    m_timer->Start();
  }
}

#endif
