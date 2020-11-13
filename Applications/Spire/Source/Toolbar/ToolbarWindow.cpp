#include "Spire/Toolbar/ToolbarWindow.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "Spire/Spire/Dimensions.hpp"
#include "Spire/Toolbar/ToolbarMenu.hpp"
#include "Spire/Ui/IconButton.hpp"
#include "Spire/Ui/Window.hpp"

using namespace Beam;
using namespace Beam::ServiceLocator;
using namespace boost;
using namespace boost::signals2;
using namespace Spire;

namespace {
  auto BUTTON_SIZE() {
    static auto size = scale(20, 28);
    return size;
  }

  auto ICON_SIZE() {
    static auto icon_size = scale(10, 10);
    return icon_size;
  }

  auto ICON_RECT() {
    static auto icon_rect = QRect(QPoint(0, 0), scale(10, 10));
    return icon_rect;
  }

  auto create_button(const QString& icon, const QString& hover_icon,
      const QString& tooltip, QWidget* parent) {
    auto button = new IconButton(imageFromSvg(icon, BUTTON_SIZE()), parent);
    button->setFixedSize(BUTTON_SIZE());
    button->setToolTip(tooltip);
    return button;
  }
}

ToolbarWindow::ToolbarWindow(Ref<RecentlyClosedModel> model,
    const DirectoryEntry& account, QWidget* parent)
    : Window(parent),
      m_model(model.Get()) {
  set_fixed_body_size(scale(308, 72));
  set_svg_icon(":/Icons/spire-icon-black.svg");
  setWindowIcon(QIcon(":/Icons/spire-icon-256x256.png"));
  setWindowTitle(tr("Spire - Signed in as ") +
    QString::fromStdString(account.m_name));
  auto body = new QWidget(this);
  body->setStyleSheet("background-color: #F5F5F5;");
  auto layout = new QVBoxLayout(body);
  layout->setContentsMargins({});
  layout->setSpacing(0);
  layout->addStretch(8);
  auto combo_box_layout = new QHBoxLayout();
  combo_box_layout->setContentsMargins({});
  combo_box_layout->setSpacing(0);
  layout->addLayout(combo_box_layout);
  layout->setStretchFactor(combo_box_layout, 26);
  layout->addStretch(10);
  combo_box_layout->addStretch(8);
  m_window_manager_button = new StaticDropDownMenu({tr("Minimize All"),
    tr("Restore All"), tr("Import/Export Settings")}, tr("Window Manager"),
    body);
  m_window_manager_button->set_next_activated(false);
  m_window_manager_button->setSizePolicy(QSizePolicy::Expanding,
    QSizePolicy::Expanding);
  combo_box_layout->addWidget(m_window_manager_button);
  combo_box_layout->setStretchFactor(m_window_manager_button, 138);
  combo_box_layout->addStretch(16);
  m_recently_closed_button = new ToolbarMenu(tr("Recently Closed"), body);
  m_recently_closed_button->setSizePolicy(QSizePolicy::Expanding,
    QSizePolicy::Expanding);
  m_recently_closed_button->connect_index_selected_signal(
    [=] (auto index) { on_item_selected(index); });
  combo_box_layout->addWidget(m_recently_closed_button);
  combo_box_layout->setStretchFactor(m_recently_closed_button, 138);
  combo_box_layout->addStretch(8);
  auto button_layout = new QHBoxLayout();
  button_layout->setContentsMargins({});
  button_layout->setSpacing(0);
  m_account_button = create_button(":/Icons/account-light-purple.svg",
    ":/Icons/account-purple.svg", tr("Account"), body);
  button_layout->addWidget(m_account_button);
  m_key_bindings_button = create_button(
    ":/Icons/key-bindings-light-purple.svg", ":/Icons/key-bindings-purple.svg",
    tr("Key Bindings"), body);
  button_layout->addWidget(m_key_bindings_button);
  m_canvas_button = create_button(":/Icons/canvas-light-purple.svg",
    ":/Icons/canvas-purple.svg", tr("Canvas"), body);
  button_layout->addWidget(m_canvas_button);
  m_book_view_button = create_button(":/Icons/bookview-light-purple.svg",
    ":/Icons/bookview-purple.svg", tr("Book View"), body);
  m_book_view_button->connect_clicked_signal(
    [=] { on_open_window(RecentlyClosedModel::Type::BOOK_VIEW); });
  button_layout->addWidget(m_book_view_button);
  m_time_and_sales_button = create_button(
    ":/Icons/time-sale-light-purple.svg", ":/Icons/time-sale-purple.svg",
    tr("Time and Sales"), body);
  m_time_and_sales_button->connect_clicked_signal(
    [=] { on_open_window(RecentlyClosedModel::Type::TIME_AND_SALE); });
  button_layout->addWidget(m_time_and_sales_button);
  m_chart_button = create_button(":/Icons/chart-light-purple.svg",
    ":/Icons/chart-purple.svg", tr("Chart"), body);
  button_layout->addWidget(m_chart_button);
  m_dashboard_button = create_button(":/Icons/dashboard-light-purple.svg",
    ":/Icons/dashboard-purple.svg", tr("Dashboard"), body);
  button_layout->addWidget(m_dashboard_button);
  m_order_imbalances_button = create_button(
    ":/Icons/order-imbalances-light-purple.svg",
    ":/Icons/order-imbalances-purple.svg", tr("Order Imbalances"), body);
  button_layout->addWidget(m_order_imbalances_button);
  m_blotter_button = create_button(":/Icons/blotter-light-purple.svg",
    ":/Icons/blotter-purple.svg", tr("Blotter"), body);
  button_layout->addWidget(m_blotter_button);
  layout->addLayout(button_layout);
  layout->setStretchFactor(button_layout, 28);
  Window::layout()->addWidget(body);
  m_entry_added_connection = m_model->connect_entry_added_signal(
    [=] (auto& e) {entry_added(e);});
  m_entry_removed_connection = m_model->connect_entry_removed_signal(
    [=] (auto e) {entry_removed(e);});
}

connection ToolbarWindow::connect_open_signal(
    const OpenSignal::slot_type& slot) const {
  return m_open_signal.connect(slot);
}

connection ToolbarWindow::connect_reopen_signal(
    const ReopenSignal::slot_type& slot) const {
  return m_reopen_signal.connect(slot);
}

void ToolbarWindow::entry_added(const RecentlyClosedModel::Entry& e) {
  m_entries.push_back(e);
  switch(e.m_type) {
    case RecentlyClosedModel::Type::BOOK_VIEW: {
      m_recently_closed_button->add(QString::fromStdString(e.m_identifier),
        imageFromSvg(QString(":/Icons/bookview-black.svg"), ICON_SIZE(),
          ICON_RECT()));
      break;
    }
    case RecentlyClosedModel::Type::TIME_AND_SALE: {
      m_recently_closed_button->add(QString::fromStdString(e.m_identifier),
        imageFromSvg(QString(":/Icons/time-sale-black.svg"), ICON_SIZE(),
          ICON_RECT()));
      break;
    }
  }
}

void ToolbarWindow::entry_removed(const RecentlyClosedModel::Entry& e) {
  for(auto i = 0; i < static_cast<int>(m_entries.size()); ++i) {
    if(m_entries[i].m_id == e.m_id) {
      m_entries.erase(m_entries.begin() + i);
      m_recently_closed_button->remove_item(i);
    }
  }
}

void ToolbarWindow::on_item_selected(int index) {
  m_reopen_signal(m_entries[index]);
}

void ToolbarWindow::on_open_window(RecentlyClosedModel::Type w) {
  m_open_signal(w);
}
