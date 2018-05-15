#ifndef SPIRE_GROUPPROFITANDLOSSREPORTWIDGET_HPP
#define SPIRE_GROUPPROFITANDLOSSREPORTWIDGET_HPP
#include <vector>
#include <QWidget>
#include <Beam/Pointers/DelayPtr.hpp>
#include <Beam/Queues/SequencePublisher.hpp>
#include <Beam/ServiceLocator/DirectoryEntry.hpp>
#include "Spire/AccountViewer/AccountViewer.hpp"
#include "Spire/Blotter/ProfitAndLossModel.hpp"
#include "Spire/Spire/Spire.hpp"

class Ui_GroupProfitAndLossReportWidget;

namespace Spire {

  /*! \class GroupProfitAndLossReportWidget
      \brief Shows a group's profit and loss.
   */
  class GroupProfitAndLossReportWidget : public QWidget {
    public:

      //! Constructs a GroupProfitAndLossReportWidget.
      /*!
        \param parent The parent widget.
        \param flags Qt flags passed to the parent widget.
      */
      GroupProfitAndLossReportWidget(QWidget* parent = nullptr,
        Qt::WindowFlags flags = 0);

      virtual ~GroupProfitAndLossReportWidget();

      //! Initializes this widget.
      /*!
        \param userProfile The user's profile.
        \param group The group to display.
      */
      void Initialize(Beam::RefType<UserProfile> userProfile,
        const Beam::ServiceLocator::DirectoryEntry& group);

    private:
      struct ReportModel {
        std::shared_ptr<Nexus::OrderExecutionService::OrderExecutionPublisher>
          m_orderPublisher;
        ProfitAndLossModel m_profitAndLossModel;
        SpirePortfolioMonitor m_portfolioMonitor;

        ReportModel(Beam::RefType<UserProfile> userProfile,
          const std::shared_ptr<
          Nexus::OrderExecutionService::OrderExecutionPublisher>&
          orderPublisher);
      };
      std::unique_ptr<Ui_GroupProfitAndLossReportWidget> m_ui;
      UserProfile* m_userProfile;
      Beam::ServiceLocator::DirectoryEntry m_group;
      Beam::DelayPtr<ReportModel> m_totalsModel;
      std::vector<std::unique_ptr<ReportModel>> m_groupModels;

      void OnUpdate(bool checked);
  };
}

#endif