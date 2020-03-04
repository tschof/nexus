#include "Spire/OrderImbalanceIndicator/HistoricalOrderImbalanceWidget.hpp"
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include "Spire/Spire/Dimensions.hpp"
#include "Spire/Ui/DropDownMenu.hpp"

using namespace Spire;

HistoricalOrderImbalanceWidget::HistoricalOrderImbalanceWidget(
    const TimeInterval& interval,
    const std::vector<Nexus::OrderImbalance>& imbalances, QWidget* parent)
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
  auto reference_text = tr("Reference Price");
  m_display_options[reference_text] =
    HistoricalOrderImbalanceChartView::DisplayType::REFERENCE_PRICE;
  auto size_text = tr("Size");
  m_display_options[size_text] =
    HistoricalOrderImbalanceChartView::DisplayType::SIZE;
  auto value_text = tr("Notional Value");
  m_display_options[value_text] =
    HistoricalOrderImbalanceChartView::DisplayType::NOTIONAL_VALUE;
  m_data_dropdown = new DropDownMenu({reference_text, size_text, value_text});
  m_data_dropdown->setFixedSize(scale(110, 26));
  m_data_dropdown->connect_selected_signal([=] (auto& text) {
      on_dropdown_changed(text);
    });
  dropdown_layout->addWidget(m_data_dropdown);
  dropdown_layout->addStretch(1);
  layout->addSpacing(scale_height(8));
  m_chart_widget = new HistoricalOrderImbalanceChartView(interval, imbalances,
    this);
  m_chart_widget->setFixedHeight(scale_height(185));
  layout->addWidget(m_chart_widget);
}

void HistoricalOrderImbalanceWidget::on_dropdown_changed(const QString& text) {
  m_chart_widget->set_display_type(m_display_options[text]);
}
