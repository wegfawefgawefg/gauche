#!/usr/bin/env bash
set -euo pipefail
roomd_ip=${1:?Usage: enable-https.sh PUBLIC_IPV4 [DOMAIN]}
roomd_name=${2:-$roomd_ip}
python3 -c 'import ipaddress,sys; ipaddress.IPv4Address(sys.argv[1])' "$roomd_ip"
[[ "$roomd_name" =~ ^[a-zA-Z0-9.-]+$ ]]
roomd_cert_args=(--ip-address "$roomd_ip" --preferred-profile shortlived)
if [[ "$roomd_name" != "$roomd_ip" ]]; then roomd_cert_args=(-d "$roomd_name"); fi

# TLS: Automated validation/renewal never interrupts the room service.
/opt/gauche-certbot/bin/certbot certonly --non-interactive --agree-tos \
    --register-unsafely-without-email --webroot -w /var/www/gauche-acme \
    --cert-name gauche-roomd "${roomd_cert_args[@]}" --deploy-hook 'systemctl reload nginx'
cat > /etc/nginx/sites-available/gauche-roomd <<EOF
limit_req_zone \$binary_remote_addr zone=room_api:10m rate=10r/s;
server {
    listen 80 default_server;
    server_name $roomd_name;
    location /.well-known/acme-challenge/ { root /var/www/gauche-acme; }
    location / { return 404; }
}
server {
    listen 443 ssl default_server;
    server_name $roomd_name;
    ssl_certificate /etc/letsencrypt/live/gauche-roomd/fullchain.pem;
    ssl_certificate_key /etc/letsencrypt/live/gauche-roomd/privkey.pem;
    ssl_protocols TLSv1.2 TLSv1.3;
    client_max_body_size 32k;
    location = /health { proxy_pass http://$roomd_ip:8788; }
    location ~ ^/rooms(?:/[A-Z0-9]+(?:/(?:join_attempt|join|heartbeat|leave|remove_member))?|/create)?\$ {
        limit_req zone=room_api burst=40 nodelay;
        limit_req_status 429;
        proxy_connect_timeout 2s;
        proxy_read_timeout 5s;
        proxy_pass http://$roomd_ip:8788;
    }
    location / { return 404; }
}
EOF
nginx -t
systemctl reload nginx
cat > /etc/systemd/system/gauche-certbot.service <<'EOF'
[Unit]
Description=Renew Gauche room API certificate
[Service]
Type=oneshot
ExecStart=/opt/gauche-certbot/bin/certbot renew --quiet --deploy-hook "systemctl reload nginx"
EOF
cat > /etc/systemd/system/gauche-certbot.timer <<'EOF'
[Unit]
Description=Check Gauche certificate renewal twice daily
[Timer]
OnCalendar=*-*-* 00,12:00:00
RandomizedDelaySec=1800
Persistent=true
[Install]
WantedBy=timers.target
EOF
systemctl daemon-reload
systemctl enable --now gauche-certbot.timer
