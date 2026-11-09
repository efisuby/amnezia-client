import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import SortFilterProxyModel 0.2

import PageEnum 1.0
import Style 1.0

import "./"
import "../Controls2"
import "../Controls2/TextTypes"
import "../Config"

PageType {
    id: root

    Component.onCompleted: PageController.disableTabBar(true)
    Component.onDestruction: PageController.disableTabBar(false)

    property string headerText: qsTr("Removing services from %1")
    property string progressText: qsTr("Usually it takes no more than 5 minutes")

    Connections {
        target: InstallController

        function onServerMigrationProgress(containerName, index, total) {
            root.progressText = qsTr("Moving %1 (%2 of %3)").arg(containerName).arg(index).arg(total)
        }
    }

    SortFilterProxyModel {
        id: proxyServersModel

        sourceModel: ServersModel

        filters: [
            ValueFilter {
                roleName: "serverId"
                value: ServersUiController.processedServerId
            }
        ]
    }

    ListViewType {
        id: listView

        anchors.fill: parent

        model: proxyServersModel

        delegate: ColumnLayout {
            width: listView.width

            spacing: 0

            NavigationBarType {
                Layout.fillWidth: true

                backButtonVisible: false
                title: root.headerText.arg(name)
            }

            ProgressBarType {
                id: progressBar

                Layout.fillWidth: true
                Layout.leftMargin: 16
                Layout.rightMargin: 16

                Timer {
                    id: timer

                    interval: 300
                    repeat: true
                    running: true
                    onTriggered: {
                        progressBar.value += 0.003
                    }
                }
            }

            SmallTextType {
                Layout.fillWidth: true
                Layout.topMargin: 8
                Layout.leftMargin: 16
                Layout.rightMargin: 16

                text: root.progressText
            }
        }
    }
}
