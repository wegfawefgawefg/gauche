import assert from 'node:assert/strict';
import {mkdir} from 'node:fs/promises';
import {chromium} from 'playwright-core';

const base = process.env.TEEMING_TEST_URL || 'http://127.0.0.1:8787';
const artifacts = '/tmp/teeming-renderer-browser';
await mkdir(artifacts, {recursive: true});
const browser = await chromium.launch({
  executablePath: process.env.CHROME || '/usr/bin/google-chrome', headless: true,
  args: ['--no-sandbox', '--enable-unsafe-webgpu', '--use-angle=swiftshader', '--enable-unsafe-swiftshader',
    '--enable-gpu', '--enable-features=Vulkan', '--use-vulkan=swiftshader', '--ignore-gpu-blocklist'],
});
try {
  for (const mode of ['webgpu', 'webgl2', 'adapter-failure', 'device-failure']) {
    const page = await browser.newPage({viewport: {width: 960, height: 640}});
    const errors = [];
    page.on('pageerror', error => errors.push(error.message));
    page.on('console', message => {
      if (/WebGPU:|WebGPU device lost|GL_INVALID|Aborted\(/.test(message.text())) errors.push(message.text());
    });
    await page.addInitScript(mode => {
      localStorage.setItem('teeming.errorReports', 'off');
      if (mode === 'adapter-failure' && navigator.gpu)
        navigator.gpu.requestAdapter = async () => { throw new Error('simulated adapter failure'); };
      if (mode === 'device-failure' && navigator.gpu)
        navigator.gpu.requestAdapter = async () => ({requestDevice: async () => { throw new Error('simulated device failure'); }});
    }, mode);
    await page.goto(base + '/?renderer=' + mode);
    await page.waitForFunction(() => window.teeming?.gameState, {timeout: 90000});
    const expected = mode === 'webgpu' ? 'teeming-webgpu' : 'teeming-webgl2';
    assert.equal(await page.evaluate(() => teeming.gameState.renderer), expected);
    await page.screenshot({path: `${artifacts}/${mode}-title.png`});
    await page.keyboard.press('Enter');
    await page.waitForFunction(() => teeming.gameState.tick > 60, {timeout: 30000});
    for (const size of [{width: 720, height: 480}, {width: 1280, height: 720}]) {
      await page.setViewportSize(size);
      await page.waitForFunction(({width, height}) => teeming.gameState.renderSize[0] === width &&
        teeming.gameState.renderSize[1] === height, size);
    }
    // Changing internal resolution must retain the full-size canvas and survive resize.
    for (const percent of [75, 50, 100]) {
      await page.evaluate(percent => { teeming.command = 'setting:render-scale:' + JSON.stringify(percent + '%'); }, percent);
      await page.waitForFunction(percent => teeming.gameState.renderPercent === percent &&
        teeming.gameState.renderSize[0] === Math.round(1280 * percent / 100) &&
        teeming.gameState.renderSize[1] === Math.round(720 * percent / 100), percent);
      assert.deepEqual(await page.evaluate(() => {
        const c = document.querySelector('canvas'); return [c.width, c.height];
      }), [1280, 720]);
      if (percent === 50) {
        await page.setViewportSize({width: 960, height: 640});
        await page.waitForFunction(() => teeming.gameState.renderSize[0] === 480 &&
          teeming.gameState.renderSize[1] === 320);
        await page.setViewportSize({width: 1280, height: 720});
        await page.waitForFunction(() => teeming.gameState.renderSize[0] === 640);
      }
    }
    await page.keyboard.press('F1');
    await page.waitForTimeout(300);
    await page.screenshot({path: `${artifacts}/${mode}-debug.png`});
    await page.keyboard.press('F1');
    const state = await page.evaluate(() => teeming.gameState);
    assert(state.triangles > 100 && state.drawBatches > 0);
    assert.equal(state.lightmap, true);
    if (mode === 'webgpu') {
      const pixels = await page.evaluate(async () => {
        const gpu = teeming.ownedGPU;
        const bytes = await gpu.read(0, 0, 0, gpu.width, gpu.height);
        let bright = 0;
        for (let i = 0; i < bytes.length; i += 4) if (bytes[i] + bytes[i+1] + bytes[i+2] > 90) bright++;
        return {bright, errors: gpu.errors, lost: gpu.lost};
      });
      assert(pixels.bright > 500, 'Rendered scene readback has visible content');
      assert.deepEqual(pixels.errors, []);
      assert.equal(pixels.lost, false);
    }
    await page.screenshot({path: `${artifacts}/${mode}-forest.png`});
    assert.deepEqual(errors, []);
    console.log(`PASS ${mode}: ${state.renderer}, resize, render scales, game, ImGui, ${state.drawBatches} batches / ${state.triangles} triangles`);
    await page.close();
  }
} finally {
  await browser.close();
}
