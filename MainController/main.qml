import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2

Window
{
    visible: true
    minimumHeight: 500
    minimumWidth: 250
    maximumHeight: 500
    maximumWidth: 250
    title: "Smart Home Controller"

    Dialog
    {
        id: dialog_WrongIP
        title: "Error"
        Text { text: "You provided wrong IP in textfield!" }
    }

    Dialog
    {
        id: dialog_Settings

        function checkIP(ip)
        {
            var dots = 0;
            var digits = 0;
            var providePort = false;

            for (var i = 0; i < ip.length; i++)
            {
                if (!isNaN(ip[i]))
                    digits++;

                else if (ip[i] === '.' && !providePort)
                {
                    dots++;

                    if (digits === 0 || digits > 3 || dots > 3)
                        return false;

                    digits = 0;
                }

                else if (ip[i] === ':' && !providePort)
                    providePort = true;

                else
                    return false;
            }

            if (dots !== 3 || (digits > 3 && !providePort))
                return false;

            else if (!providePort)
                return false;

            return true;
        }

        title: "Connection settings"

        ColumnLayout
        {
            Label
            {
                text: "Blinds IP"
            }

            TextField
            {
                id: blindsIPField
                placeholderText: "123.45.67.89:1111"
            }

            Label
            {
                text: "Central Heating IP"
            }

            TextField
            {
                id: heatingIPField
                placeholderText: "123.45.67.89:1111"
            }
        }

        onAccepted:
        {
            if ((blindsIPField.text.length != 0 && !checkIP(blindsIPField.text)) ||
                    (heatingIPField.text.length != 0 && !checkIP(heatingIPField.text)))
            {
                blindsIPField.text = ""
                heatingIPField.text = ""
                dialog_WrongIP.open()
            }

            else
            {
                GuiController.setBlindsIP(blindsIPField.text)
                GuiController.setHeatingIP(heatingIPField.text)
            }
        }
    }

    Dialog
    {
        id: dialog_Status
        title: "Connection status"

        GridLayout
        {
            columns: 2

            Label
            {
                text: "Central heating: "
            }

            Label
            {
                property bool heatingStatus: GuiController.heatingConnected
                color: "red"
                text: "Not connected"

                onHeatingStatusChanged:
                {
                    if (heatingStatus)
                    {
                        color = "green"
                        text = "Connected"
                    }

                    else
                    {
                        color = "red"
                        text = "Not connected"
                    }
                }
            }

            Label
            {
                text: "Blinds: "
            }

            Label
            {
                property bool blindsConnected: GuiController.blindsConnected
                color: "red"
                text: "Not connected"

                onBlindsConnectedChanged:
                {
                    if (blindsConnected)
                    {
                        color = "green"
                        text = "Connected"
                    }

                    else
                    {
                        color = "red"
                        text = "Not connected"
                    }
                }
            }
        }
    }

    MenuBar
    {
        Menu
        {
            title: "Settings"

            MenuItem
            {
                text: "Connection parameters"
                onClicked: dialog_Settings.open()
            }

            MenuItem
            {
                text: "Connection status"
                onClicked: dialog_Status.open()
            }
        }
    }

    Item
    {
        id: mainWindow
        anchors.fill: parent
        anchors.margins: 10
        anchors.topMargin: 50

        RowLayout
        {
            id: firstRow
            spacing: 40
            Label
            {
                id: statusLabel
                color: "red"
                text: "Turned off"
            }

            Switch
            {
                id: mainSwitch

                onPositionChanged:
                {
                    if (position == 1)
                    {
                        statusLabel.color = "green"
                        statusLabel.text = "Turned on"

                        secondRow.visible = true
                        thirdRow.visible = true
                    }

                    else
                    {
                        statusLabel.color = "red"
                        statusLabel.text = "Turned off"

                        secondRow.visible = false
                        thirdRow.visible = false
                    }
                }
            }
        }

        ColumnLayout
        {
            id: secondRow
            anchors.top: firstRow.bottom
            anchors.topMargin: 30
            visible: false
            enabled: GuiController.blindsConnected

            Label
            {
                text: "<b>Blinds<b>"
                font.pointSize: 15
            }

            Label
            {
                id: blindsStatus
                Layout.topMargin: 20
                text: "Current coverage: " + GuiController.currentCoverage + "%"
            }

            Label
            {
                text: "Desired coverage: " + GuiController.desiredCoverage + "%"
            }

            Slider
            {
                id: blindsSlider
                from: 0
                stepSize: 1.0
                to: 100
                onPressedChanged:
                {
                    if (!pressed)
                        GuiController.setDesiredCoverage(value)
                }
            }
        }

        ColumnLayout
        {
            id: thirdRow
            anchors.top: secondRow.bottom
            anchors.topMargin: 30
            visible: false
            enabled: GuiController.heatingConnected

            RowLayout
            {
                Label
                {
                    text: "<b>Central heating<b>"
                    font.pointSize: 15
                }
            }

            RowLayout
            {
                Label
                {
                    id: heaterInfo
                    text: "Status: Turned off"
                    color: "red"
                }

                Switch
                {
                    id: heaterSwitch

                    position: GuiController.heatingOn
                    onPositionChanged:
                    {
                        if (position == 1)
                        {
                            heaterInfo.text = "Status: Turned on"
                            heaterInfo.color = "green"
                            heaterStatus.visible = true
                            desiredTempStatus.visible = true
                            tempSlider.visible = true
                            GuiController.setHeatingOn(true)
                        }

                        else
                        {
                            heaterInfo.text = "Status: Turned off"
                            heaterInfo.color = "red"
                            heaterStatus.visible = false
                            desiredTempStatus.visible = false
                            tempSlider.visible = false
                            GuiController.setHeatingOn(false)
                        }
                    }

                }
            }

            Label
            {
                id: heaterStatus
                Layout.topMargin: 20
                text: "Current Temperature: " + GuiController.currentTemp + "°C"
                visible: GuiController.heatingOn
            }

            Label
            {
                id: desiredTempStatus
                text: "Desired temperature: " + tempSlider.value + "°C"
                visible: GuiController.heatingOn
            }

            Slider
            {
                id: tempSlider
                from: 30
                stepSize: 1.0
                to: 80
                visible: GuiController.heatingOn
                Component.onCompleted: value = GuiController.desiredTemp
                onVisibleChanged: value = GuiController.desiredTemp
                onPressedChanged:
                {
                    if (pressed == false)
                        GuiController.setDesiredTemp(value)
                }
            }
        }
    }

    Connections
    {
       target: GuiController
       onDesiredTempChanged: tempSlider.value = GuiController.desiredTemp
    }
}
