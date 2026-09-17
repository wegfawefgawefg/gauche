#!/usr/bin/env python3
"""Run as root on the existing dedicated room VPS, beside relay.py and the unit."""
from pathlib import Path
import shutil
import subprocess

subprocess.run(['apt-get','update','-qq'],check=True)
subprocess.run(['apt-get','install','-y','python3-aiohttp'],check=True)
base=Path(__file__).resolve().parent
shutil.copy2(base/'relay.py','/opt/gauche-roomd/browser-relay.py')
shutil.copy2(base/'teeming-browser-relay.service','/etc/systemd/system/teeming-browser-relay.service')
path=Path('/etc/nginx/sites-available/gauche-roomd')
original=path.read_text()
backup=path.with_suffix('.pre-browser')
if not backup.exists():backup.write_text(original)
text=original
if 'teeming_origin' not in text:
    text='''map $http_origin $teeming_origin {
    default "";
    "https://teeming.pages.dev" $http_origin;
    "http://localhost:8787" $http_origin;
    "http://127.0.0.1:8787" $http_origin;
}
'''+text
    text=text.replace('    client_max_body_size 32k;', '''    client_max_body_size 32k;
    add_header Access-Control-Allow-Origin $teeming_origin always;
    add_header Access-Control-Allow-Methods "GET, POST, OPTIONS" always;
    add_header Access-Control-Allow-Headers "Content-Type" always;
    add_header Access-Control-Expose-Headers "Date" always;
    add_header Vary "Origin" always;
    if ($request_method = OPTIONS) { return 204; }
    location = /browser-relay {
        proxy_pass http://127.0.0.1:8791;
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection "upgrade";
        proxy_set_header X-Real-IP $remote_addr;
        proxy_read_timeout 90s;
        proxy_send_timeout 90s;
    }''')
path.write_text(text)
result=subprocess.run(['nginx','-t'])
if result.returncode:
    path.write_text(original)
    raise SystemExit('Nginx validation failed; original configuration restored')
subprocess.run(['systemctl','daemon-reload'],check=True)
subprocess.run(['systemctl','enable','--now','teeming-browser-relay'],check=True)
subprocess.run(['systemctl','reload','nginx'],check=True)
