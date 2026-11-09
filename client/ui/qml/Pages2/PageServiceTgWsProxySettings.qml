import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import PageEnum 1.0
import ProtocolEnum 1.0
import Style 1.0

import "./"
import "../Controls2"
import "../Controls2/TextTypes"
import "../Config"
import "../Components"

PageType {
    id: root

    readonly property string hostName: ServersUiController.serverHostName(ServersUiController.processedServerId)
    readonly property string proxyLink: {
        TgWsProxyConfigModel.secret
        TgWsProxyConfigModel.port
        TgWsProxyConfigModel.fakeTlsDomain
        return TgWsProxyConfigModel.proxyLink(root.hostName)
    }

    function copy(value) {
        GC.copyToClipBoard(value)
        PageController.showNotificationMessage(qsTr("Copied"))
    }

    Connections {
        target: InstallController

        function onUpdateContainerFinished() {
            PageController.showNotificationMessage(qsTr("Settings updated successfully"))
        }
    }

    FlickableType {
        id: flickable

        anchors.top: parent.top
        anchors.bottom: changeSettingsButton.top
        anchors.bottomMargin: 24

        contentHeight: content.implicitHeight

        ColumnLayout {
            id: content

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            spacing: 0

            NavigationBarType {
                Layout.fillWidth: true

                title: qsTr("tg-ws-proxy settings")
            }

            ListItemType {
                Layout.fillWidth: true
                Layout.leftMargin: 16
                Layout.rightMargin: 16

                dense: true
                overline: qsTr("Proxy link")
                text: root.proxyLink
                maximumLineCount: 3
                Accessible.name: overline

                trailingIconSource: "qrc:/images/controls/copy.svg"

                onClicked: root.copy(root.proxyLink)
            }

            ListItemType {
                Layout.fillWidth: true
                Layout.leftMargin: 16
                Layout.rightMargin: 16

                dense: true
                overline: qsTr("Host")
                text: root.hostName
                Accessible.name: overline + " " + text

                trailingIconSource: "qrc:/images/controls/copy.svg"

                onClicked: root.copy(text)
            }

            ListItemType {
                Layout.fillWidth: true
                Layout.leftMargin: 16
                Layout.rightMargin: 16

                dense: true
                overline: qsTr("Port")
                text: TgWsProxyConfigModel.port
                Accessible.name: overline + " " + text

                trailingIconSource: "qrc:/images/controls/copy.svg"

                onClicked: root.copy(text)
            }

            ListItemType {
                Layout.fillWidth: true
                Layout.leftMargin: 16
                Layout.rightMargin: 16

                dense: true
                overline: qsTr("FakeTLS domain")
                text: TgWsProxyConfigModel.fakeTlsDomain !== "" ? TgWsProxyConfigModel.fakeTlsDomain : qsTr("Off")
                Accessible.name: overline + " " + text
            }

            ListItemType {
                id: secretLabel

                property bool masked: true

                Layout.fillWidth: true
                Layout.leftMargin: 16
                Layout.rightMargin: 16

                showSeparator: false
                dense: true
                overline: qsTr("Secret")
                text: masked ? "*".repeat(TgWsProxyConfigModel.secret.length) : TgWsProxyConfigModel.secret
                Accessible.name: overline

                trailingIconSource: "qrc:/images/controls/copy.svg"

                onClicked: root.copy(TgWsProxyConfigModel.secret)

                IconButtonType {
                    icon.source: secretLabel.masked ? "qrc:/images/controls/eye.svg" : "qrc:/images/controls/eye-off.svg"
                    Accessible.name: secretLabel.masked ? qsTr("Show secret") : qsTr("Hide secret")

                    onClicked: secretLabel.masked = !secretLabel.masked
                }
            }

            SheetType {
                id: changeSettingsDrawer

                parent: root

                expandedHeight: drawerContent.implicitHeight + 16 + PageController.safeAreaBottomMargin + PageController.imeHeight

                ColumnLayout {
                    id: drawerContent

                    property string tempPort: TgWsProxyConfigModel.port
                    property string tempFakeTlsDomain: TgWsProxyConfigModel.fakeTlsDomain

                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: 16
                    anchors.rightMargin: 16

                    spacing: 12

                    Connections {
                        target: changeSettingsDrawer

                        function onOpened() {
                            drawerContent.tempPort = TgWsProxyConfigModel.port
                            drawerContent.tempFakeTlsDomain = TgWsProxyConfigModel.fakeTlsDomain
                        }

                        function onClosed() {
                            TgWsProxyConfigModel.port = drawerContent.tempPort
                            TgWsProxyConfigModel.fakeTlsDomain = drawerContent.tempFakeTlsDomain
                            portTextField.textField.text = TgWsProxyConfigModel.port
                            fakeTlsDomainTextField.textField.text = TgWsProxyConfigModel.fakeTlsDomain
                        }
                    }

                    SectionHeaderType {
                        Layout.fillWidth: true
                        Layout.topMargin: 16
                        Layout.bottomMargin: 20

                        headerText: qsTr("tg-ws-proxy settings")
                    }

                    TextFieldType {
                        id: portTextField

                        Layout.fillWidth: true

                        headerText: qsTr("Port")
                        textField.text: TgWsProxyConfigModel.port
                        textField.maximumLength: 5
                        textField.validator: IntValidator { bottom: 1; top: 65535 }

                        textField.onTextEdited: TgWsProxyConfigModel.port = textField.text
                    }

                    TextFieldType {
                        id: fakeTlsDomainTextField

                        Layout.fillWidth: true

                        headerText: qsTr("FakeTLS domain")
                        textField.placeholderText: qsTr("Leave empty to turn off FakeTLS")
                        textField.text: TgWsProxyConfigModel.fakeTlsDomain
                        textField.validator: RegularExpressionValidator { regularExpression: /^$|^[A-Za-z0-9]([A-Za-z0-9-]*[A-Za-z0-9])?(\.[A-Za-z0-9]([A-Za-z0-9-]*[A-Za-z0-9])?)+$/ }

                        textField.onTextEdited: TgWsProxyConfigModel.fakeTlsDomain = textField.text
                    }

                    ButtonType {
                        Layout.fillWidth: true
                        Layout.topMargin: 12

                        text: qsTr("Change connection settings")

                        onClicked: {
                            if (!portTextField.textField.acceptableInput) {
                                portTextField.errorText = qsTr("The port must be in the range of 1 to 65535")
                                return
                            }
                            if (!fakeTlsDomainTextField.textField.acceptableInput) {
                                fakeTlsDomainTextField.errorText = qsTr("Enter a domain name, for example dzen.ru")
                                return
                            }

                            PageController.goToPage(PageEnum.PageSetupWizardInstalling)
                            InstallController.updateServerConfig(ServersUiController.processedServerId, ServersUiController.processedContainerIndex, ProtocolEnum.TgWsProxy)
                            drawerContent.tempPort = portTextField.textField.text
                            drawerContent.tempFakeTlsDomain = fakeTlsDomainTextField.textField.text
                            changeSettingsDrawer.close()
                        }
                    }
                }
            }
        }
    }

    ButtonType {
        id: changeSettingsButton

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: 16
        anchors.rightMargin: 16
        anchors.bottomMargin: 8 + PageController.safeAreaBottomMargin

        text: qsTr("Change connection settings")

        onClicked: changeSettingsDrawer.open()
    }
}
