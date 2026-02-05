// Snake game setup
const canvas = document.getElementById('gameCanvas');
const ctx = canvas.getContext('2d');
const scoreEl = document.getElementById('score');

const gridSize = 20; // Size of each square in pixels
const tileCount = canvas.width / gridSize; // Number of tiles per row/column

let snake;
let direction;
let nextDirection;
let food;
let score;
let gameLoopId;

// Initialize and start a new game
function startGame() {
  snake = [
    { x: 10, y: 10 },
    { x: 9, y: 10 },
    { x: 8, y: 10 }
  ];

  direction = { x: 1, y: 0 };
  nextDirection = { ...direction };
  score = 0;
  scoreEl.textContent = score;

  placeFood();

  if (gameLoopId) {
    clearInterval(gameLoopId);
  }

  // Run game at a steady speed
  gameLoopId = setInterval(updateGame, 120);
}

// Place food in a random empty tile
function placeFood() {
  let newFood;

  do {
    newFood = {
      x: Math.floor(Math.random() * tileCount),
      y: Math.floor(Math.random() * tileCount)
    };
  } while (snake.some((segment) => segment.x === newFood.x && segment.y === newFood.y));

  food = newFood;
}

// Draw one square on the canvas
function drawTile(x, y, color) {
  ctx.fillStyle = color;
  ctx.fillRect(x * gridSize, y * gridSize, gridSize - 1, gridSize - 1);
}

// Main update loop
function updateGame() {
  direction = { ...nextDirection };

  const head = {
    x: snake[0].x + direction.x,
    y: snake[0].y + direction.y
  };

  // Check wall collision
  const hitWall = head.x < 0 || head.x >= tileCount || head.y < 0 || head.y >= tileCount;

  // Check self collision
  const hitSelf = snake.some((segment) => segment.x === head.x && segment.y === head.y);

  // Restart game if collision happens
  if (hitWall || hitSelf) {
    startGame();
    return;
  }

  snake.unshift(head);

  // If food is eaten, increase score and place new food
  if (head.x === food.x && head.y === food.y) {
    score += 1;
    scoreEl.textContent = score;
    placeFood();
  } else {
    snake.pop();
  }

  render();
}

// Draw all game elements
function render() {
  ctx.clearRect(0, 0, canvas.width, canvas.height);

  // Draw food
  drawTile(food.x, food.y, '#ef4444');

  // Draw snake
  snake.forEach((segment, index) => {
    drawTile(segment.x, segment.y, index === 0 ? '#22c55e' : '#16a34a');
  });
}

// Handle arrow key controls
window.addEventListener('keydown', (event) => {
  switch (event.key) {
    case 'ArrowUp':
      if (direction.y !== 1) nextDirection = { x: 0, y: -1 };
      break;
    case 'ArrowDown':
      if (direction.y !== -1) nextDirection = { x: 0, y: 1 };
      break;
    case 'ArrowLeft':
      if (direction.x !== 1) nextDirection = { x: -1, y: 0 };
      break;
    case 'ArrowRight':
      if (direction.x !== -1) nextDirection = { x: 1, y: 0 };
      break;
    default:
      return;
  }

  event.preventDefault();
});

startGame();
render();
