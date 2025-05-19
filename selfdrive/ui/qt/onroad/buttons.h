#pragma once

#include <QPushButton>

#include "selfdrive/ui/ui.h"

//const int btn_size = 192;
//const int img_size = (btn_size / 4) * 3;
const int btn_size = 160;
const int img_size = (btn_size / 2) * 1.5;

class ExperimentalButton : public QPushButton {
  Q_OBJECT

public:
  explicit ExperimentalButton(QWidget *parent = 0);
  void updateState(const UIState &s);

private:
  void paintEvent(QPaintEvent *event) override;
  void changeMode();

  Params params;
  QPixmap engage_img;
  QPixmap experimental_img;
  bool experimental_mode;
  bool engageable;
};

void drawIcon(QPainter &p, const QPoint &center, const QPixmap &img, const QBrush &bg, float opacity, float angle = 0.0f);
void drawIconGradient(QPainter &p, const QPoint &center, const QPixmap &img, const QBrush &bg, float opacity, float angle);

inline QColor limeColor(int alpha = 255) { return QColor(120, 255, 120, alpha); }
inline QColor orangeColor(int alpha = 255) { return QColor(255, 149, 0, alpha); }
inline QColor redColor(int alpha = 255) { return QColor(201, 34, 49, alpha); }
inline QColor darkRedColor(int alpha = 255) { return QColor(139, 0, 0, alpha); }
