// Canvas and UI references
const canvas = document.getElementById('gameCanvas');
const ctx = canvas.getContext('2d');
const scoreEl = document.getElementById('score');
const highScoreEl = document.getElementById('highScore');
const statusMessageEl = document.getElementById('statusMessage');
const startScreenEl = document.getElementById('startScreen');
const startButtonEl = document.getElementById('startButton');
const gameWrapperEl = document.getElementById('gameWrapper');
const darkThemeBtn = document.getElementById('darkThemeBtn');
const neonThemeBtn = document.getElementById('neonThemeBtn');
const snakeColorPicker = document.getElementById('snakeColorPicker');
const foodColorPicker = document.getElementById('foodColorPicker');

// Grid constants
const GRID_SIZE = 20; // Pixel size of each square tile
const TILE_COUNT = canvas.width / GRID_SIZE; // Number of tiles per row/column

const HIGH_SCORE_KEY = 'snakeHighScore';

// Timing constants
const BASE_SPEED_MS = 140;
const MIN_SPEED_MS = 70;
const SPEED_STEP_MS = 6; // Decrease interval each time speed level increases
const POINTS_PER_SPEED_LEVEL = 2; // Increase speed every N points

// Food animation constants (subtle pulse)
const FOOD_PULSE_SPEED = 0.006;
const FOOD_MIN_SCALE = 0.82;
const FOOD_MAX_SCALE = 1.0;

// Direction helpers
const DIRECTIONS = {
  ArrowUp: { x: 0, y: -1 },
  ArrowDown: { x: 0, y: 1 },
  ArrowLeft: { x: -1, y: 0 },
  ArrowRight: { x: 1, y: 0 }
};

// Mutable game state kept in one object for clarity
const state = {
  snake: [],
  direction: { x: 1, y: 0 },
  nextDirection: { x: 1, y: 0 },
  food: { x: 0, y: 0 },
  score: 0,
  highScore: 0,
  isPaused: false,
  loopId: null,
  tickMs: BASE_SPEED_MS,
  hasStarted: false
};

// Read high score from localStorage once when the page loads
function loadHighScore() {
  const storedHighScore = Number(localStorage.getItem(HIGH_SCORE_KEY));
  state.highScore = Number.isFinite(storedHighScore) ? storedHighScore : 0;
  updateScoreUI();
}

// Persist high score whenever the player beats it
function updateHighScore() {
  if (state.score > state.highScore) {
    state.highScore = state.score;
    localStorage.setItem(HIGH_SCORE_KEY, String(state.highScore));
    updateScoreUI();
  }
}

// Start (or restart) the game with fresh state
function startGame() {
  state.hasStarted = true;
  state.snake = [
    { x: 10, y: 10 },
    { x: 9, y: 10 },
    { x: 8, y: 10 }
  ];

  state.direction = { x: 1, y: 0 };
  state.nextDirection = { ...state.direction };
  state.score = 0;
  state.tickMs = BASE_SPEED_MS;
  state.isPaused = false;

  updateScoreUI();
  setStatusMessage('');
  placeFood();
  restartLoop();
  render();
}

// Control the timer using the current speed
function restartLoop() {
  if (state.loopId) {
    clearInterval(state.loopId);
  }

  state.loopId = setInterval(updateGame, state.tickMs);
}

// Speed up the snake progressively based on score (to a safe minimum)
function recalculateSpeed() {
  const speedLevel = Math.floor(state.score / POINTS_PER_SPEED_LEVEL);
  const fasterTick = BASE_SPEED_MS - speedLevel * SPEED_STEP_MS;
  const nextTickMs = Math.max(MIN_SPEED_MS, fasterTick);

  // Restart the timer only if the speed actually changed
  if (nextTickMs !== state.tickMs) {
    state.tickMs = nextTickMs;
    restartLoop();
  }
}

// Put food on any tile that is not occupied by the snake
function placeFood() {
  let newFood;

  do {
    newFood = {
      x: Math.floor(Math.random() * TILE_COUNT),
      y: Math.floor(Math.random() * TILE_COUNT)
    };
  } while (state.snake.some((segment) => segment.x === newFood.x && segment.y === newFood.y));

  state.food = newFood;
}

// Draw one tile in a specified color
function drawTile(x, y, color) {
  ctx.fillStyle = color;
  ctx.fillRect(x * GRID_SIZE, y * GRID_SIZE, GRID_SIZE - 1, GRID_SIZE - 1);
}

// Read the active visual colors from CSS variables
function getThemeColors() {
  const styles = getComputedStyle(gameWrapperEl);

  return {
    snakeHead: styles.getPropertyValue('--snake-head-color').trim() || '#22c55e',
    snakeBody: styles.getPropertyValue('--snake-body-color').trim() || '#16a34a',
    food: styles.getPropertyValue('--food-color').trim() || '#ef4444'
  };
}

// Apply one of the predefined visual themes
function applyTheme(themeName) {
  gameWrapperEl.classList.toggle('theme-dark', themeName === 'dark');
  gameWrapperEl.classList.toggle('theme-neon', themeName === 'neon');
}

// True when directions are exact opposites (prevents instant 180 turns)
function isOppositeDirection(current, next) {
  return current.x + next.x === 0 && current.y + next.y === 0;
}

// Draw food with a subtle pulse animation to make it feel alive
function drawAnimatedFood() {
  const pulse = (Math.sin(Date.now() * FOOD_PULSE_SPEED) + 1) / 2; // 0..1
  const scale = FOOD_MIN_SCALE + pulse * (FOOD_MAX_SCALE - FOOD_MIN_SCALE);

  const centerX = (state.food.x + 0.5) * GRID_SIZE;
  const centerY = (state.food.y + 0.5) * GRID_SIZE;
  const size = (GRID_SIZE - 2) * scale;

  const themeColors = getThemeColors();
  ctx.fillStyle = themeColors.food;
  ctx.fillRect(centerX - size / 2, centerY - size / 2, size, size);
}

// Main game tick
function updateGame() {
  if (state.isPaused) {
    return;
  }

  state.direction = { ...state.nextDirection };

  const head = {
    x: state.snake[0].x + state.direction.x,
    y: state.snake[0].y + state.direction.y
  };

  const hitWall = head.x < 0 || head.x >= TILE_COUNT || head.y < 0 || head.y >= TILE_COUNT;
  const hitSelf = state.snake.some((segment) => segment.x === head.x && segment.y === head.y);

  // Restart immediately if the snake crashes
  if (hitWall || hitSelf) {
    setStatusMessage('Crashed! Restarting...');
    startGame();
    return;
  }

  state.snake.unshift(head);

  // Eat food and speed up
  if (head.x === state.food.x && head.y === state.food.y) {
    state.score += 1;
    updateScoreUI();
    updateHighScore();
    placeFood();
    recalculateSpeed();
  } else {
    state.snake.pop();
  }

  render();
}

// Render current game frame
function render() {
  ctx.clearRect(0, 0, canvas.width, canvas.height);

  // Food (animated pulse)
  drawAnimatedFood();

  // Snake (head + body)
  const themeColors = getThemeColors();
  state.snake.forEach((segment, index) => {
    drawTile(segment.x, segment.y, index === 0 ? themeColors.snakeHead : themeColors.snakeBody);
  });

  // Optional pause overlay
  if (state.isPaused) {
    ctx.fillStyle = 'rgba(0, 0, 0, 0.35)';
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    ctx.fillStyle = '#ffffff';
    ctx.font = 'bold 28px Arial';
    ctx.textAlign = 'center';
    ctx.fillText('Paused', canvas.width / 2, canvas.height / 2);
  }
}

function updateScoreUI() {
  scoreEl.textContent = state.score;
  highScoreEl.textContent = state.highScore;
}

function setStatusMessage(message) {
  statusMessageEl.textContent = message;
}

function togglePause() {
  if (!state.hasStarted) {
    return;
  }

  state.isPaused = !state.isPaused;
  setStatusMessage(state.isPaused ? 'Paused' : 'Resumed');
  render();
}

// Keyboard controls for movement + pause/resume
window.addEventListener('keydown', (event) => {
  if (!state.hasStarted && event.key.toLowerCase() !== 'p') {
    return;
  }

  const next = DIRECTIONS[event.key];

  // Pause/resume on P key
  if (event.key.toLowerCase() === 'p') {
    togglePause();
    event.preventDefault();
    return;
  }

  if (!next) {
    return;
  }

  // Ignore direction changes while paused
  if (state.isPaused) {
    event.preventDefault();
    return;
  }

  // Prevent 180-degree turns
  if (!isOppositeDirection(state.direction, next)) {
    state.nextDirection = next;
  }

  event.preventDefault();
});

// Re-render continuously so subtle visual animations stay smooth
function animationLoop() {
  if (state.hasStarted) {
    render();
  }

  requestAnimationFrame(animationLoop);
}

// Theme controls (visual-only; no gameplay logic changes)
darkThemeBtn.addEventListener('click', () => {
  applyTheme('dark');
});

neonThemeBtn.addEventListener('click', () => {
  applyTheme('neon');
});

// Custom color pickers override snake and food colors in either theme
snakeColorPicker.addEventListener('input', (event) => {
  const base = event.target.value;
  gameWrapperEl.style.setProperty('--snake-head-color', base);
  gameWrapperEl.style.setProperty('--snake-body-color', base);
});

foodColorPicker.addEventListener('input', (event) => {
  gameWrapperEl.style.setProperty('--food-color', event.target.value);
});

loadHighScore();
requestAnimationFrame(animationLoop);

// Start only when the player clicks the Start button
startButtonEl.addEventListener('click', () => {
  startScreenEl.classList.add('hidden');
  startGame();
});
