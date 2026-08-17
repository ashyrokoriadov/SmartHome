$python = "C:\Users\andre\AppData\Local\Programs\Python\Python313\python.exe"
$scriptDir = "D:\HomeAssistantDataSources\PyScripts"

Write-Host "Waiting for Mosquitto..."

while (-not (Test-NetConnection -ComputerName localhost -Port 1883 -InformationLevel Quiet)) {
    Start-Sleep -Seconds 2
}

Write-Host "Mosquitto is available."

Start-Process `
    -FilePath $python `
    -ArgumentList "$scriptDir\sensor_read_xiaomi_temperature_mqtt.py" `
    -WorkingDirectory $scriptDir `
    -WindowStyle Hidden

Start-Process `
    -FilePath $python `
    -ArgumentList "$scriptDir\sensor_read_victron_mqtt.py" `
    -WorkingDirectory $scriptDir `
    -WindowStyle Hidden
	
Start-Process `
    -FilePath $python `
    -ArgumentList "$scriptDir\humidifier_read_data_mqtt.py" `
    -WorkingDirectory $scriptDir `
    -WindowStyle Hidden

Write-Host "Data source scripts started."