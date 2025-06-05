#include "selfdrive/ui/qt/offroad/developer_panel.h"
#include "selfdrive/ui/qt/widgets/ssh_keys.h"
#include "selfdrive/ui/qt/widgets/controls.h"

DeveloperPanel::DeveloperPanel(SettingsWindow *parent) : ListWidget(parent) {
  adbToggle = new ParamControl("AdbEnabled", tr("Enable ADB"),
            tr("ADB (Android Debug Bridge) allows connecting to your device over USB or over the network. See https://docs.comma.ai/how-to/connect-to-comma for more info."), "");
  addItem(adbToggle);

  // SSH keys
  addItem(new SshToggle());
  addItem(new SshControl());

  joystickToggle = new ParamControl("JoystickDebugMode", tr("Joystick Debug Mode"), "", "");
  QObject::connect(joystickToggle, &ParamControl::toggleFlipped, [=](bool state) {
    params.putBool("LongitudinalManeuverMode", false);
    longManeuverToggle->refresh();
  });
  addItem(joystickToggle);

  longManeuverToggle = new ParamControl("LongitudinalManeuverMode", tr("Longitudinal Maneuver Mode"), "", "");
  QObject::connect(longManeuverToggle, &ParamControl::toggleFlipped, [=](bool state) {
    params.putBool("JoystickDebugMode", false);
    joystickToggle->refresh();
  });
  addItem(longManeuverToggle);

  experimentalLongitudinalToggle = new ParamControl(
    "AlphaLongitudinalEnabled",
    tr("openpilot Longitudinal Control (Alpha)"),
    QString("<b>%1</b><br><br>%2")
      .arg(tr("WARNING: openpilot longitudinal control is in alpha for this car and will disable Automatic Emergency Braking (AEB)."))
      .arg(tr("On this car, openpilot defaults to the car's built-in ACC instead of openpilot's longitudinal control. "
              "Enable this to switch to openpilot longitudinal control. Enabling Experimental mode is recommended when enabling openpilot longitudinal control alpha.")),
    ""
  );
  experimentalLongitudinalToggle->setConfirmation(true, false);
  QObject::connect(experimentalLongitudinalToggle, &ParamControl::toggleFlipped, [=]() {
    updateToggles(offroad);
  });
  addItem(experimentalLongitudinalToggle);

  is_release = false;

  QObject::connect(uiState(), &UIState::offroadTransition, this, &DeveloperPanel::updateToggles);
}

void DeveloperPanel::updateToggles(bool _offroad) {
  // 모든 버튼 항상 보이게, 항상 활성화
  for (auto btn : findChildren<ParamControl *>()) {
    btn->setVisible(true);
    btn->setEnabled(true);
  }

  // Long 및 Experimental 토글도 항상 활성화
  experimentalLongitudinalToggle->setVisible(true);
  experimentalLongitudinalToggle->setEnabled(true);
  longManeuverToggle->setVisible(true);
  longManeuverToggle->setEnabled(true);

  experimentalLongitudinalToggle->refresh();
  longManeuverToggle->refresh();

  offroad = _offroad;
}

void DeveloperPanel::showEvent(QShowEvent *event) {
  updateToggles(offroad);
}
