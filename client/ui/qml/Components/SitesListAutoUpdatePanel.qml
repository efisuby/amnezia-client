import QtQuick
import QtQuick.Layouts

import "../Controls2"

ColumnLayout {
    id: root

    spacing: 12

    SwitchRowType {
        Layout.fillWidth: true

        text: qsTr("Load site list from link on connect")
        descriptionText: qsTr("The list of sites accessed via VPN will be replaced with the downloaded one")
        checked: IpSplitTunnelingController.isSitesListAutoUpdateEnabled

        onToggled: IpSplitTunnelingController.isSitesListAutoUpdateEnabled = checked
    }

    TextFieldType {
        Layout.fillWidth: true

        visible: IpSplitTunnelingController.isSitesListAutoUpdateEnabled
        headerText: qsTr("Site list link")
        textField.text: IpSplitTunnelingController.sitesListUrl
        textField.inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText

        textField.onEditingFinished: {
            if (textField.text !== IpSplitTunnelingController.sitesListUrl) {
                IpSplitTunnelingController.sitesListUrl = textField.text
            }
        }
    }
}
