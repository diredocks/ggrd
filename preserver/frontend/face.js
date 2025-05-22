import { videoCtx } from './utils.js';
import { config } from './config.js';

export let faceDataRaw = [];
export let faceDataSmoothed = [];

const SMOOTHING_ALPHA = 0.3;

function euclideanDist(a, b) {
  const dx = a.x - b.x;
  const dy = a.y - b.y;
  return Math.sqrt(dx * dx + dy * dy);
}

export function smoothFaces(rawFaces) {
  const used = new Set();
  const smoothed = [];

  for (const raw of rawFaces) {
    let bestMatch = null;
    let bestDist = Infinity;
    let bestIndex = -1;

    for (let i = 0; i < faceDataSmoothed.length; i++) {
      if (used.has(i)) continue;

      const dist = euclideanDist(raw, faceDataSmoothed[i]);
      if (dist < bestDist) {
        bestDist = dist;
        bestMatch = faceDataSmoothed[i];
        bestIndex = i;
      }
    }

    if (bestMatch && bestDist < 100) { // 100 px threshold for matching
      used.add(bestIndex);
      smoothed.push({
        x: bestMatch.x + (raw.x - bestMatch.x) * SMOOTHING_ALPHA,
        y: bestMatch.y + (raw.y - bestMatch.y) * SMOOTHING_ALPHA,
        w: bestMatch.w + (raw.w - bestMatch.w) * SMOOTHING_ALPHA,
        h: bestMatch.h + (raw.h - bestMatch.h) * SMOOTHING_ALPHA,
        label: raw.label
      });
    } else {
      // New face
      smoothed.push(raw);
    }
  }

  faceDataSmoothed = smoothed;
}

function drawRoundedRect(ctx, x, y, width, height, radius) {
  ctx.beginPath();
  ctx.moveTo(x + radius, y);
  ctx.lineTo(x + width - radius, y);
  ctx.quadraticCurveTo(x + width, y, x + width, y + radius);
  ctx.lineTo(x + width, y + height - radius);
  ctx.quadraticCurveTo(x + width, y + height, x + width - radius, y + height);
  ctx.lineTo(x + radius, y + height);
  ctx.quadraticCurveTo(x, y + height, x, y + height - radius);
  ctx.lineTo(x, y + radius);
  ctx.quadraticCurveTo(x, y, x + radius, y);
  ctx.closePath();
}

export function drawFaceBoxes(faces) {
  videoCtx.lineWidth = 2;
  videoCtx.font = '16px sans-serif';
  videoCtx.textBaseline = 'top';

  for (const face of faces) {
    const { x, y, w, h, label } = face;

    // Draw rounded yellow rectangle
    drawRoundedRect(videoCtx, x, y, w, h, 8);
    videoCtx.strokeStyle = 'yellow';
    videoCtx.stroke();

    // Draw label background
    const labelWidth = videoCtx.measureText(label).width + 10;
    videoCtx.fillStyle = 'rgba(255, 255, 0, 0.8)'; // semi-transparent yellow
    drawRoundedRect(videoCtx, x, y - 24, labelWidth, 20, 5);
    videoCtx.fill();

    // Draw label text
    videoCtx.fillStyle = 'black';
    videoCtx.fillText(label, x + 5, y - 22);
  }
}

const faceWs = new WebSocket(`${config.wsBaseUrl}${config.endpoints.face}`);
faceWs.onmessage = (event) => {
  try {
    const data = JSON.parse(event.data);
    faceDataRaw.length = 0;
    data.faces?.forEach(f => faceDataRaw.push(f));
  } catch (e) {
    console.error('Invalid face data:', e);
    faceDataRaw.length = 0;
  }
};
