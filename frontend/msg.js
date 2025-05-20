// msg.js

const messagesEl = document.getElementById('messages');

const msgSocket = new WebSocket('ws://127.0.0.1:9001/msg');

msgSocket.onmessage = (event) => {
  try {
    const data = JSON.parse(event.data);
    if (data.event && data.label) {
      // 显示消息框
      messagesEl.style.display = 'block';

      const now = new Date();
      const timeStr = now.toLocaleTimeString('zh-CN', { hour12: false });
      const messageText = `${timeStr} ${data.label} ${data.event === 'come' ? '驾到' : '抱头鼠窜了'}`;

      const p = document.createElement('div');
      p.classList.add('message-entry');
      p.textContent = messageText;
      messagesEl.appendChild(p);
      messagesEl.scrollTop = messagesEl.scrollHeight;

      // 5 秒后淡出并移除
      setTimeout(() => {
        p.classList.add('fade-out');
        setTimeout(() => {
          p.remove();

          // 如果消息容器里没有其他消息了，就隐藏
          if (messagesEl.children.length === 0) {
            messagesEl.style.display = 'none';
          }
        }, 1000); // 等待动画完成
      }, 5000);
    }
  } catch (e) {
    console.error('Invalid message JSON:', event.data);
  }
};

msgSocket.onerror = (err) => {
  console.error('Message WebSocket error:', err);
};
