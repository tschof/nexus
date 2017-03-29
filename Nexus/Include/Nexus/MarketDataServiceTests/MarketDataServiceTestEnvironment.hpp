#ifndef NEXUS_MARKETDATASERVICETESTENVIRONMENT_HPP
#define NEXUS_MARKETDATASERVICETESTENVIRONMENT_HPP
#include <Beam/IO/LocalClientChannel.hpp>
#include <Beam/IO/LocalServerConnection.hpp>
#include <Beam/IO/SharedBuffer.hpp>
#include <Beam/Pointers/UniquePointerPolicy.hpp>
#include <Beam/Serialization/BinaryReceiver.hpp>
#include <Beam/Serialization/BinarySender.hpp>
#include <Beam/ServiceLocator/AuthenticationServletAdapter.hpp>
#include <Beam/ServiceLocator/SessionAuthenticator.hpp>
#include <Beam/ServiceLocatorTests/ServiceLocatorTestEnvironment.hpp>
#include <Beam/Services/AuthenticatedServiceProtocolClientBuilder.hpp>
#include <Beam/Services/ServiceProtocolClient.hpp>
#include <Beam/Services/ServiceProtocolServletContainer.hpp>
#include <Beam/Threading/TriggerTimer.hpp>
#include <Beam/TimeService/IncrementalTimeClient.hpp>
#include <boost/functional/factory.hpp>
#include <boost/functional/value_factory.hpp>
#include <boost/noncopyable.hpp>
#include <boost/optional/optional.hpp>
#include "Nexus/AdministrationService/VirtualAdministrationClient.hpp"
#include "Nexus/MarketDataService/LocalHistoricalDataStore.hpp"
#include "Nexus/MarketDataService/MarketDataClient.hpp"
#include "Nexus/MarketDataService/MarketDataFeedClient.hpp"
#include "Nexus/MarketDataService/MarketDataFeedServlet.hpp"
#include "Nexus/MarketDataService/MarketDataRegistry.hpp"
#include "Nexus/MarketDataService/MarketDataRegistryServlet.hpp"
#include "Nexus/MarketDataService/VirtualMarketDataClient.hpp"
#include "Nexus/MarketDataService/VirtualMarketDataFeedClient.hpp"

namespace Nexus {
namespace MarketDataService {
namespace Tests {

  /*! \class MarketDataServiceTestEnvironment
      \brief Wraps most components needed to run an instance of the
             MarketDataService with helper functions.
   */
  class MarketDataServiceTestEnvironment : private boost::noncopyable {
    public:

      //! Constructs an MarketDataServiceTestEnvironment.
      /*!
        \param serviceLocatorClient The ServiceLocatorClient to use.
        \param administrationClient The AdministrationClient to use.
      */
      MarketDataServiceTestEnvironment(
        std::shared_ptr<Beam::ServiceLocator::VirtualServiceLocatorClient>
        serviceLocatorClient,
        std::shared_ptr<AdministrationService::VirtualAdministrationClient>
        administrationClient);

      ~MarketDataServiceTestEnvironment();

      //! Opens the servlet.
      void Open();

      //! Closes the servlet.
      void Close();

      //! Returns the historical data store.
      const LocalHistoricalDataStore& GetDataStore() const;

      //! Returns the historical data store.
      LocalHistoricalDataStore& GetDataStore();

      //! Returns the MarketDataRegistry.
      const MarketDataRegistry& GetRegistry() const;

      //! Sets a Security's BBO.
      /*!
        \param security The Security to set.
        \param bbo The <i>security</i>'s BBO.
      */
      void SetBbo(const Security& security, const BboQuote& bbo);

      //! Builds a new MarketDataClient.
      /*!
        \param serviceLocatorClient The ServiceLocatorClient used to
               authenticate the MarketDataClient.
      */
      std::unique_ptr<VirtualMarketDataClient> BuildClient(
        Beam::RefType<Beam::ServiceLocator::VirtualServiceLocatorClient>
        serviceLocatorClient);

      //! Builds a new MarketDataFeedClient.
      /*!
        \param serviceLocatorClient The ServiceLocatorClient used to
               authenticate the MarketDataFeedClient.
      */
      std::unique_ptr<VirtualMarketDataFeedClient> BuildFeedClient(
        Beam::RefType<Beam::ServiceLocator::VirtualServiceLocatorClient>
        serviceLocatorClient);

    private:
      using ServerConnection =
        Beam::IO::LocalServerConnection<Beam::IO::SharedBuffer>;
      using ClientChannel =
        Beam::IO::LocalClientChannel<Beam::IO::SharedBuffer>;
      using AdministrationClient =
        AdministrationService::VirtualAdministrationClient;
      using ServiceLocatorClient =
        Beam::ServiceLocator::VirtualServiceLocatorClient;
      using ServiceProtocolServletContainer =
        Beam::Services::ServiceProtocolServletContainer<
        Beam::ServiceLocator::MetaAuthenticationServletAdapter<
        MetaMarketDataRegistryServlet<MarketDataRegistry*,
        LocalHistoricalDataStore*, std::shared_ptr<AdministrationClient>>,
        ServiceLocatorClient*, Beam::NativePointerPolicy>, ServerConnection*,
        Beam::Serialization::BinarySender<Beam::IO::SharedBuffer>,
        Beam::Codecs::NullEncoder,
        std::shared_ptr<Beam::Threading::TriggerTimer>>;
      using BaseRegistryServlet = MarketDataRegistryServlet<
        ServiceProtocolServletContainer, MarketDataRegistry*,
        LocalHistoricalDataStore*, std::shared_ptr<AdministrationClient>>;
      using RegistryServlet =
        Beam::ServiceLocator::AuthenticationServletAdapter<
        ServiceProtocolServletContainer, BaseRegistryServlet*,
        ServiceLocatorClient*>;
      using FeedServiceProtocolServletContainer =
        Beam::Services::ServiceProtocolServletContainer<
        Beam::ServiceLocator::MetaAuthenticationServletAdapter<
        MetaMarketDataFeedServlet<BaseRegistryServlet*>, ServiceLocatorClient*>,
        ServerConnection*,
        Beam::Serialization::BinarySender<Beam::IO::SharedBuffer>,
        Beam::Codecs::NullEncoder,
        std::shared_ptr<Beam::Threading::TriggerTimer>>;
      using ServiceProtocolClientBuilder =
        Beam::Services::AuthenticatedServiceProtocolClientBuilder<
        ServiceLocatorClient, Beam::Services::MessageProtocol<
        std::unique_ptr<ClientChannel>,
        Beam::Serialization::BinarySender<Beam::IO::SharedBuffer>,
        Beam::Codecs::NullEncoder>, Beam::Threading::TriggerTimer>;
      using MarketDataClient = MarketDataService::VirtualMarketDataClient;
      using MarketDataFeedClient =
        MarketDataService::VirtualMarketDataFeedClient;
      std::shared_ptr<ServiceLocatorClient> m_serviceLocatorClient;
      std::shared_ptr<AdministrationClient> m_administrationClient;
      MarketDataRegistry m_registry;
      ServerConnection m_serverConnection;
      LocalHistoricalDataStore m_dataStore;
      boost::optional<BaseRegistryServlet> m_registryServlet;
      boost::optional<ServiceProtocolServletContainer> m_container;
      ServerConnection m_feedServerConnection;
      boost::optional<FeedServiceProtocolServletContainer> m_feedContainer;
      Beam::Threading::TriggerTimer m_samplingTimer;
      std::unique_ptr<MarketDataFeedClient> m_feedClient;
  };

  inline MarketDataServiceTestEnvironment::MarketDataServiceTestEnvironment(
      std::shared_ptr<Beam::ServiceLocator::VirtualServiceLocatorClient>
      serviceLocatorClient,
      std::shared_ptr<AdministrationService::VirtualAdministrationClient>
      administrationClient)
      : m_serviceLocatorClient{std::move(serviceLocatorClient)},
        m_administrationClient{std::move(administrationClient)} {}

  inline MarketDataServiceTestEnvironment::~MarketDataServiceTestEnvironment() {
    Close();
  }

  inline void MarketDataServiceTestEnvironment::Open() {
    m_registryServlet.emplace(m_administrationClient, &m_registry,
      &m_dataStore);
    m_container.emplace(Beam::Initialize(m_serviceLocatorClient.get(),
      &*m_registryServlet), &m_serverConnection,
      boost::factory<std::shared_ptr<Beam::Threading::TriggerTimer>>());
    m_feedContainer.emplace(Beam::Initialize(m_serviceLocatorClient.get(),
      &*m_registryServlet), &m_feedServerConnection,
      boost::factory<std::shared_ptr<Beam::Threading::TriggerTimer>>());
    m_container->Open();
    m_feedContainer->Open();
  }

  inline void MarketDataServiceTestEnvironment::Close() {
    m_feedContainer.reset();
    m_container.reset();
    m_registryServlet.reset();
  }

  inline const LocalHistoricalDataStore& MarketDataServiceTestEnvironment::
      GetDataStore() const {
    return m_dataStore;
  }

  inline LocalHistoricalDataStore& MarketDataServiceTestEnvironment::
      GetDataStore() {
    return m_dataStore;
  }

  inline const MarketDataRegistry&
      MarketDataServiceTestEnvironment::GetRegistry() const {
    return m_registry;
  }

  inline void MarketDataServiceTestEnvironment::SetBbo(const Security& security,
      const BboQuote& bbo) {
    m_registryServlet->PublishBboQuote(SecurityBboQuote(bbo, security), 0);
  }

  inline std::unique_ptr<VirtualMarketDataClient>
      MarketDataServiceTestEnvironment::BuildClient(
      Beam::RefType<Beam::ServiceLocator::VirtualServiceLocatorClient>
      serviceLocatorClient) {
    ServiceProtocolClientBuilder builder(Beam::Ref(serviceLocatorClient),
      [&] {
        return std::make_unique<ServiceProtocolClientBuilder::Channel>(
          "test_market_data_client", Beam::Ref(m_serverConnection));
      },
      [&] {
        return std::make_unique<ServiceProtocolClientBuilder::Timer>();
      });
    auto client = std::make_unique<MarketDataService::MarketDataClient<
      ServiceProtocolClientBuilder>>(builder);
    return MakeVirtualMarketDataClient(std::move(client));
  }

  inline std::unique_ptr<VirtualMarketDataFeedClient>
      MarketDataServiceTestEnvironment::BuildFeedClient(
      Beam::RefType<ServiceLocatorClient> serviceLocatorClient) {
    using Client = MarketDataService::MarketDataFeedClient<std::string,
      Beam::Threading::TriggerTimer*, Beam::Services::MessageProtocol<
      ClientChannel, Beam::Serialization::BinarySender<Beam::IO::SharedBuffer>,
      Beam::Codecs::NullEncoder>, Beam::Threading::TriggerTimer>;
    auto client = std::make_unique<Client>(Beam::Initialize(
      std::string("test_market_data_feed_client"),
      Beam::Ref(m_feedServerConnection)),
      Beam::ServiceLocator::SessionAuthenticator<ServiceLocatorClient>(
      Beam::Ref(serviceLocatorClient)), &m_samplingTimer, Beam::Initialize());
    return MakeVirtualMarketDataFeedClient(std::move(client));
  }
}
}
}

#endif