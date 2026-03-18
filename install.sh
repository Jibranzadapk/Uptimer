#!/bin/bash

echo "Installing Uptimer..."

make

#!/bin/bash

TARGET="/usr/local/bin/uptimer"

if [ -d "/usr/local/" ]; then

    if [ -d "/usr/local/bin" ]; then
        sudo cp uptimer "$TARGET"
    else
        sudo mkdir -p /usr/local/bin
        sudo cp uptimer "$TARGET"
    fi

else
    sudo mkdir -p /usr/local/bin
    sudo cp uptimer "$TARGET"
fi

sudo chmod +x /usr/local/bin/uptimer

sudo mkdir -p /var/lib/uptimer
sudo chmod 755 /var/lib/uptimer

sudo cp uptimer.service /etc/systemd/system/uptimer.service

sudo systemctl daemon-reload
sudo systemctl enable uptimer
sudo systemctl start uptimer

echo "Done! Now just type: uptimer"
