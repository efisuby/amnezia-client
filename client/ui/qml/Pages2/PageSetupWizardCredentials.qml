import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import PageEnum 1.0
import Style 1.0

import "./"
import "../Controls2"
import "../Config"
import "../Components"
import "../Controls2/TextTypes"

PageType {
    id: root

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

                title: qsTr("Configure your server")
                description: qsTr("You'll need server credentials. Enter the server IP and password")
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.leftMargin: 16
                Layout.rightMargin: 16

                spacing: 24

                ServerCredentialsFields {
                    id: credentialsFields

                    Layout.fillWidth: true
                }

                ColumnLayout {
                    Layout.fillWidth: true

                    spacing: 12

                    BasicCardType {
                        id: hostLink

                        Layout.fillWidth: true

                        visible: GC.isDesktop() && !IsMacOsNeBuild

                        text: qsTr("Don't have a server?")
                        subtitle: qsTr("Amnezia Hosting - VPN servers without complicated settings")

                        icon.source: "qrc:/images/controls/amnezia.svg"

                        onClicked: {
                            Qt.openUrlExternally(LanguageUiController.getCurrentHostUrl())
                        }
                    }

                    BasicCardType {
                        id: siteLink

                        Layout.fillWidth: true

                        text: qsTr("How to run your VPN server")
                        subtitle: qsTr("Where to get connection data, step-by-step instructions for buying a VPS")

                        icon.source: "qrc:/images/controls/help-circle.svg"

                        onClicked: {
                            Qt.openUrlExternally(LanguageUiController.getCurrentSiteUrl("starter-guide"))
                        }
                    }
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
            id: continueButton

            Layout.fillWidth: true

            text: qsTr("Continue")

            onClicked: {
                if (!credentialsFields.isFilled()) {
                    return
                }

                InstallController.setProcessedServerCredentials(credentialsFields.hostName,
                                                                credentialsFields.userName,
                                                                credentialsFields.secretData)
                ServersUiController.setProcessedServerId("")

                PageController.showBusyIndicator(true)
                var isConnectionOpened = InstallController.checkSshConnection()
                PageController.showBusyIndicator(false)
                if (!isConnectionOpened) {
                    return
                }

                PageController.goToPage(PageEnum.PageSetupWizardEasy)
            }
        }

        CaptionTextType {
            Layout.fillWidth: true
            Layout.topMargin: 16
            Layout.bottomMargin: 12

            horizontalAlignment: Text.AlignHCenter

            text: qsTr("All data you enter will remain strictly confidential and will not be shared or disclosed to the Amnezia or any third parties")
        }
    }
}
