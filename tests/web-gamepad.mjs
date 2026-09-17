import assert from 'node:assert/strict';
import {chromium} from 'playwright-core';
const browser=await chromium.launch({executablePath:'/usr/bin/google-chrome',headless:true,args:['--no-sandbox','--use-angle=swiftshader','--enable-unsafe-swiftshader']});
try{
 const page=await browser.newPage();
 page.on('console',m=>{if(m.text().includes('[Teeming]'))console.log(m.text().slice(0,600));});
 await page.addInitScript(()=>{
   window.pad={id:'Xbox 360 Controller (STANDARD GAMEPAD Vendor: 045e Product: 028e)',index:0,connected:true,mapping:'standard',axes:[0,0,0,0],buttons:Array.from({length:17},()=>({pressed:false,touched:false,value:0})),timestamp:performance.now()};
   Object.defineProperty(navigator,'getGamepads',{value:()=>[window.pad]});
   window.pressPad=(i,pressed)=>{pad.buttons[i]={pressed,touched:pressed,value:pressed?1:0};pad.timestamp=performance.now();};
 });
 await page.goto(process.env.TEEMING_TEST_URL || 'http://127.0.0.1:8787');
 await page.waitForFunction(()=>window.teeming?.gameState,{timeout:45000});
 await page.evaluate(()=>pressPad(0,true));await page.waitForTimeout(250);await page.evaluate(()=>pressPad(0,false));
 await page.waitForFunction(()=>window.teeming.gameState.playing,{timeout:10000});
 const before=await page.evaluate(()=>window.teeming.gameState.position);
 await page.evaluate(()=>{pad.axes[1]=1;pad.timestamp=performance.now();});await page.waitForTimeout(700);
 await page.evaluate(()=>{pad.axes[1]=0;pad.axes[0]=1;pad.timestamp=performance.now();});await page.waitForTimeout(700);
 const after=await page.evaluate(()=>window.teeming.gameState);
 assert.notDeepEqual(before,after.position);
 console.log('PASS standard gamepad menu confirmation and analog movement',before,after.position);
}finally{await browser.close();}
