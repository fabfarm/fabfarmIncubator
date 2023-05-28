# Requires -Version 3.0 
Start-Process -Verb RunAs -FilePath "powershell.exe" -ArgumentList "usbipd wsl attach --busid 5-3"
