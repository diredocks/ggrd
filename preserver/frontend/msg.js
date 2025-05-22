// msg.js
import { faceDataRaw } from './face.js';

const DISAPPEAR_DELAY = 500;
const CHECK_INTERVAL = 500;

const seenFaces = new Map(); // id -> { lastSeen, visible, label }

function showMessage(text) {
  const msgBox = document.getElementById('msg');
  if (!msgBox) return;

  const div = document.createElement('div');
  div.className = 'message';
  div.textContent = text;
  msgBox.appendChild(div);

  setTimeout(() => {
    div.remove();
  }, 3000);
}

export function checkFaces() {
  const now = Date.now();
  const currentIds = new Set(faceDataRaw.map(f => f.id));

  // 处理当前帧中的面孔
  for (const f of faceDataRaw) {
    const { id, label } = f;

    if (!seenFaces.has(id)) {
      // 新人脸
      seenFaces.set(id, { lastSeen: now, visible: true, label });
      showMessage(`👋 ${label} 驾到！`);
    } else {
      // 已知人脸更新
      const info = seenFaces.get(id);
      info.lastSeen = now;

      if (!info.visible) {
        info.visible = true;
        showMessage(`🤝 ${info.label} 驾到！`);
      }
    }
  }

  // 检查是否有人脸消失
  for (const [id, info] of seenFaces.entries()) {
    if (!currentIds.has(id)) {
      if (info.visible && now - info.lastSeen > DISAPPEAR_DELAY) {
        info.visible = false;
        showMessage(`🐭 ${info.label} 抱头鼠窜了`);
      }
    }
  }
}
