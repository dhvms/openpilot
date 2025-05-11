#include "selfdrive/ui/qt/onroad/buttons.h"

#include <QPainter>

#include "selfdrive/ui/qt/util.h"

void drawIcon(QPainter &p, const QPoint &center, const QPixmap &img, const QBrush &bg, float opacity, float angle) {
  p.setRenderHint(QPainter::Antialiasing);
  p.setOpacity(1.0);
  p.setPen(Qt::NoPen);
  p.setBrush(bg);
  p.drawEllipse(center, btn_size / 2, btn_size / 2);

  p.setOpacity(opacity);

  if (angle != 0.0f) {
    p.save();
    p.translate(center);
    p.rotate(-angle);
    p.drawPixmap(-QPoint(img.width() / 2, img.height() / 2), img);
    p.restore();
  } else {
    p.drawPixmap(center - QPoint(img.width() / 2, img.height() / 2), img);
  }

  p.setOpacity(1.0);
}

void drawIconGradient(QPainter &p, const QPoint &center, const QPixmap &img, const QBrush &bg, float opacity, float angle) {
  p.setRenderHint(QPainter::Antialiasing);
  p.setOpacity(1.0);
  p.setPen(Qt::NoPen);
  p.setBrush(bg);
  p.drawEllipse(center, btn_size / 2, btn_size / 2);

  int borderThickness = 10;
  int adjustedRadius = btn_size / 2 - borderThickness / 2;

  int startAngle = 90 * 16;
  int spanAngle = angle * 16;
  int extraAngle = angle > 0 ? (angle - 360) * 16 : (angle + 360) * 16;

  if (angle != 0) {
    QConicalGradient gradient(center, 90);

    if (angle > 0) {
      gradient.setColorAt(0.0f, limeColor(200));
      gradient.setColorAt(0.5f, orangeColor(200));
      gradient.setColorAt(1.0f, redColor(200));
    } else {
      gradient.setColorAt(0.0f, redColor(200));
      gradient.setColorAt(0.5f, orangeColor(200));
      gradient.setColorAt(1.0f, limeColor(200));
    }

    p.setPen(QPen(QBrush(gradient), borderThickness));
    p.setBrush(Qt::NoBrush);
    p.drawArc(QRect(center.x() - adjustedRadius, center.y() - adjustedRadius, adjustedRadius * 2, adjustedRadius * 2), startAngle, spanAngle);
  }

  if (qAbs(angle) > 360) {
    p.setPen(QPen(QBrush(darkRedColor(200)), borderThickness));
    p.setBrush(Qt::NoBrush);
    p.drawArc(QRect(center.x() - adjustedRadius, center.y() - adjustedRadius, adjustedRadius * 2, adjustedRadius * 2), startAngle, extraAngle);
  }

  p.setOpacity(opacity);
  p.save();
  p.translate(center);
  p.rotate(-angle);
  p.drawPixmap(-QPoint(img.width() / 2, img.height() / 2), img);
  p.restore();
  p.setOpacity(1.0);
}

// ExperimentalButton
ExperimentalButton::ExperimentalButton(QWidget *parent) : experimental_mode(false), engageable(false), QPushButton(parent) {
  setFixedSize(btn_size, btn_size);

  engage_img = loadPixmap("../assets/img_experimental_white.svg", {img_size, img_size});
  experimental_img = loadPixmap("../assets/img_experimental.svg", {img_size, img_size});
  QObject::connect(this, &QPushButton::clicked, this, &ExperimentalButton::changeMode);
}

void ExperimentalButton::changeMode() {
  const auto cp = (*uiState()->sm)["carParams"].getCarParams();
  bool can_change = hasLongitudinalControl(cp) && params.getBool("ExperimentalModeConfirmed");
  if (can_change) {
    params.putBool("ExperimentalMode", !experimental_mode);
  }
}

void ExperimentalButton::updateState(const UIState &s) {
  const auto cs = (*s.sm)["selfdriveState"].getSelfdriveState();
  bool eng = cs.getEngageable() || cs.getEnabled();
  if ((cs.getExperimentalMode() != experimental_mode) || (eng != engageable)) {
    engageable = eng;
    experimental_mode = cs.getExperimentalMode();
    update();
  }
}

void ExperimentalButton::paintEvent(QPaintEvent *event) {
  QPainter p(this);
  QPixmap img = experimental_mode ? experimental_img : engage_img;
  drawIcon(p, QPoint(btn_size / 2, btn_size / 2), img, QColor(0, 0, 0, 100), 0.8);
}
