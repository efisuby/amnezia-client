import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import PageEnum 1.0
import Style 1.0

import "./"
import "../Controls2"
import "../Controls2/TextTypes"
import "../Components"
import "../Config"

PageType {
    id: root

    readonly property bool isServerAddedByIp:
        /^\d{1,3}(\.\d{1,3}){3}$/.test(ServersUiController.serverHostName(ServersUiController.processedServerId))

    function migrateServer() {
        InstallController.setProcessedServerCredentials(credentialsFields.hostName,
                                                        credentialsFields.userName,
                                                        credentialsFields.secretData)

        PageController.showBusyIndicator(true)
        var isConnectionOpened = InstallController.checkSshConnection()
        PageController.showBusyIndicator(false)
        if (!isConnectionOpened) {
            return
        }

        PageController.goToPage(PageEnum.PageDeinstalling, true, {
                                    "headerText": qsTr("Moving %1 to another server"),
                                    "progressText": qsTr("It may take several minutes")
                                })
        InstallController.migrateServer(ServersUiController.processedServerId)
    }

    Connections {
        target: InstallController

        function onServerMigrated(finishedMessage) {
            PageController.closePage()
            PageController.closePage()
            PageController.showNotificationMessage(finishedMessage)
        }
    }

    FlickableType {
        id: flickable

        anchors.top: parent.top
        anchors.bottom: bottomBar.top

        contentHeight: content.implicitHeight + 16

        ColumnLayout {
            id: content

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            spacing: 0

            NavigationBarType {
                Layout.fillWidth: true

                title: qsTr("Move to another server")
                description: qsTr("Protocols and services are installed on the new server with the same settings, keys and users. IPsec is not moved, SFTP files are not copied. The current server is not changed")
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.leftMargin: 16
                Layout.rightMargin: 16

                spacing: 24

                NotificationType {
                    Layout.fillWidth: true

                    text: root.isServerAddedByIp
                          ? qsTr("This server was added by IP address. After the move, users will need new connection keys")
                          : qsTr("After the move, point the server domain to the new server IP address. Users will connect without changing their keys")
                }

                ServerCredentialsFields {
                    id: credentialsFields

                    Layout.fillWidth: true
                }
            }
        }
    }

    BottomBarType {
        id: bottomBar

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        filled: true

        ButtonType {
            Layout.fillWidth: true

            text: qsTr("Move")

            onClicked: {
                if (!credentialsFields.isFilled()) {
                    return
                }

                if (ServersUiController.isDefaultServerCurrentlyProcessed() && ConnectionController.isConnected) {
                    PageController.showNotificationMessage(qsTr("Cannot move server during active connection"))
                    return
                }

                var headerText = qsTr("Do you want to move the server?")
                var descriptionText = qsTr("Amnezia services already installed on the new server will be reinstalled.")
                var yesButtonText = qsTr("Continue")
                var noButtonText = qsTr("Cancel")

                var noButtonFunction = function() {

                }

                Dialogs.askQuestion(headerText, descriptionText, yesButtonText, noButtonText, root.migrateServer, noButtonFunction)
            }
        }
    }
}
