#ifndef NEXUS_DEFINITIONSWEBSERVLET_HPP
#define NEXUS_DEFINITIONSWEBSERVLET_HPP
#include <Beam/IO/OpenState.hpp>
#include <Beam/Pointers/Ref.hpp>
#include <Beam/WebServices/HttpRequestSlot.hpp>
#include <Beam/WebServices/SessionStore.hpp>
#include <boost/noncopyable.hpp>
#include "web_portal/web_portal/web_portal.hpp"
#include "web_portal/web_portal/web_portal_session.hpp"
#include "Nexus/ServiceClients/ApplicationServiceClients.hpp"

namespace Nexus {
namespace ClientWebPortal {

  /*! \class DefinitionsWebServlet
      \brief Provides a web interface to the DefinitionsService.
   */
  class DefinitionsWebServlet : private boost::noncopyable {
    public:

      //! Constructs a DefinitionsWebServlet.
      /*!
        \param sessions The available web sessions.
        \param serviceClients The clients used to access Spire services.
      */
      DefinitionsWebServlet(Beam::RefType<
        Beam::WebServices::SessionStore<ClientWebPortalSession>> sessions,
        Beam::RefType<ApplicationServiceClients> serviceClients);

      ~DefinitionsWebServlet();

      std::vector<Beam::WebServices::HttpRequestSlot> GetSlots();

      void Open();

      void Close();

    private:
      ApplicationServiceClients* m_serviceClients;
      Beam::WebServices::SessionStore<ClientWebPortalSession>* m_sessions;
      Beam::IO::OpenState m_openState;

      void Shutdown();
      Beam::WebServices::HttpResponse OnLoadComplianceRuleSchemas(
        const Beam::WebServices::HttpRequest& request);
      Beam::WebServices::HttpResponse OnLoadCountryDatabase(
        const Beam::WebServices::HttpRequest& request);
      Beam::WebServices::HttpResponse OnLoadCurrencyDatabase(
        const Beam::WebServices::HttpRequest& request);
      Beam::WebServices::HttpResponse OnLoadDestinationDatabase(
        const Beam::WebServices::HttpRequest& request);
      Beam::WebServices::HttpResponse OnLoadExchangeRates(
        const Beam::WebServices::HttpRequest& request);
      Beam::WebServices::HttpResponse OnLoadMarketDatabase(
        const Beam::WebServices::HttpRequest& request);
  };
}
}

#endif
