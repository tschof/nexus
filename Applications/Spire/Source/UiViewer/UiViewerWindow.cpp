#include "Spire/UiViewer/UiViewerWindow.hpp"
#include <QGridLayout>
#include <QLabel>
#include "Spire/Spire/Dimensions.hpp"
#include "Spire/Spire/Utility.hpp"
#include "Spire/UiViewer/CheckBoxTestWidget.hpp"
#include "Spire/UiViewer/ColorSelectorButtonTestWidget.hpp"
#include "Spire/UiViewer/ComboBoxTestWidget.hpp"
#include "Spire/UiViewer/CurrencyComboBoxTestWidget.hpp"
#include "Spire/UiViewer/DateInputTestWidget.hpp"
#include "Spire/UiViewer/DecimalSpinBoxTestWidget.hpp"
#include "Spire/UiViewer/DurationInputTestWidget.hpp"
#include "Spire/UiViewer/FilteredDropDownMenuTestWidget.hpp"
#include "Spire/UiViewer/FlatButtonTestWidget.hpp"
#include "Spire/UiViewer/FontSelectorTestWidget.hpp"
#include "Spire/UiViewer/IconButtonTestWidget.hpp"
#include "Spire/UiViewer/IntegerSpinBoxTestWidget.hpp"
#include "Spire/UiViewer/MarketComboBoxTestWidget.hpp"
#include "Spire/UiViewer/MoneySpinBoxTestWidget.hpp"
#include "Spire/UiViewer/OrderStatusComboBoxTestWidget.hpp"
#include "Spire/UiViewer/OrderTypeComboBoxTestWidget.hpp"
#include "Spire/UiViewer/QuantitySpinBoxTestWidget.hpp"
#include "Spire/UiViewer/RangeInputTestWidget.hpp"
#include "Spire/UiViewer/ScrollAreaTestWidget.hpp"
#include "Spire/UiViewer/SecurityInputTestWidget.hpp"
#include "Spire/UiViewer/SecurityWidgetTestWidget.hpp"
#include "Spire/UiViewer/SideComboBoxTestWidget.hpp"
#include "Spire/UiViewer/SpinBoxTestWidget.hpp"
#include "Spire/UiViewer/StaticDropDownMenuTestWidget.hpp"
#include "Spire/UiViewer/TabTestWidget.hpp"
#include "Spire/UiViewer/TextInputTestWidget.hpp"
#include "Spire/UiViewer/TimeInForceComboBoxTestWidget.hpp"
#include "Spire/UiViewer/TimeOfDayTestWidget.hpp"
#include "Spire/UiViewer/ToggleButtonTestWidget.hpp"
#include "Spire/UiViewer/TransitionTestWidget.hpp"

using namespace Spire;

UiViewerWindow::UiViewerWindow(QWidget* parent)
    : Window(parent) {
  setMinimumSize(scale(775, 432));
  setWindowTitle(tr("UI Viewer"));
  set_svg_icon(":/Icons/spire-icon-black.svg", ":/Icons/spire-icon-grey.svg");
  setWindowIcon(QIcon(":/Icons/spire-icon-256x256.png"));
  auto body = new QWidget(this);
  body->setStyleSheet("background-color: #F5F5F5");
  layout()->addWidget(body);
  m_layout = new QHBoxLayout(body);
  m_widget_list = new QListWidget(this);
  m_widget_list->setSelectionMode(QAbstractItemView::SingleSelection);
  m_widget_list->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  m_widget_list->setMaximumWidth(scale_width(200));
  m_widget_list->setStyleSheet(QString(R"(
    QListWidget {
      background-color: white;
      border: 1px solid #A0A0A0;
      outline: none;
      padding: %1px %2px 0px %2px;
    }

    QListWidget:focus {
      border: 1px solid #4B23A0;
    }

    QListWidget::item {
      padding-top: %5px;
      padding-bottom: %5px;
    }

    QListWidget::item:selected {
      border: %3px solid #4B23A0 %4px solid #4B23A0;
      color: #000000;
    })").arg(scale_height(4)).arg(scale_width(4))
        .arg(scale_height(1)).arg(scale_width(1))
        .arg(scale_height(3)));
  connect(m_widget_list, &QListWidget::currentItemChanged, this,
    &UiViewerWindow::on_item_selected);
  m_layout->addWidget(m_widget_list);
  add_test_widget(tr("CheckBox"), new CheckBoxTestWidget(this));
  add_test_widget(tr("ColorSelectorButton"),
    new ColorSelectorButtonTestWidget(this));
  add_test_widget(tr("CurrencyComboBox"), new ComboBoxTestWidget(
    new CurrencyComboBoxTestWidget(this), this));
  add_test_widget(tr("DateInputWidget"), new DateInputTestWidget(this));
  add_test_widget(tr("DecimalSpinBox"), new SpinBoxTestWidget(
    new DecimalSpinBoxTestWidget(this), this));
  add_test_widget(tr("DurationInputWidget"),
    new DurationInputTestWidget(this));
  add_test_widget(tr("FilteredDropDownMenu"),
    new FilteredDropDownMenuTestWidget(this));
  add_test_widget(tr("FlatButton"), new FlatButtonTestWidget(this));
  add_test_widget(tr("FontSelectorWidget"), new FontSelectorTestWidget(this));
  add_test_widget(tr("IconButton"), new IconButtonTestWidget(this));
  add_test_widget(tr("IntegerSpinBox"), new SpinBoxTestWidget(
    new IntegerSpinBoxTestWidget(this), this));
  add_test_widget(tr("MarketComboBox"), new ComboBoxTestWidget(
    new MarketComboBoxTestWidget(this)));
  add_test_widget(tr("MoneySpinBox"), new SpinBoxTestWidget(
    new MoneySpinBoxTestWidget(this), this));
  add_test_widget(tr("OrderStatusComboBox"), new ComboBoxTestWidget(
    new OrderStatusComboBoxTestWidget(this)));
  add_test_widget(tr("OrderTypeComboBox"), new ComboBoxTestWidget(
    new OrderTypeComboBoxTestWidget(this)));
  add_test_widget(tr("QuantitySpinBox"), new SpinBoxTestWidget(
    new QuantitySpinBoxTestWidget(this), this));
  add_test_widget(tr("RangeInputWidget"), new RangeInputTestWidget(this));
  add_test_widget(tr("ScrollArea"), new ScrollAreaTestWidget(this));
  add_test_widget(tr("SecurityInput"), new SecurityInputTestWidget(this));
  add_test_widget(tr("SecurityWidget"), new SecurityWidgetTestWidget(this));
  add_test_widget(tr("SideComboBox"), new ComboBoxTestWidget(
    new SideComboBoxTestWidget(this), this));
  add_test_widget(tr("StaticDropDownMenu"),
    new StaticDropDownMenuTestWidget(this));
  add_test_widget(tr("TabWidget"), new TabTestWidget(this));
  add_test_widget(tr("TextInputWidget"), new TextInputTestWidget(this));
  add_test_widget(tr("TimeInForceComboBox"), new ComboBoxTestWidget(
    new TimeInForceComboBoxTestWidget(this)));
  add_test_widget(tr("TimeOfDayInputWidget"), new TimeOfDayTestWidget(this));
  add_test_widget(tr("ToggleButton"), new ToggleButtonTestWidget(this));
  add_test_widget(tr("TransitionWidget"), new TransitionTestWidget(this));
  m_widget_list->setCurrentRow(0);
}

void UiViewerWindow::add_test_widget(const QString& name, QWidget* widget) {
  m_widgets.insert(name, widget);
  m_layout->addWidget(widget);
  m_widget_list->addItem(name);
  widget->hide();
}

void UiViewerWindow::on_item_selected(const QListWidgetItem* current,
    const QListWidgetItem* previous) {
  if(previous) {
    m_widgets[previous->text()]->hide();
  }
  m_widgets[current->text()]->show();
}