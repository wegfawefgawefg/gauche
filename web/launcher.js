const canvas = document.querySelector('#canvas');
const stage = document.querySelector('#stage');
const welcome = document.querySelector('#welcome');
const play = document.querySelector('#play');
const status = document.querySelector('#status');
const progress = document.querySelector('#progress');
const logs = [];
let game, starting = false, syncing = false;
function resumeAudio() {
  const context = game?.SDL3?.audioContext;
  if (context?.state === 'suspended') context.resume().catch(log);
}
function log(...parts) {
  const line = parts.join(' ');
  logs.push(new Date().toISOString() + ' ' + line);
  if (logs.length > 2000) logs.shift();
  console.log('[Teeming]', line);
}
function failure(error) {
  log(error?.stack || String(error));
  welcome.style.display = 'flex';
  status.textContent = 'Could not start: ' + error + '. Save the debug log below.';
  progress.hidden = true;
}
window.addEventListener('error', event => log(event.message));
window.addEventListener('unhandledrejection', event => log(String(event.reason)));

async function load() {
  if (starting) return;
  starting = true;
  play.disabled = true;
  play.textContent = 'Loading…';
  progress.hidden = false;
  try {
    const response = await fetch('manifest.json', {cache: 'no-cache'});
    if (!response.ok) throw new Error('Download failed: manifest');
    const manifest = await response.json();
    document.querySelector('#build').textContent = manifest.revision;
    status.textContent = 'Loading the world…';
    let loaded = 0;
    progress.max = manifest.assets.reduce((sum, pack) => sum + pack.bytes, 0);
    progress.value = 0;
    const packs = await Promise.all(manifest.assets.map(async pack => {
      const response = await fetch(pack.file);
      if (!response.ok) throw new Error('Asset download failed');
      const data = new Uint8Array(await response.arrayBuffer());
      loaded += data.length;
      progress.value = loaded;
      return {pack, data};
    }));
    const createTeeming = (await import('./' + manifest.module)).default;
    game = await createTeeming({canvas, noInitialRun: true, print: log, printErr: log,
      onAbort: failure,
      preRun: [module => {
        const fs = module.FS;
        for (const {pack, data} of packs) for (const [path, offset, length] of pack.entries) {
          fs.mkdirTree(path.slice(0, path.lastIndexOf('/')));
          fs.writeFile(path, data.subarray(offset, offset + length));
        }
        fs.chdir('/teeming');
      }]
    });
    window.teeming = game;
    const fs = game.FS;
    fs.mkdirTree('/persistent');
    fs.mount(fs.filesystems.IDBFS, {}, '/persistent');
    await new Promise(resolve => fs.syncfs(true, error => {
      if (error) log('Settings storage unavailable:', error);
      resolve();
    }));
    const args = [];
    // Bounded playtest entry points; regular visits go to the title menu.
    const query = new URLSearchParams(location.search);
    if (query.get('host')) args.push('--host-room', query.get('host').slice(0, 64));
    if (/^[A-Z0-9]{6}$/i.test(query.get('room') || '')) args.push('--join-room', query.get('room').toUpperCase());
    if (query.get('name')) args.push('--player-name', query.get('name').slice(0, 40));
    if (query.get('bot') === '1') args.push('--bot');
    if (query.get('autostart') === '1') args.push('--auto-start', '2');
    if (query.get('smoke') === '1') args.push('--smoke-run', '--frames', '120');
    status.textContent = 'Starting…';
    game.callMain(args);
    resumeAudio();
    stage.classList.add('playing');
    welcome.style.display = 'none';
    canvas.focus();
    setInterval(() => {
      if (syncing) return;
      syncing = true;
      fs.syncfs(false, error => { syncing = false; if (error) log('Settings save:', error); });
    }, 5000);
  } catch (error) { failure(error); }
}
play.addEventListener('click', load);
document.querySelector('#fullscreen').addEventListener('click', () => {
  if (document.fullscreenElement) document.exitFullscreen();
  else stage.requestFullscreen().catch(log);
});
canvas.addEventListener('contextmenu', event => event.preventDefault());
window.addEventListener('pointerdown', resumeAudio);
window.addEventListener('keydown', event => {
  resumeAudio();
  if (document.activeElement === canvas && ['ArrowUp','ArrowDown','ArrowLeft','ArrowRight',' '].includes(event.key)) event.preventDefault();
});
// Controllers may only appear after the first button press; keep checking.
setInterval(() => {
  const pressed = [...(navigator.getGamepads?.() || [])].some(pad => pad?.buttons.some(button => button.pressed));
  if (!starting && pressed) load();
  if (pressed) resumeAudio();
}, 200);
document.querySelector('#report').addEventListener('click', () => {
  let networkLog = '';
  if (game?.gameState?.netLog) {
    try { networkLog = game.FS.readFile(game.gameState.netLog, {encoding:'utf8'}).slice(-131072); }
    catch (error) { log('Network log unavailable:', error); }
  }
  const report = {build: document.querySelector('#build').textContent, userAgent:navigator.userAgent,
    state:game?.gameState, audio:game?.SDL3?.audioContext?.state, networkLog, logs};
  const url = URL.createObjectURL(new Blob([JSON.stringify(report, null, 2)], {type:'application/json'}));
  const link = document.createElement('a'); link.href = url; link.download = 'teeming-debug.json'; link.click();
  setTimeout(() => URL.revokeObjectURL(url), 1000);
});
