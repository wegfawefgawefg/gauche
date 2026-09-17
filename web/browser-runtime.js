// Browser presentation owns fullscreen and frame scheduling; SDL only draws.
export function installBrowserRuntime(game, canvas, log) {
  const prompt = document.querySelector('#fullscreen-prompt');
  let pending = false;
  function sync() {
    game.browserFullscreen = Boolean(document.fullscreenElement);
    window.dispatchEvent(new Event('resize'));
    canvas.focus();
  }
  async function toggleFullscreen() {
    if (pending) return;
    if (!document.fullscreenEnabled) {
      game.browserNotice = 'Fullscreen is unavailable in this browser';
      return;
    }
    if (!document.fullscreenElement && !navigator.userActivation?.isActive) {
      prompt.hidden = false;
      prompt.querySelector('button').focus();
      return;
    }
    pending = true;
    try {
      if (document.fullscreenElement) await document.exitFullscreen();
      else await document.querySelector('#stage').requestFullscreen();
      prompt.hidden = true;
      game.browserNotice = '';
    } catch (error) {
      log('Fullscreen:', error.name);
      game.browserNotice = 'Could not enter fullscreen; try again';
      prompt.hidden = false;
    } finally { pending = false; sync(); }
  }
  game.toggleFullscreen = toggleFullscreen;
  prompt.querySelector('[data-enter]').onclick = toggleFullscreen;
  prompt.querySelector('[data-cancel]').onclick = () => { prompt.hidden = true; canvas.focus(); };
  document.addEventListener('fullscreenchange', sync);
  game.browserFullscreen = Boolean(document.fullscreenElement);

  let due = 0, previousCap = -1, previousStamp = 0, previousRender = 0;
  let refreshSamples = [], frameSamples = [], previousReturn = 0, workMs = 0;
  function averageRate(samples) {
    return samples.length ? Math.round(1000 * samples.length / samples.reduce((a,b)=>a+b,0)) : 0;
  }
  // A visibility change resolves an already pending RAF, avoiding a hidden-tab hang.
  function nextCallback() {
    return new Promise(resolve => {
      let raf = 0, timer = 0;
      function done(stamp) {
        cancelAnimationFrame(raf); clearTimeout(timer);
        document.removeEventListener('visibilitychange', visibility);
        resolve(stamp);
      }
      function visibility() { done(performance.now()); }
      document.addEventListener('visibilitychange', visibility, {once:true});
      if (document.hidden) timer = setTimeout(()=>done(performance.now()), 50);
      else raf = requestAnimationFrame(done);
    });
  }
  game.nextFrame = async cap => {
    if(previousReturn)workMs=workMs*.9+(performance.now()-previousReturn)*.1;
    if (cap !== previousCap) { due = 0; previousCap = cap; }
    for (;;) {
      const stamp = await nextCallback();
      if (document.hidden) { due=0; previousStamp=0; previousRender=0; previousReturn=0; return; }
      if (previousStamp && stamp > previousStamp && stamp-previousStamp < 100) {
        refreshSamples.push(stamp-previousStamp);
        if (refreshSamples.length>120) refreshSamples.shift();
      }
      previousStamp=stamp;
      if (cap>0 && stamp < due-0.25) continue;
      if (cap>0) {
        const interval=1000/cap;
        due = due && stamp-due<interval*2 ? due+interval : stamp+interval;
      }
      if (previousRender && stamp>previousRender && stamp-previousRender<500) {
        frameSamples.push(stamp-previousRender);
        if(frameSamples.length>120)frameSamples.shift();
      }
      previousRender=stamp;
      game.frameTiming={callbackFps:averageRate(refreshSamples),renderFps:averageRate(frameSamples),cap,workMs:Math.round(workMs*100)/100};
      previousReturn=performance.now();
      return;
    }
  };
}
