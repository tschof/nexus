#include "Spire/RiskTimer/RiskTimerMonitor.hpp"
#include "Spire/RiskTimer/RiskTimerModel.hpp"
#include "Spire/RiskTimer/RiskTimerDialog.hpp"
#include "Spire/UI/UserProfile.hpp"

using namespace Beam;
using namespace Beam::ServiceLocator;
using namespace boost;
using namespace boost::posix_time;
using namespace Nexus;
using namespace Nexus::AdministrationService;
using namespace Nexus::RiskService;
using namespace Spire;

namespace {
  const unsigned int UPDATE_INTERVAL = 100;
}

RiskTimerMonitor::RiskTimerMonitor(Ref<UserProfile> userProfile)
    : m_userProfile(userProfile.Get()) {
  connect(&m_updateTimer, &QTimer::timeout, this,
    &RiskTimerMonitor::OnUpdateTimer);
  m_updateTimer.start(UPDATE_INTERVAL);
}

RiskTimerMonitor::~RiskTimerMonitor() {
  if(m_dialog != nullptr) {
    m_userProfile->GetRiskTimerProperties().SetRiskTimerDialogInitialPosition(
      m_dialog->pos());
  }
}

void RiskTimerMonitor::Load() {
  auto account =
    m_userProfile->GetServiceClients().GetServiceLocatorClient().GetAccount();
  m_userProfile->GetServiceClients().GetAdministrationClient().
    GetRiskStatePublisher(account).Monitor(m_slotHandler.GetSlot<RiskState>(
    std::bind(&RiskTimerMonitor::OnRiskState, this, std::placeholders::_1)));
}

void RiskTimerMonitor::OnRiskState(const RiskState& riskState) {
  if(riskState.m_type == RiskState::Type::ACTIVE) {
    if(m_dialog != nullptr) {
      m_userProfile->GetRiskTimerProperties().SetRiskTimerDialogInitialPosition(
        m_dialog->pos());
      m_dialog->close();
      m_dialog.reset();
      m_model.reset();
    }
  } else if(riskState.m_type == RiskState::Type::CLOSE_ORDERS ||
      riskState.m_type == RiskState::Type::DISABLED) {
    if(m_dialog == nullptr) {
      m_model = std::make_shared<RiskTimerModel>(Ref(*m_userProfile));
      if(riskState.m_expiry == pos_infin) {
        m_model->SetTimeRemaining(seconds(0));
      } else {
        auto currentTime =
          m_userProfile->GetServiceClients().GetTimeClient().GetTime();
        m_model->SetTimeRemaining(std::max(time_duration(seconds(0)),
          riskState.m_expiry - currentTime));
      }
      m_dialog = std::make_unique<RiskTimerDialog>();
      m_dialog->setWindowFlags(
        Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
      m_dialog->SetModel(m_model);
      m_dialog->setFixedSize(m_dialog->size());
      m_dialog->move(m_userProfile->GetRiskTimerProperties().
        GetRiskTimerDialogInitialPosition());
      m_dialog->show();
    }
  }
}

void RiskTimerMonitor::OnUpdateTimer() {
  HandleTasks(m_slotHandler);
}
