const form = document.querySelector("#guess-form");
const input = document.querySelector("#guess-input");
const message = document.querySelector("#message");
const attemptsCount = document.querySelector("#attempts-count");
const restartButton = document.querySelector("#restart-button");

let secretNumber;
let attempts;
let gameFinished;

function generateSecretNumber() {
  return Math.floor(Math.random() * 100) + 1;
}

function setMessage(text, type = "") {
  message.textContent = text;
  message.className = `message ${type}`.trim();
}

function resetGame() {
  secretNumber = generateSecretNumber();
  attempts = 0;
  gameFinished = false;
  attemptsCount.textContent = attempts;
  input.value = "";
  input.disabled = false;
  form.querySelector('button[type="submit"]').disabled = false;
  setMessage("Введите число и сделайте первую попытку.");
  input.focus();
}

function checkGuess(event) {
  event.preventDefault();

  if (gameFinished) {
    return;
  }

  const guess = Number(input.value);

  if (!Number.isInteger(guess) || guess < 1 || guess > 100) {
    setMessage("Введите целое число от 1 до 100.", "warning");
    input.focus();
    return;
  }

  attempts += 1;
  attemptsCount.textContent = attempts;

  if (guess === secretNumber) {
    gameFinished = true;
    input.disabled = true;
    form.querySelector('button[type="submit"]').disabled = true;
    setMessage(`Верно! Вы угадали число ${secretNumber} за ${attempts} ${getAttemptWord(attempts)}.`, "success");
    return;
  }

  if (guess < secretNumber) {
    setMessage("Загаданное число больше. Попробуйте ещё раз.");
  } else {
    setMessage("Загаданное число меньше. Попробуйте ещё раз.");
  }

  input.select();
  input.focus();
}

function getAttemptWord(count) {
  const lastTwoDigits = count % 100;
  const lastDigit = count % 10;

  if (lastTwoDigits >= 11 && lastTwoDigits <= 14) {
    return "попыток";
  }

  if (lastDigit === 1) {
    return "попытку";
  }

  if (lastDigit >= 2 && lastDigit <= 4) {
    return "попытки";
  }

  return "попыток";
}

form.addEventListener("submit", checkGuess);
restartButton.addEventListener("click", resetGame);

resetGame();
