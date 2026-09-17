#!/usr/bin/env python3
"""Run as root beside server.py and unit; does not restart roomd or the bridge."""
from pathlib import Path
import shutil
import subprocess
base=Path(__file__).resolve().parent
shutil.copy2(base/'server.py','/opt/gauche-roomd/browser-reports.py')
shutil.copy2(base/'teeming-browser-reports.service','/etc/systemd/system/teeming-browser-reports.service')
path=Path('/etc/nginx/sites-available/gauche-roomd')
original=path.read_text()
backup=path.with_suffix('.pre-reports')
if not backup.exists():backup.write_text(original)
if 'location = /browser-reports' not in original:
    marker='    location = /browser-relay {'
    if marker not in original:raise SystemExit('Expected browser-enabled Nginx site')
    path.write_text(original.replace(marker,'''    location = /browser-reports {
        proxy_pass http://127.0.0.1:8792;
        proxy_set_header X-Real-IP $remote_addr;
        client_max_body_size 32k;
        access_log off;
    }
'''+marker))
if subprocess.run(['nginx','-t']).returncode:
    path.write_text(original)
    raise SystemExit('Nginx validation failed; restored original')
subprocess.run(['systemctl','daemon-reload'],check=True)
subprocess.run(['systemctl','enable','--now','teeming-browser-reports'],check=True)
subprocess.run(['systemctl','reload','nginx'],check=True)
