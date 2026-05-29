1. Abra o arquivo de serviço:
`sudo nano /etc/systemd/system/lcd.service`

2. Coloque este conteúdo:
```bash
Ini, TOML
[Unit]
Description=LCD and PY Code
After=network.target

[Service]
ExecStart=/usr/bin/python3 /home/rubico/code/main.py
WorkingDirectory=/home/rubico
StandardOutput=inherit
StandardError=inherit
Restart=on-failure
RestartSec=5
User=rubico

[Install]
WantedBy=multi-user.target
```

3. Ative o serviço definitivamente:

```bash
sudo systemctl daemon-reload
sudo systemctl enable lcd.service
sudo systemctl start lcd.service
```

4. Dependencias
```bash
sudo apt-get update
sudo apt-get install python3-pip python3-smbus i2c-tools python3-picamera2
sudo apt-get install python3-rpi.gpio
pip3 install rplcd pyserial pillow