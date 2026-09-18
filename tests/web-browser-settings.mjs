import assert from 'node:assert/strict';
import {chromium} from 'playwright-core';
const browser=await chromium.launch({executablePath:'/usr/bin/google-chrome',headless:true,
 args:['--no-sandbox','--use-angle=swiftshader','--enable-unsafe-swiftshader']});
const base=process.env.TEEMING_TEST_URL || 'http://127.0.0.1:8787';
try {
 const page=await browser.newPage({viewport:{width:960,height:720}});
 const reports=[];
 await page.route('**/browser-reports',async route=>{reports.push(route.request().postDataJSON());await route.fulfill({status:204,headers:{'Access-Control-Allow-Origin':'*'}});});
 await page.addInitScript(()=>{
  // Simulate a high-refresh scheduler independently of the CI host's display.
  let stamp=0;
  window.requestAnimationFrame=fn=>setTimeout(()=>fn(stamp+=1000/144),1);
  window.cancelAnimationFrame=clearTimeout;
 });
 await page.goto(base);
 await page.waitForFunction(()=>window.teeming?.gameState,{timeout:60000});
 await page.evaluate(()=>teeming.command='display');await page.waitForTimeout(350);
 for(let i=0;i<3;i++) {
  await page.keyboard.press('Enter');
  await page.waitForFunction(()=>!!document.fullscreenElement,{timeout:5000});
  await page.waitForFunction(()=>teeming.gameState.fullscreen);
  await page.keyboard.press('Enter');
  await page.waitForFunction(()=>!document.fullscreenElement,{timeout:5000});
  await page.waitForFunction(()=>!teeming.gameState.fullscreen);
 }
 // Escape/browser exits must synchronize, rather than leaving the menu stuck on Exit.
 await page.keyboard.press('Enter');await page.waitForFunction(()=>!!document.fullscreenElement);
 await page.evaluate(()=>document.exitFullscreen());await page.waitForFunction(()=>!teeming.gameState.fullscreen);
 console.log('PASS repeated settings fullscreen entry/exit and external exit');
 await page.waitForTimeout(2000);
 let timing=await page.evaluate(()=>teeming.frameTiming);
 assert.equal(timing.renderFps,144,JSON.stringify(timing));
 await page.evaluate(()=>teeming.command='display:frame-cap');
 await page.waitForTimeout(2600);
 timing=await page.evaluate(()=>teeming.frameTiming);
 assert(timing.renderFps>=50 && timing.renderFps<=65,JSON.stringify(timing));
 console.log('PASS high-refresh scheduler and working 60 FPS cap',timing);
 // Save a lower render scale, verify resize/fullscreen, then reload the preference.
 await page.evaluate(()=>teeming.command='display:render-scale');
 await page.waitForFunction(()=>teeming.gameState.renderPercent===75);
 await page.evaluate(()=>teeming.command='display:render-scale');
 await page.waitForFunction(()=>teeming.gameState.renderPercent===50 &&
  teeming.gameState.renderSize[0]===480 && teeming.gameState.renderSize[1]===360);
 await page.keyboard.press('Enter');await page.waitForFunction(()=>!!document.fullscreenElement);
 await page.waitForFunction(()=>teeming.gameState.renderSize[0]===Math.round(canvas.width*.5) &&
  teeming.gameState.renderSize[1]===Math.round(canvas.height*.5));
 await page.evaluate(()=>document.exitFullscreen());
 await page.waitForFunction(()=>!teeming.gameState.fullscreen);
 await page.evaluate(()=>new Promise((resolve,reject)=>teeming.FS.syncfs(false,e=>e?reject(e):resolve())));
 await page.reload();await page.waitForFunction(()=>window.teeming?.gameState);
 await page.waitForFunction(()=>teeming.gameState.renderPercent===50 && teeming.gameState.renderSize[0]===480);
 await page.evaluate(()=>teeming.command='display:render-scale');
 await page.waitForFunction(()=>teeming.gameState.renderPercent===100 && teeming.gameState.renderSize[0]===960);
 console.log('PASS render scale, fullscreen sizing, persistence and restoring 100%');
 // Browser shortcut is not cancelled by SDL.
 assert.equal(await page.evaluate(()=>{const e=new KeyboardEvent('keydown',{code:'F11',key:'F11',bubbles:true,cancelable:true});canvas.dispatchEvent(e);return e.defaultPrevented;}),false);
 // Exercise the same automatic path used by unhandled application failures.
 await page.evaluate(()=>window.dispatchEvent(new ErrorEvent('error',{message:'settings test https://example.com/?token=secret'})));
 await page.waitForTimeout(400);
 assert.equal(reports.length,1);
 assert.equal(reports[0].kind,'javascript-error');
 assert(!JSON.stringify(reports[0]).includes('secret'));
 assert(!('room' in reports[0].state));
 await page.evaluate(()=>teeming.reportFailure('javascript-error','duplicate'));
 await page.waitForTimeout(200);assert.equal(reports.length,1);
 await page.evaluate(()=>teeming.command='display:auto-reports');
 await page.waitForFunction(()=>!teeming.autoReports);
 await page.reload();await page.waitForFunction(()=>window.teeming?.gameState);
 assert.equal(await page.evaluate(()=>teeming.autoReports),false);
 await page.evaluate(()=>teeming.reportFailure('startup-failed','should not upload'));
 await page.waitForTimeout(250);assert.equal(reports.length,1);
 console.log('PASS automatic report, sanitization, duplicate suppression and persisted opt-out');
}finally{await browser.close();}
