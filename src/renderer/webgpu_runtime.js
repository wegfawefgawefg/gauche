// Browser device operations. The C++ renderer owns draw order, resources and batching.
Module.initOwnedWebGPU = async function () {
  if (Module.ownedGPU) return true;
  if (new URL(location.href).searchParams.get('renderer') === 'webgl2' || !navigator.gpu) return false;
  let device;
  try {
    const adapter = await navigator.gpu.requestAdapter();
    if (!adapter) return false;
    device = await adapter.requestDevice();
    // Validate configuration on a disposable canvas before claiming SDL's canvas.
    const probe = document.createElement('canvas').getContext('webgpu');
    if (!probe) { device.destroy(); return false; }
    device.pushErrorScope('validation');
    probe.configure({device, format: navigator.gpu.getPreferredCanvasFormat(), alphaMode: 'opaque'});
    // Exercise presentation before claiming the game canvas. Some drivers can
    // create a device but fail on their first swapchain texture.
    const encoder = device.createCommandEncoder();
    encoder.beginRenderPass({colorAttachments: [{view: probe.getCurrentTexture().createView(),
      loadOp: 'clear', storeOp: 'store', clearValue: [0,0,0,1]}]}).end();
    device.queue.submit([encoder.finish()]);
    await device.queue.onSubmittedWorkDone();
    const error = await device.popErrorScope();
    probe.unconfigure();
    if (error) { device.destroy(); return false; }
  } catch (error) {
    device?.destroy();
    Module.print?.('WebGPU unavailable; using WebGL 2: ' + String(error));
    return false;
  }
  const gpu = {device, images: new Map(), groups: new Map(), pipelines: new Map(), nextId: 1,
    screen: null, lost: false, errors: [], width: 0, height: 0, vertices: null, indices: null};
  device.addEventListener('uncapturederror', event => {
    gpu.errors.push(event.error.message);
    Module.printErr?.('WebGPU: ' + event.error.message);
  });
  device.lost.then(info => {
    if (info.reason === 'destroyed') return;
    gpu.lost = true;
    Module.printErr?.('WebGPU device lost: ' + info.message);
    Module.rendererFailure?.('Graphics device lost. Reload to use WebGL 2.');
  });
  gpu.context = Module.canvas.getContext('webgpu');
  if (!gpu.context) { device.destroy(); return false; }
  gpu.format = navigator.gpu.getPreferredCanvasFormat();
  gpu.context.configure({device, format: gpu.format, alphaMode: 'opaque'});
  gpu.nearest = device.createSampler({minFilter: 'nearest', magFilter: 'nearest'});
  gpu.linear = device.createSampler({minFilter: 'linear', magFilter: 'linear'});
  gpu.shader = device.createShaderModule({code: `
struct VertexOut { @builtin(position) position: vec4f, @location(0) color: vec4f, @location(1) uv: vec2f, @location(2) lightUV: vec2f };
@group(0) @binding(0) var spriteSampler: sampler;
@group(0) @binding(1) var sprite: texture_2d<f32>;
@group(0) @binding(2) var lightSampler: sampler;
@group(0) @binding(3) var lightmap: texture_2d<f32>;
@group(0) @binding(4) var<uniform> size: vec4f;
@vertex fn vs(@location(0) p: vec3f, @location(1) color: vec4f, @location(2) uv: vec2f, @location(3) lightUV: vec2f) -> VertexOut {
 var out: VertexOut; out.position = vec4f(p.x / size.x * 2 - 1, 1 - p.y / size.y * 2, p.z, 1);
 out.color=color; out.uv=uv; out.lightUV=lightUV; return out;
}
@fragment fn fs(v: VertexOut) -> @location(0) vec4f { return textureSample(sprite,spriteSampler,v.uv)*v.color*textureSample(lightmap,lightSampler,v.lightUV); }
`});
  gpu.layout = device.createBindGroupLayout({entries: [
    {binding:0,visibility:GPUShaderStage.FRAGMENT,sampler:{}},
    {binding:1,visibility:GPUShaderStage.FRAGMENT,texture:{}},
    {binding:2,visibility:GPUShaderStage.FRAGMENT,sampler:{}},
    {binding:3,visibility:GPUShaderStage.FRAGMENT,texture:{}},
    {binding:4,visibility:GPUShaderStage.VERTEX,buffer:{type:'uniform'}}
  ]});
  gpu.pipelineLayout = device.createPipelineLayout({bindGroupLayouts:[gpu.layout]});
  gpu.pipeline = mode => {
    if (gpu.pipelines.has(mode)) return gpu.pipelines.get(mode);
    let color={srcFactor:'src-alpha',dstFactor:'one-minus-src-alpha',operation:'add'};
    let alpha={srcFactor:'one',dstFactor:'one-minus-src-alpha',operation:'add'};
    if (mode===2 || mode===0x20) {color.dstFactor='one';alpha={srcFactor:'zero',dstFactor:'one',operation:'add'};}
    if (mode===0x10 || mode===0x20) color.srcFactor='one';
    if (mode===4) {color={srcFactor:'zero',dstFactor:'src',operation:'add'};alpha={srcFactor:'zero',dstFactor:'one',operation:'add'};}
    if (mode===8) {color.srcFactor='dst';alpha={srcFactor:'zero',dstFactor:'one',operation:'add'};}
    const result=device.createRenderPipeline({layout:gpu.pipelineLayout,
      vertex:{module:gpu.shader,entryPoint:'vs',buffers:[{arrayStride:44,attributes:[
        {shaderLocation:0,offset:0,format:'float32x3'}, {shaderLocation:1,offset:12,format:'float32x4'},
        {shaderLocation:2,offset:28,format:'float32x2'}, {shaderLocation:3,offset:36,format:'float32x2'}]}]},
      fragment:{module:gpu.shader,entryPoint:'fs',targets:[{format:'rgba8unorm',...(mode?{blend:{color,alpha}}:{})}]},
      primitive:{topology:'triangle-list',cullMode:'none'}});
    gpu.pipelines.set(mode,result);return result;
  };
  gpu.create = (w,h) => {
    const texture=device.createTexture({size:[w,h],format:'rgba8unorm',usage:GPUTextureUsage.TEXTURE_BINDING|GPUTextureUsage.COPY_DST|GPUTextureUsage.COPY_SRC|GPUTextureUsage.RENDER_ATTACHMENT});
    const uniform=device.createBuffer({size:16,usage:GPUBufferUsage.UNIFORM|GPUBufferUsage.COPY_DST});
    device.queue.writeBuffer(uniform,0,new Float32Array([w,h,0,0]));
    const id=gpu.nextId++;gpu.images.set(id,{texture,view:texture.createView(),uniform,w,h,id});return id;
  };
  gpu.destroy = id => { const im=gpu.images.get(id);if(!im)return;im.texture.destroy();im.uniform.destroy();gpu.images.delete(id);gpu.groups.clear(); };
  gpu.upload = (id,x,y,w,h,pointer,pitch) => {
    device.queue.writeTexture({texture:gpu.images.get(id).texture,origin:[x,y]},HEAPU8.subarray(pointer,pointer+pitch*(h-1)+w*4),{bytesPerRow:pitch,rowsPerImage:h},[w,h]);
  };
  gpu.resize = (w,h) => {
    if(gpu.screen && gpu.width===w && gpu.height===h)return;
    if(gpu.screen)gpu.destroy(gpu.screen.id);
    gpu.screen=gpu.images.get(gpu.create(Math.max(w,1),Math.max(h,1)));gpu.width=w;gpu.height=h;
  };
  gpu.execute = (vp,vbytes,ip,ibytes,cp,count,w,h) => {
    if(gpu.lost)return false;
    gpu.resize(w,h);
    const streaming=(key,size,usage,pointer)=>{
      if(!size)return;
      if(!gpu[key] || gpu[key].size<size){gpu[key]?.destroy();gpu[key]=device.createBuffer({size:Math.max(65536,size),usage:usage|GPUBufferUsage.COPY_DST});}
      device.queue.writeBuffer(gpu[key],0,HEAPU8.subarray(pointer,pointer+size));
    };
    streaming('vertices',vbytes,GPUBufferUsage.VERTEX,vp);streaming('indices',ibytes,GPUBufferUsage.INDEX,ip);
    const encoder=device.createCommandEncoder();let pass=null,active=null;
    for(let i=0;i<count;i++){
      const o=(cp>>2)+i*16, target=HEAPU32[o]?gpu.images.get(HEAPU32[o]):gpu.screen;
      const clear=HEAPU32[o+11];
      if(pass && (active!==target || clear)){pass.end();pass=null;}
      if(!pass){
        pass=encoder.beginRenderPass({colorAttachments:[{view:target.view,loadOp:clear?'clear':'load',storeOp:'store',clearValue:clear?Array.from(HEAPF32.subarray(o+12,o+16)):[0,0,0,0]}]});active=target;
      }
      if(clear)continue;
      const texture=gpu.images.get(HEAPU32[o+1]),light=gpu.images.get(HEAPU32[o+2]),filter=HEAPU32[o+10];
      const key=[texture.id,light.id,target.id,filter].join(':');let group=gpu.groups.get(key);
      if(!group){group=device.createBindGroup({layout:gpu.layout,entries:[{binding:0,resource:filter?gpu.linear:gpu.nearest},{binding:1,resource:texture.view},{binding:2,resource:gpu.linear},{binding:3,resource:light.view},{binding:4,resource:{buffer:target.uniform}}]});gpu.groups.set(key,group);}
      pass.setPipeline(gpu.pipeline(HEAPU32[o+9]));pass.setBindGroup(0,group);pass.setVertexBuffer(0,gpu.vertices);pass.setIndexBuffer(gpu.indices,'uint32');pass.setScissorRect(HEAPU32[o+5],HEAPU32[o+6],HEAPU32[o+7],HEAPU32[o+8]);pass.drawIndexed(HEAPU32[o+4],1,HEAPU32[o+3]);
    }
    pass?.end();device.queue.submit([encoder.finish()]);return true;
  };
  const blitShader=device.createShaderModule({code:`
struct Out { @builtin(position) position:vec4f, @location(0) uv:vec2f };
@vertex fn vs(@builtin(vertex_index) i:u32)->Out { let uv=vec2f(f32((i<<1u)&2u),f32(i&2u));var o:Out;o.position=vec4f(uv.x*2-1,1-uv.y*2,0,1);o.uv=uv;return o; }
@group(0) @binding(0) var s:sampler;@group(0) @binding(1) var image:texture_2d<f32>;
@fragment fn fs(v:Out)->@location(0) vec4f {return textureSample(image,s,v.uv);}`});
  gpu.blit=device.createRenderPipeline({layout:'auto',vertex:{module:blitShader,entryPoint:'vs'},fragment:{module:blitShader,entryPoint:'fs',targets:[{format:gpu.format}]},primitive:{topology:'triangle-list'}});
  gpu.present = () => {
    if(gpu.lost || !gpu.screen)return false;
    const encoder=device.createCommandEncoder();const pass=encoder.beginRenderPass({colorAttachments:[{view:gpu.context.getCurrentTexture().createView(),loadOp:'clear',storeOp:'store',clearValue:[0,0,0,1]}]});
    pass.setPipeline(gpu.blit);pass.setBindGroup(0,device.createBindGroup({layout:gpu.blit.getBindGroupLayout(0),entries:[{binding:0,resource:gpu.nearest},{binding:1,resource:gpu.screen.view}]}));pass.draw(3);pass.end();device.queue.submit([encoder.finish()]);return true;
  };
  gpu.read = async (id,x,y,w,h) => {
    const image=id?gpu.images.get(id):gpu.screen,pitch=Math.ceil(w*4/256)*256;
    const buffer=device.createBuffer({size:pitch*h,usage:GPUBufferUsage.COPY_DST|GPUBufferUsage.MAP_READ});
    const encoder=device.createCommandEncoder();encoder.copyTextureToBuffer({texture:image.texture,origin:[x,y]}, {buffer,bytesPerRow:pitch,rowsPerImage:h},[w,h]);device.queue.submit([encoder.finish()]);await buffer.mapAsync(GPUMapMode.READ);
    const bytes=new Uint8Array(buffer.getMappedRange()),out=new Uint8Array(w*h*4);for(let row=0;row<h;row++)out.set(bytes.subarray(row*pitch,row*pitch+w*4),row*w*4);buffer.unmap();buffer.destroy();return out;
  };
  gpu.shutdown = () => {gpu.context.unconfigure();device.destroy();Module.ownedGPU=null;};
  Module.ownedGPU=gpu;return true;
};
