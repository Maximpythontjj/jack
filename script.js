const canvas = document.querySelector('.particle-canvas');
const ctx = canvas.getContext('2d');

function resize() {
  const dpr = window.devicePixelRatio || 1;
  const rect = canvas.getBoundingClientRect();
  canvas.width = rect.width * dpr;
  canvas.height = rect.height * dpr;
  ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
  draw();
}

const cards = [
  { x: 120, y: 120, w: 150, h: 80, glow: 'rgba(255,255,255,0.08)' },
  { x: 420, y: 200, w: 180, h: 90, glow: 'rgba(80,130,255,0.12)' },
  { x: 220, y: 420, w: 150, h: 80, glow: 'rgba(255,255,255,0.1)' },
  { x: window.innerWidth - 320, y: 140, w: 170, h: 90, glow: 'rgba(90,120,255,0.14)' },
  { x: window.innerWidth - 460, y: 420, w: 160, h: 80, glow: 'rgba(255,255,255,0.07)' },
  { x: window.innerWidth / 2 - 90, y: window.innerHeight - 180, w: 180, h: 90, glow: 'rgba(70,90,200,0.16)' }
];

function drawCard(card) {
  ctx.save();
  ctx.translate(card.x, card.y);
  ctx.fillStyle = card.glow;
  ctx.shadowColor = card.glow;
  ctx.shadowBlur = 80;
  ctx.fillRect(0, 0, card.w, card.h);

  ctx.fillStyle = 'rgba(8, 11, 18, 0.78)';
  ctx.shadowColor = 'rgba(0, 0, 0, 0.6)';
  ctx.shadowBlur = 40;
  ctx.beginPath();
  const radius = 16;
  const { w, h } = card;
  ctx.moveTo(radius, 0);
  ctx.lineTo(w - radius, 0);
  ctx.quadraticCurveTo(w, 0, w, radius);
  ctx.lineTo(w, h - radius);
  ctx.quadraticCurveTo(w, h, w - radius, h);
  ctx.lineTo(radius, h);
  ctx.quadraticCurveTo(0, h, 0, h - radius);
  ctx.lineTo(0, radius);
  ctx.quadraticCurveTo(0, 0, radius, 0);
  ctx.closePath();
  ctx.fill();

  ctx.fillStyle = 'rgba(255, 255, 255, 0.18)';
  ctx.fillRect(24, 24, w - 48, 10);
  ctx.fillStyle = 'rgba(255, 255, 255, 0.12)';
  ctx.fillRect(24, 46, (w - 48) * 0.65, 8);
  ctx.restore();
}

function draw() {
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  cards.forEach(drawCard);
}

window.addEventListener('resize', () => {
  cards[3].x = window.innerWidth - 320;
  cards[4].x = window.innerWidth - 460;
  cards[5].x = window.innerWidth / 2 - 90;
  cards[5].y = window.innerHeight - 180;
  resize();
});

resize();

document.body.classList.add('is-loaded');
