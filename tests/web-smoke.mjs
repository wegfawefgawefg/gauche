import assert from 'node:assert/strict';
import {mkdir,writeFile} from 'node:fs/promises';
import {chromium} from 'playwright-core';

const base=process.env.TEEMING_TEST_URL || 'http://127.0.0.1:8787';
const artifacts=process.env.TEEMING_TEST_ARTIFACTS || '/tmp/teeming-browser-smoke';
await mkdir(artifacts,{recursive:true});
const browser=await chromium.launch({executablePath:process.env.CHROME || '/usr/bin/google-chrome',headless:true,
  args:['--no-sandbox','--use-angle=swiftshader','--enable-unsafe-swiftshader',
    '--disable-background-timer-throttling','--disable-renderer-backgrounding','--disable-backgrounding-occluded-windows']});
const pages=[],errors=[];
const state=page=>page.evaluate(()=>window.teeming.gameState);
const command=(page,value)=>page.evaluate(value=>{window.teeming.command=value;},value);
async function open(query='') {
  const page=await browser.newPage({viewport:{width:1280,height:900}});
  pages.push(page);
  page.on('pageerror',error=>errors.push(error.message));
  await page.goto(base+'/?'+query);
  await page.click('#play');
  await page.waitForFunction(()=>window.teeming?.gameState,{timeout:60000});
  return page;
}
try {
  const solo=await open();
  await solo.screenshot({path:artifacts+'/title.png'});
  await solo.keyboard.press('Enter');
  await solo.waitForFunction(()=>window.teeming.gameState.tick>30);
  const before=await state(solo);
  await solo.keyboard.down('s');await solo.waitForTimeout(200);await solo.keyboard.up('s');
  await solo.keyboard.down('d');await solo.waitForTimeout(200);await solo.keyboard.up('d');
  await solo.waitForTimeout(200);
  assert.notDeepEqual((await state(solo)).position,before.position,'Keyboard should move player');
  assert.equal(await solo.evaluate(()=>window.teeming.SDL3.audioContext.state),'running');
  await command(solo,'audio:master');
  await solo.waitForTimeout(300);
  const settings=await solo.evaluate(async()=>{
    const fs=window.teeming.FS;
    await new Promise((resolve,reject)=>fs.syncfs(false,error=>error?reject(error):resolve()));
    return fs.readFile('/persistent/gubsy/settings_profiles/audio.lisp',{encoding:'utf8'});
  });
  await solo.screenshot({path:artifacts+'/forest.png'});
  await solo.reload();await solo.click('#play');
  await solo.waitForFunction(()=>window.teeming?.gameState);
  assert.equal(await solo.evaluate(()=>window.teeming.FS.readFile('/persistent/gubsy/settings_profiles/audio.lisp',{encoding:'utf8'})),settings);
  await solo.close();
  console.log('PASS keyboard gameplay, audio activation, settings persistence');

  const host=await open('host=Browser%20smoke&autostart=1');
  await host.waitForFunction(()=>window.teeming.gameState.room.length===6,{timeout:20000});
  const code=(await state(host)).room;
  const guest=await open('room='+code+'&bot=1');
  await guest.waitForFunction(()=>window.teeming.gameState.playing && window.teeming.gameState.tick>180,{timeout:30000});
  const initial=await state(guest);
  assert.equal(initial.players,2);
  assert.equal(initial.recoveries,0);

  const late=await open();
  await command(late,'room:browse');
  await late.waitForFunction(code=>window.teeming.gameState.listedRooms.includes(code),code,{timeout:15000});
  await command(late,'room:code:'+code);
  await late.waitForFunction(()=>window.teeming.gameState.playing && window.teeming.gameState.players===3,{timeout:30000});
  await guest.waitForTimeout(10000);
  const joined=await state(guest), lateState=await state(late);
  assert(joined.tick>initial.tick+300,'Simulation continues with three browsers');
  assert.equal(joined.recoveries,0);
  assert.equal(lateState.recoveries,0);
  await late.screenshot({path:artifacts+'/multiplayer.png'});
  // A short suspended tab must catch up when resumed.
  const cdp=await late.context().newCDPSession(late);
  await cdp.send('Page.setWebLifecycleState',{state:'frozen'});
  await guest.waitForTimeout(2500);
  await cdp.send('Page.setWebLifecycleState',{state:'active'});
  await late.waitForFunction(tick=>window.teeming.gameState.tick>tick+120,lateState.tick,{timeout:15000});
  await command(late,'room:leave');
  await late.waitForFunction(()=>window.teeming.gameState.room==='',{timeout:10000});
  console.log('PASS public discovery, room join/start, late join, stable simulation, short suspension and leave', {room:code,guest:joined,late:lateState});
  await command(guest,'room:leave');await guest.waitForTimeout(1200);
  await command(host,'room:leave');await host.waitForTimeout(1200);
  assert.deepEqual(errors,[]);
} catch(error) {
  for(const [index,page] of pages.entries())if(!page.isClosed()){
    console.error('PAGE',index,await page.evaluate(()=>({status:document.querySelector('#status')?.textContent,state:window.teeming?.gameState})).catch(()=>null));
    const net=await page.evaluate(()=>{const g=window.teeming;try{return g.FS.readFile(g.gameState.netLog,{encoding:'utf8'});}catch{return '';}}).catch(()=> '');
    await writeFile(artifacts+'/network-'+index+'.log',net);
    await page.screenshot({path:artifacts+'/failure-'+index+'.png'}).catch(()=>{});
  }
  throw error;
} finally {await browser.close();}
