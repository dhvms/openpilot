#include "ntunepannel.h"
#include "ntunewidget.h"
#include <QStringListModel>
#include <QList>
#include <QHBoxLayout>
#include <QFont>
#include <QDebug>

QList<QString> nTuneMainWidget::mainTitles = {
    "General", "SCC", "Torque"
    };

QList<QList<TuneItemInfo>> nTuneMainWidget::mainItems = {
{
    TuneItemInfo("common.json", "cameraOffset", "우측이동 - , 좌측이동 +",
                 0.06f, -1.0f, 1.0f, 0.01f, 2),
    TuneItemInfo("common.json", "pathOffset", "If oversteer occurs in a corner, reduce it.",
                 0.0f, -2.0f, 2.0f, 0.1f, 1),
    TuneItemInfo("common.json", "autoLaneChangeSpeed", "해당속도이상에서 자동차선변경",
                 30.0f, 10.0f, 60.0f, 5.0f, 1, "km"),
    TuneItemInfo("common.json", "steerActuatorDelay", "값이크면 조향을 좀더 미리함",
                 0.3f, 0.0f, 0.8f, 0.05f, 2, "sec"),
},
{
    TuneItemInfo("scc_v3.json", "aTargetFactor", tr("값이 클수록 앞차 반응에 민감."),
                 1.0f, 0.7f, 1.5f, 0.05f, 2),
    TuneItemInfo("scc_v3.json", "longitudinalActuatorDelay", tr("Longitudinal Actuator Delay"),
                 0.5f, 0.1f, 1.0f, 0.05f, 2, "sec"),
    TuneItemInfo("scc_v3.json", "longitudinalTuningkf", tr("값이 클수록 응답성이 좋아짐 "),
                 1.0f, 0.0f, 2.0f, 0.05f, 2),
    TuneItemInfo("scc_v3.json", "stopAccel", tr(""),
                 -2.0f, -5.0f, 0.0f, 0.05f, 2),
    TuneItemInfo("scc_v3.json", "startAccel", tr(""),
                 1.5f, 0.0f, 5.0f, 0.05f, 2),
    TuneItemInfo("scc_v3.json", "stoppingDecelRate", tr(""),
                 0.3f, 0.1f, 1.0f, 0.05f, 2),
    TuneItemInfo("scc_v3.json", "vEgoStopping", tr(""),
                 0.3f, 0.1f, 1.0f, 0.05f, 2),
    TuneItemInfo("scc_v3.json", "vEgoStarting", tr(""),
                 0.3f, 0.1f, 1.0f, 0.05f, 2),
    TuneItemInfo("scc_v3.json", "comfortBrake", tr("값이 크면 급하게 브레이킹."),
                 2.6f, 2.3f, 2.7f, 0.1f, 1, "m/s²"),
    TuneItemInfo("scc_v3.json", "stopDistance", tr("정지시 앞차와의 거리."),
                 6.0f, 4.0f, 7.0f, 0.1f, 1, "m"),
},
{
    TuneItemInfo("lat_torque_v4.json", "latAccelFactor", "", 2.5f, 0.5f, 4.5f, 0.1f, 2),
    TuneItemInfo("lat_torque_v4.json", "friction", "", 0.1f, 0.0f, 0.2f, 0.01f, 3),
},
};

nTuneMainWidget::nTuneMainWidget(QWidget *parent)
    : QWidget{parent} {

    setStyleSheet(R"(
        * {
          color: white;
          font-size: 55px;
        }
        SettingsWindow {
          background-color: black;
        }
        QStackedWidget, ScrollView, QListView {
          background-color: #292929;
          border-radius: 0px;
        }
        QTabBar::tab { padding: 10px; border-radius: 10px; background-color: transparent; border-bottom: none;}
        QTabBar::tab:selected { background-color: #555555; border-bottom: none;}
        QTabBar::tab:!selected { background-color: transparent; border-bottom: none;}
        QComboBox {
            background-color: transparent;
            border: 2px solid #555555;
            border-radius: 5px;
            font-size: 50px;
        }
        QComboBox QAbstractItemView {
            background: transparent;
            selection-background-color: #555555;
            font-size: 50px;
        }
    )");

    QHBoxLayout *layout = new QHBoxLayout(this);

    QStringList data;
    foreach (auto title, mainTitles) {
        data.append(title);
    }

    QStringListModel *model = new QStringListModel(data);
    listView = new QListView();
    listView->setStyleSheet(R"(
        QListView::item {  padding-top: 30px; padding-bottom: 30px; padding-left: 15px; padding-right: 15px;
            border-radius: 10px; font-weight: bold;}
        QListView::item:selected { background-color: #555555; }
        QListView::item:!selected { background-color: #00000000; }
    )");
    listView->setModel(model);
    BoldItemDelegate *delegate = new BoldItemDelegate(listView);
    listView->setItemDelegate(delegate);
    listView->setSelectionMode(QAbstractItemView::SingleSelection);
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QItemSelectionModel *selectionModel = listView->selectionModel();
    selectionModel->select(model->index(0, 0), QItemSelectionModel::Select);

    stackedWidget = new QStackedWidget;
    stackedWidget->setStyleSheet("QWidget { background-color: #404040; border-radius: 10px; }");
    foreach (auto items, mainItems) {
        stackedWidget->addWidget(new nTunePannel(items));
    }

    layout->addWidget(listView, 1);
    layout->addWidget(stackedWidget, 4);

    this->setLayout(layout);

    QObject::connect(listView->selectionModel(), &QItemSelectionModel::currentChanged,
                     [&](const QModelIndex &current, const QModelIndex &previous) {
                         qDebug() << "Selected item:" << current.row();
                         this->stackedWidget->setCurrentIndex(current.row());
                     });
}

bool nTuneMainWidget::checkFilesExist() {
    for (const auto& itemList : mainItems) {
        for (const auto& item : itemList) {
            QFile file(item.fullPath());
            if (!file.exists()) {
                return false;
            }
        }
    }
    return true;
}

nTunePannel::nTunePannel(QList<TuneItemInfo>& items, QWidget *parent)
    : QWidget{parent} {

    QVBoxLayout *layout = new QVBoxLayout(this);
    tabBar = new QTabBar;
    tabBar->setStyleSheet(R"(
        QTabBar::tab { font-weight: bold; font-size: 45px; }
    )");
    stackedWidget = new QStackedWidget;
    stackedWidget->setStyleSheet(R"(
        padding: 20px;
    )");

    foreach (const TuneItemInfo item, items) {
        tabBar->addTab(item.key);
        stackedWidget->addWidget(new nTuneWidget(item));
    }

    layout->addWidget(tabBar);
    layout->addWidget(stackedWidget);

    connect(tabBar, &QTabBar::currentChanged, stackedWidget, &QStackedWidget::setCurrentIndex);

    setMouseTracking(true);
}
