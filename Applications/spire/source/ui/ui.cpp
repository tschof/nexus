#include "spire/ui/ui.hpp"
#include <QPainter>
#include <QtSvg/QSvgRenderer>

using namespace spire;

QImage spire::imageFromSvg(const QString& path, const QSize& size) {
  return imageFromSvg(path, size, QRect(0, 0, size.width(), size.height()));
}

QImage spire::imageFromSvg(const QString& path, const QSize& size,
    const QRect& box) {
  QSvgRenderer renderer(path);
  auto image = QImage(size, QImage::Format_ARGB32);
  image.fill(QColor(0, 0, 0, 0));
  QPainter painter(&image);
  renderer.render(&painter, box);
  return image;
}