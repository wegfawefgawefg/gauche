#!/usr/bin/env bash
set -euo pipefail

# DEPLOY: Run on a fresh Ubuntu 24.04 VPS with the binary and service beside this script.
roomd_ip=${1:?Usage: install-server.sh PUBLIC_IPV4}
python3 -c 'import ipaddress,sys; ipaddress.IPv4Address(sys.argv[1])' "$roomd_ip"
roomd_files=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
test "$(id -u)" = 0
export DEBIAN_FRONTEND=noninteractive
apt-get update -qq
apt-get install -y -qq nginx ufw python3-venv ca-certificates

# FIREWALL: The plain HTTP backend and diagnostic routes are never public.
ufw allow 22/tcp
ufw allow 80/tcp
ufw allow 443/tcp
ufw allow 8789/udp
ufw allow 8790/udp
ufw default deny incoming
ufw default allow outgoing
ufw --force enable
printf 'PasswordAuthentication no\nPermitRootLogin prohibit-password\n' > /etc/ssh/sshd_config.d/00-gauche-key-only.conf
sshd -t
systemctl reload ssh

# SERVICE: The public bind address is also the advertised punch/relay address.
id gauche-roomd >/dev/null 2>&1 || useradd --system --no-create-home --shell /usr/sbin/nologin gauche-roomd
install -d /opt/gauche-roomd
install -m 755 "$roomd_files/gubsy-roomd" /opt/gauche-roomd/gubsy-roomd
install -m 644 "$roomd_files/gauche-roomd.service" /etc/systemd/system/gauche-roomd.service
printf 'ROOMD_PUBLIC_IP=%s\n' "$roomd_ip" > /etc/gauche-roomd.env
systemctl daemon-reload
systemctl enable --now gauche-roomd

# HTTP: Only certificate challenges are available until TLS is installed.
install -d /var/www/gauche-acme
cat > /etc/nginx/sites-available/gauche-roomd <<EOF
server {
    listen 80 default_server;
    server_name $roomd_ip;
    location /.well-known/acme-challenge/ { root /var/www/gauche-acme; }
    location / { return 404; }
}
EOF
rm -f /etc/nginx/sites-enabled/default
ln -sf /etc/nginx/sites-available/gauche-roomd /etc/nginx/sites-enabled/gauche-roomd
nginx -t
systemctl enable --now nginx
systemctl reload nginx

# CERTIFICATES: The IP certificate support requires Certbot 5.4 or later.
python3 -m venv /opt/gauche-certbot
/opt/gauche-certbot/bin/pip install --disable-pip-version-check 'certbot>=5.4,<6'
