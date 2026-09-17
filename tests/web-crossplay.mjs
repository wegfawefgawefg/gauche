import {chromium} from 'playwright-core';
import {spawn} from 'node:child_process';
import fs from 'node:fs/promises';
const root=new URL('..',import.meta.url).pathname.replace(/\/$/,'');
const base=process.env.TEEMING_TEST_URL || 'http://127.0.0.1:8787';
const browser=await chromium.launch({executablePath:'/usr/bin/google-chrome',headless:true,args:['--no-sandbox','--use-angle=swiftshader','--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-renderer-backgrounding','--disable-backgrounding-occluded-windows']});
const processes=[];
function native(args,label){const child=spawn(root+'/build-release/gauche',args,{cwd:root,env:{...process.env,SDL_VIDEODRIVER:'dummy',SDL_AUDIODRIVER:'dummy',XDG_DATA_HOME:'/tmp/teeming-crossplay-'+label,XDG_CONFIG_HOME:'/tmp/teeming-config-'+label}});processes.push(child);child.stdout.on('data',d=>console.log(label,String(d)));child.stderr.on('data',d=>console.log(label,'ERR',String(d)));return child;}
async function page(query){const p=await browser.newPage({viewport:{width:960,height:720}});p.on('pageerror',e=>console.log('BROWSER ERROR',e.message));p.on('console',m=>{if(m.text().includes('[Teeming]'))console.log(m.text().slice(0,300));});await p.goto(base+'/?'+query);await p.waitForFunction(()=>window.teeming?.gameState,{timeout:45000});return p;}
async function monitor(p,label){for(let i=0;i<10;i++){await new Promise(r=>setTimeout(r,2000));console.log(label,await p.evaluate(()=>window.teeming.gameState));}const state=await p.evaluate(()=>window.teeming.gameState);if(!state.playing||state.tick<300||state.players!==2||state.recoveries!==0)throw Error('Crossplay failed '+JSON.stringify(state));console.log('NETLOG',await p.evaluate(()=>{const fs=window.teeming.FS;return fs.readdir('/persistent/netlogs').filter(x=>x.endsWith('.log')).map(x=>fs.readFile('/persistent/netlogs/'+x,{encoding:'utf8'})).join('\n').slice(-6000);}));}
try{
 const p=await page('host=Web%20crossplay&autostart=1');await p.waitForFunction(()=>window.teeming.gameState.room.length===6,{timeout:15000});const code=await p.evaluate(()=>window.teeming.gameState.room);
 const guest=native(['--headless','--join-room',code,'--force-relay','--seconds','28'],'guest');
 await monitor(p,'WEBHOST');guest.kill('SIGTERM');await p.close();
 const roomfile='/tmp/teeming-native-host-code';await fs.rm(roomfile,{force:true});
 const host=native(['--host-room','Native crossplay','--room-file',roomfile,'--auto-start','2','--seconds','50','--force-relay'],'host');
 let room='';for(let i=0;i<100;i++){try{room=(await fs.readFile(roomfile,'utf8')).trim();if(room)break;}catch{}await new Promise(r=>setTimeout(r,100));}
 if(!room)throw Error('No native room');const q=await page('room='+room+'&bot=1');await monitor(q,'WEBGUEST');host.kill('SIGTERM');
}finally{for(const p of processes)p.kill('SIGTERM');await browser.close();}
