import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import Style 1.0

import "../Controls2"
import "../Controls2/TextTypes"

ColumnLayout {
    id: root

    readonly property string hostName: hostnameField.textField.text
    readonly property string userName: usernameField.textField.text
    readonly property string secretData: secretDataField.textField.text

    function trimField(field) {
        field.textField.text = field.textField.text.replace(/^\s+|\s+$/g, '')
    }

    function isFilled() {
        var hasEmptyField = false

        if (hostnameField.textField.text === "") {
            hostnameField.errorText = qsTr("Ip address cannot be empty")
            hasEmptyField = true
        } else if (!hostnameField.textField.acceptableInput) {
            hostnameField.errorText = qsTr("Enter the address in the format 255.255.255.255:88")
        }

        if (usernameField.textField.text === "") {
            usernameField.errorText = qsTr("Login cannot be empty")
            hasEmptyField = true
        }

        if (secretDataField.textField.text === "") {
            secretDataField.errorText = qsTr("Password/private key cannot be empty")
            hasEmptyField = true
        }

        return !hasEmptyField
    }

    spacing: 12

    TextFieldType {
        id: hostnameField

        Layout.fillWidth: true

        headerText: qsTr("Server IP address [:port]")
        textField.placeholderText: qsTr("255.255.255.255:22")

        textField.onEditingFinished: root.trimField(hostnameField)
    }

    TextFieldType {
        id: usernameField

        Layout.fillWidth: true

        headerText: qsTr("SSH Username")
        textField.placeholderText: "root"

        textField.onEditingFinished: root.trimField(usernameField)
    }

    TextFieldType {
        id: secretDataField

        Layout.fillWidth: true

        headerText: qsTr("Password or SSH private key")
        passwordToggle: true

        textField.onEditingFinished: root.trimField(secretDataField)
    }

    NotificationType {
        Layout.fillWidth: true

        text: qsTr("SSH key requirements: supported key types are ED25519 and RSA in PEM format. Paste the private key, including the BEGIN/END lines. If your key doesn’t work, generate a compatible one")
    }
}
