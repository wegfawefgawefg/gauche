// Automatic reports deliberately exclude names, room codes, URLs and save files.
export function createReporting(getGame, getBuild) {
  let enabled = true, sent = 0, last = -Infinity;
  const seen = new Set(), events = [];
  try { enabled = localStorage.getItem('teeming.errorReports') !== 'off'; } catch {}
  function setEnabled(value) {
    enabled = Boolean(value);
    try { localStorage.setItem('teeming.errorReports', enabled ? 'on' : 'off'); } catch {}
    const game=getGame(); if(game)game.autoReports=enabled;
  }
  function clean(text) {
    return String(text).replace(/https?:\/\/[^\s)]+/g,'[url]')
      .replace(/[a-f0-9]{32,}/gi,'[id]').slice(0,1200);
  }
  function report(kind, message='') {
    if(!enabled || sent>=8 || seen.has(kind) || performance.now()-last<10000)return;
    seen.add(kind); ++sent; last=performance.now();
    const game=getGame(), source=game?.gameState || {}, state={};
    for(const key of ['seed','floor','tick','recoveries','host','rtt','confirmed','players','snapshot','phase','ready','started','over','playing','renderSize','windowSize'])
      if(source[key]!==undefined)state[key]=source[key];
    let network=[];
    if(source.netLog)try {
      network=game.FS.readFile(source.netLog,{encoding:'utf8'}).split('\n').filter(Boolean).slice(-20)
        .filter(line=>line.length<=1024 && /^(?:[a-z_]+=(?:-?[0-9]+|[a-z_]+) ?)+$/.test(line));
    } catch {}
    const report={version:1,build:getBuild(),kind,message:clean(message),
      browser:navigator.userAgent.slice(0,300),state,network,timing:game?.frameTiming || {},events:events.slice(-20)};
    fetch('https://45.77.123.14/browser-reports', {method:'POST',credentials:'omit',
      headers:{'Content-Type':'application/json'},body:JSON.stringify(report),
      keepalive:true,signal:AbortSignal.timeout(5000)}).catch(()=>{});
  }
  let lastSession='', recoveries=0, wasReady=false;
  function observe() {
    const game=getGame(), s=game?.gameState;
    if(!s)return;
    if(s.netLog!==lastSession) {lastSession=s.netLog;recoveries=0;wasReady=false;}
    const event={tick:s.tick,floor:s.floor,ready:s.ready,players:s.players,recoveries:s.recoveries,rtt:s.rtt};
    events.push(event);if(events.length>20)events.shift();
    if(s.recoveries>=recoveries+3) {report('desync-storm');recoveries=s.recoveries;}
    if(!wasReady && /fail|timed? out|timeout|denied|mismatch|unreachable|did not answer/i.test((s.roomStatus || '')+' '+(s.status || '')))report('join-failed');
    if(wasReady && !s.ready && /disconnect|lost|timeout|failed/i.test(s.status || ''))report('disconnect');
    wasReady=s.ready;
  }
  return {get enabled(){return enabled;},setEnabled,report,observe};
}
