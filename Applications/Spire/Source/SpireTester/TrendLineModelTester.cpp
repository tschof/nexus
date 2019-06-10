#include <catch.hpp>
#include "Spire/SpireTester/SpireTester.hpp"
#include "Spire/Charting/ChartPoint.hpp"
#include "Spire/Charting/ChartValue.hpp"
#include "Spire/Charting/TrendLine.hpp"
#include "Spire/Charting/TrendLineModel.hpp"

using namespace Spire;

namespace {
  ChartPoint make_point(double x, double y) {
    return ChartPoint(ChartValue(x), ChartValue(y));
  }  

  TrendLine make_line(int x1, int y1, int x2, int y2, const QColor& color,
      TrendLineStyle style) {
    return TrendLine{{make_point(x1, y1), make_point(x2, y2)}, color, style};
  }

  bool operator ==(const ChartPoint& lhs, const ChartPoint& rhs) {
    return lhs.m_x == rhs.m_x && lhs.m_y == rhs.m_y;
  }

  bool operator ==(const TrendLine& lhs, const TrendLine& rhs) {
    return std::get<0>(lhs.m_points) == std::get<0>(rhs.m_points) &&
      std::get<1>(lhs.m_points) == std::get<1>(rhs.m_points) &&
      lhs.m_color == rhs.m_color && lhs.m_style == rhs.m_style;
  }
}

TEST_CASE("test_creating_and_getting_trend_lines", "[TrendLineModel]") {
  auto model = TrendLineModel();
  auto line1 = make_line(10, 10, 20, 20, Qt::red, TrendLineStyle::SOLID);
  auto id1 = model.add(line1);
  auto line2 = make_line(30, 30, 40, 40, Qt::red, TrendLineStyle::SOLID);
  auto id2 = model.add(line2);
  auto line3 = make_line(50, 50, 60, 60, Qt::red, TrendLineStyle::SOLID);
  auto id3 = model.add(line3);
  auto line4 = make_line(70, 70, 80, 80, Qt::red, TrendLineStyle::SOLID);
  auto id4 = model.add(line4);
  REQUIRE(line1 == model.get(id1));
  REQUIRE(line2 == model.get(id2));
  REQUIRE(line3 == model.get(id3));
  REQUIRE(line4 == model.get(id4));
  print_test_name("test_creating_and_getting_trend_lines");
}

TEST_CASE("test_removing_trend_lines", "[TrendLineModel]") {
  auto model = TrendLineModel();
  auto line1 = make_line(10, 10, 20, 20, Qt::red, TrendLineStyle::SOLID);
  auto id1 = model.add(line1);
  auto line2 = make_line(30, 30, 40, 40, Qt::red, TrendLineStyle::SOLID);
  auto id2 = model.add(line2);
  auto line3 = make_line(50, 50, 60, 60, Qt::red, TrendLineStyle::SOLID);
  auto id3 = model.add(line3);
  auto lines1 = model.get_lines();
  REQUIRE(lines1.size() == 3);
  REQUIRE(lines1[0] == line1);
  REQUIRE(lines1[1] == line2);
  REQUIRE(lines1[2] == line3);
  model.remove(id1);
  auto lines2 = model.get_lines();
  REQUIRE(lines2.size() == 2);
  REQUIRE(lines2[0] == line2);
  REQUIRE(lines2[1] == line3);
  model.remove(id2);
  auto lines3 = model.get_lines();
  REQUIRE(lines3.size() == 1);
  REQUIRE(lines3[0] == line3);
  model.remove(id3);
  auto lines4 = model.get_lines();
  REQUIRE(lines4.size() == 0);
  print_test_name("test_removing_trend_lines");
}

TEST_CASE("test_updating_and_getting_trend_lines", "[TrendLineModel]") {
  auto model = TrendLineModel();
  auto line1 = make_line(10, 10, 20, 20, Qt::red, TrendLineStyle::SOLID);
  auto id1 = model.add(line1);
  auto line2 = make_line(30, 30, 40, 40, Qt::red, TrendLineStyle::SOLID);
  auto id2 = model.add(line2);
  auto line3 = make_line(50, 50, 60, 60, Qt::red, TrendLineStyle::SOLID);
  auto id3 = model.add(line3);
  auto updated_line1 = model.get(id1);
  updated_line1.m_points = {make_point(110, 110), make_point(120, 120)};
  model.update(updated_line1, id1);
  auto updated_line2 = model.get(id2);
  updated_line2.m_points = {make_point(130, 130), make_point(140, 140)};
  model.update(updated_line2, id2);
  auto updated_line3 = model.get(id3);
  updated_line3.m_points = {make_point(150, 150), make_point(160, 160)};
  model.update(updated_line3, id3);
  REQUIRE(updated_line1 == model.get(id1));
  REQUIRE(updated_line2 == model.get(id2));
  REQUIRE(updated_line3 == model.get(id3));
  print_test_name("test_updating_and_getting_trend_lines");
}

TEST_CASE("test_setting and getting_selected_status", "[TrendLineModel]") {
  auto model = TrendLineModel();
  auto line1 = make_line(10, 10, 20, 20, Qt::red, TrendLineStyle::SOLID);
  auto id1 = model.add(line1);
  auto line2 = make_line(30, 30, 40, 40, Qt::red, TrendLineStyle::SOLID);
  auto id2 = model.add(line2);
  REQUIRE(model.get_selected().size() == 0);
  model.set_selected(id1);
  auto selected1 = model.get_selected();
  REQUIRE(selected1[0] == id1);
  REQUIRE(selected1.size() == 1);
  model.set_selected(id2);
  auto selected2 = model.get_selected();
  REQUIRE(selected2[0] == id1);
  REQUIRE(selected2[1] == id2);
  REQUIRE(selected2.size() == 2);
  model.unset_selected(id1);
  auto selected3 = model.get_selected();
  REQUIRE(selected3[0] == id2);
  REQUIRE(selected3.size() == 1);
  model.unset_selected(id2);
  auto selected4 = model.get_selected();
  REQUIRE(selected4.size() == 0);
  print_test_name("test_setting_and_getting_selected_status");
}

TEST_CASE("test_invert_selection", "[TrendLineModel]") {
  print_test_name("test_invert_selection");
}

TEST_CASE("test_clear_selection", "[TrendLineModel]") {
  print_test_name("test_clear_selection");
}

TEST_CASE("test_closest_basic", "[]") {
  print_test_name("test_closest_basic");
}

TEST_CASE("test_closest_vertical_line", "[TrendLineModel]") {
  print_test_name("test_closest_vertical_line");
}

TEST_CASE("test_closest_horizontal_line", "[TrendLineModel]") {
  print_test_name("test_closest_horizontal_line");
}
