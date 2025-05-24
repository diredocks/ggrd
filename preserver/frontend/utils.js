export const statusEl = document.getElementById('status');
export const fpsEl = document.getElementById('fps');
export const tpsEl = document.getElementById('tps');
export const videoCanvas = document.getElementById('videoCanvas');
export const videoCtx = videoCanvas.getContext('2d');
export const backgroundCanvas = document.getElementById('backgroundCanvas');
export const backgroundCtx = backgroundCanvas.getContext('2d');

export function resizeBackground() {
  backgroundCanvas.width = window.innerWidth;
  backgroundCanvas.height = window.innerHeight;
}
window.addEventListener('resize', resizeBackground);
resizeBackground();

let frameCount = 0;
let tickCount = 0;
let lastTime = performance.now();
let lastTimeTps = performance.now();

export function countFrameAndUpdateFPS() {
  const now = performance.now();
  const delta = now - lastTime;
  if (delta >= 1000) {
    const fps = Math.round((frameCount * 1000) / delta);
    fpsEl.textContent = `FPS: ${fps}`;
    frameCount = 0;
    lastTime = now;
  }
  frameCount++;
}

export function countTickAndUpdateTPS() {
  const now = performance.now();
  const delta = now - lastTimeTps;
  if (delta >= 1000) {
    const tps = Math.round((tickCount * 1000) / delta);
    tpsEl.textContent = `TPS: ${tps}`;
    tickCount = 0;
    lastTimeTps = now;
  }
  tickCount++;
}
