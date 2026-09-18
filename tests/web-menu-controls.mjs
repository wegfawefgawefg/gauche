import assert from 'node:assert/strict';
import {chromium} from 'playwright-core';
const browser=await chromium.launch({executablePath:'/usr/bin/google-chrome',headless:true,
 args:['--no-sandbox','--use-angle=swiftshader','--enable-unsafe-swiftshader']});
try {
 const page=await browser.newPage({viewport:{width:1280,height:720}});
 await page.addInitScript(()=>{
  localStorage.setItem('teeming.errorReports','off');
  window.pad={id:'Xbox 360 Controller',index:0,connected:true,mapping:'standard',axes:[0,0,0,0],
   buttons:Array.from({length:17},()=>({pressed:false,touched:false,value:0})),timestamp:performance.now()};
  Object.defineProperty(navigator,'getGamepads',{value:()=>[pad]});
 });
 const command=async text=>{await page.evaluate(text=>teeming.command=text,text);await page.waitForTimeout(300);};
 const key=async text=>{await page.keyboard.press(text);await page.waitForTimeout(200);};
 const state=()=>page.evaluate(()=>teeming.gameState);
 const button=async i=>{
  await page.evaluate(i=>{pad.buttons[i]={pressed:true,touched:true,value:1};pad.timestamp=performance.now();},i);
  await page.waitForTimeout(100);
  await page.evaluate(i=>{pad.buttons[i]={pressed:false,touched:false,value:0};pad.timestamp=performance.now();},i);
  await page.waitForTimeout(200);
 };
 await page.goto((process.env.TEEMING_TEST_URL || 'http://127.0.0.1:8787')+'/?renderer=webgl2');
 await page.waitForFunction(()=>window.teeming?.gameState,{timeout:60000});
 await command('audio');
 assert.equal((await state()).focusedControl,'master');
 await command('setting:master:100');await key('ArrowRight');
 assert.equal((await state()).volumes[0],1);
 await key('ArrowLeft');assert(Math.abs((await state()).volumes[0]-.99)<.001);
 await command('setting:master:0');await key('ArrowLeft');
 assert.equal((await state()).volumes[0],0);
 await key('ArrowRight');assert(Math.abs((await state()).volumes[0]-.01)<.001);
 // Drag across the actual track, including multiple frames while holding the mouse.
 await page.mouse.move(410,311);await page.mouse.down();
 await page.mouse.move(640,311,{steps:12});await page.waitForTimeout(200);
 await page.mouse.up();await page.waitForTimeout(250);
 assert(Math.abs((await state()).volumes[0]-.5)<.04,JSON.stringify(await state()));
 console.log('PASS slider pointer drag, 1% keyboard steps and non-wrapping endpoints');
 // Move the pointer away so controller focus remains observable.
 await page.mouse.move(50,50);
 await button(0);await button(13);
 assert.equal((await state()).focusedControl,'music');
 const music=(await state()).volumes[1];await button(14);
 assert(Math.abs((await state()).volumes[1]-(music-.01))<.001);
 await button(1);assert.equal((await state()).menuScreen,13);
 await command('display');await key('ArrowDown');
 assert.equal((await state()).focusedControl,'render-scale');
 await key('Enter');assert.deepEqual((await state()).openControls,['render-scale']);
 await key('ArrowDown');await key('Escape');
 assert.deepEqual((await state()).openControls,[]);
 assert.equal((await state()).menuScreen,14);assert.equal((await state()).renderPercent,100);
 await button(0);await button(13);await button(0);
 assert.equal((await state()).renderPercent,75);
 await button(0);await button(13);await button(1);
 assert.equal((await state()).renderPercent,75);assert.equal((await state()).menuScreen,14);
 await key('ArrowDown');await key('ArrowDown');await key('ArrowDown');
 assert.equal((await state()).focusedControl,'auto-reports');
 const reporting=(await state()).autoReports;await key('Enter');
 assert.equal((await state()).autoReports,!reporting);await key('Enter');
 await button(1);assert.equal((await state()).menuScreen,13);
 console.log('PASS controller slider adjustment, dropdown selection and cancel before leaving page');
 const volumes=(await state()).volumes;
 await page.evaluate(()=>new Promise((resolve,reject)=>teeming.FS.syncfs(false,e=>e?reject(e):resolve())));
 await page.reload();await page.waitForFunction(()=>window.teeming?.gameState);
 assert.deepEqual((await state()).volumes,volumes);
 assert.equal((await state()).renderPercent,75);
 console.log('PASS adjusted audio and display values persist after reload');
} finally {await browser.close();}
