import {
  statusEl, videoCtx, videoCanvas, backgroundCtx, backgroundCanvas,
  countFrameAndUpdateFPS
} from './utils.js';

import { faceDataRaw, smoothFaces, drawFaceBoxes, faceDataSmoothed } from './face.js';
import { checkFaces } from './msg.js';

const ws = new WebSocket('ws://127.0.0.1:9001/stream');
ws.binaryType = 'arraybuffer';

ws.onopen = () => {
  statusEl.textContent = 'connected';
};

ws.onmessage = async (event) => {
  const arrayBuffer = event.data;
  const blob = new Blob([arrayBuffer], { type: 'image/jpeg' });
  try {
    const bitmap = await createImageBitmap(blob);
    videoCtx.drawImage(bitmap, 0, 0, videoCanvas.width, videoCanvas.height);

    backgroundCtx.clearRect(0, 0, backgroundCanvas.width, backgroundCanvas.height);
    const scale = Math.max(
      backgroundCanvas.width / bitmap.width,
      backgroundCanvas.height / bitmap.height
    );
    const x = (backgroundCanvas.width - bitmap.width * scale) / 2;
    const y = (backgroundCanvas.height - bitmap.height * scale) / 2;
    backgroundCtx.drawImage(bitmap, x, y, bitmap.width * scale, bitmap.height * scale);

    smoothFaces(faceDataRaw);
    drawFaceBoxes(faceDataSmoothed);
    checkFaces();

    countFrameAndUpdateFPS();
  } catch (e) {
    console.error('error drawing frame:', e);
  }
};

ws.onclose = () => {
  statusEl.textContent = 'closed';
};
ws.onerror = (err) => {
  console.error('WebSocket error:', err);
  statusEl.textContent = 'error connecting';
};
