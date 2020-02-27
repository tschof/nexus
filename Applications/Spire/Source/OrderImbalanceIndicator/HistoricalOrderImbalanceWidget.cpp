#include "Spire/OrderImbalanceIndicator/HistoricalOrderImbalanceWidget.hpp"
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include "Spire/OrderImbalanceIndicator/HistoricalOrderImbalanceChartView.hpp"
#include "Spire/Spire/Dimensions.hpp"
#include "Spire/Ui/DropDownMenu.hpp"

using namespace Spire;

HistoricalOrderImbalanceWidget::HistoricalOrderImbalanceWidget(
    std::shared_ptr<OrderImbalanceIndicatorModel> model, QWidget* parent)
    : QWidget(parent) {
  setStyleSheet("background-color: #FFFFFF;");
  auto layout = new QVBoxLayout(this);
  layout->setContentsMargins(scale_width(26), scale_height(18), scale_width(8),
    scale_height(18));
  layout->setSpacing(0);
  auto dropdown_layout = new QHBoxLayout();
  dropdown_layout->setContentsMargins({});
  dropdown_layout->setSpacing(0);
  layout->addLayout(dropdown_layout);
  auto label = new QLabel(tr("Historical Data"), this);
  label->setFixedSize(scale(87, 26));
  label->setStyleSheet(QString(R"(
      color: #000000;
      font-family: Roboto;
      font-size: %1px;
    )").arg(scale_height(12)));
  dropdown_layout->addWidget(label);
  m_data_dropdown = new DropDownMenu({tr("Reference Price"), tr("Size"),
    tr("Notional Value")});
  m_data_dropdown->setFixedSize(scale(110, 26));
  dropdown_layout->addWidget(m_data_dropdown);
  dropdown_layout->addStretch(1);
  layout->addSpacing(scale_height(8));
  m_chart_widget = new HistoricalOrderImbalanceChartView(model, this);
  m_chart_widget->setFixedHeight(scale_height(185));
  layout->addWidget(m_chart_widget);
}
