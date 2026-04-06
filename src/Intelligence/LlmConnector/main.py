from __future__ import annotations

import importlib
import re
from pathlib import Path
from typing import Any

AUDIO_EXTENSIONS = {".mp3", ".wav", ".m4a", ".flac", ".ogg", ".webm"}
SYSTEM_PROMPT = (
    "You are ACME Inc. customer support and must respond in one sentence maximum, "
    "first asking probing questions to understand the issue unless already clear, "
    "then proposing a fictional fix, offering a refund only if the user is unsatisfied, "
    "and if they accept the refund then search for a mock refund ID and execute it in your response."
)


def initialize() -> str:
    return "LlmConnector"


def _load_dotenv() -> None:
    dotenv = importlib.import_module("dotenv")
    dotenv.load_dotenv()


def _is_audio_path(user_input: str) -> bool:
    file_path = Path(user_input.strip())
    return file_path.is_file() and file_path.suffix.lower() in AUDIO_EXTENSIONS


def _enforce_single_sentence(text: str) -> str:
    condensed = " ".join(text.strip().split())
    if not condensed:
        return "Could you share a bit more detail so I can help?"

    sentence_match = re.search(r"[.!?]", condensed)
    if sentence_match:
        return condensed[: sentence_match.end()].strip()

    return condensed


def transcribe_audio(audio_path: str, model_name: str = "base") -> str:
    whisper = importlib.import_module("whisper")
    model = whisper.load_model(model_name)
    result = model.transcribe(audio_path)
    return str(result.get("text", "")).strip()


class AcmeSupportAgent:
    def __init__(self, model: str = "gpt-4o-mini") -> None:
        _load_dotenv()
        openai = importlib.import_module("openai")
        self.client = openai.OpenAI()
        self.model = model
        self.messages: list[dict[str, str]] = [{"role": "system", "content": SYSTEM_PROMPT}]

    def reply(self, prompt: str) -> str:
        self.messages.append({"role": "user", "content": prompt})
        completion = self.client.chat.completions.create(model=self.model, messages=self.messages)
        raw_reply = completion.choices[0].message.content or ""
        reply = _enforce_single_sentence(raw_reply)
        self.messages.append({"role": "assistant", "content": reply})
        return reply


def _resolve_prompt(raw_input: str) -> str:
    if _is_audio_path(raw_input):
        transcript = transcribe_audio(raw_input)
        return transcript or "I uploaded audio but there was no spoken content detected."

    return raw_input


def run_cli() -> None:
    agent = AcmeSupportAgent()
    print("ACME Support is ready; type a message, an audio file path, or 'exit' to quit.")

    while True:
        user_input = input("You: ").strip()
        if not user_input:
            continue
        if user_input.lower() in {"exit", "quit"}:
            print("ACME: Thanks for contacting ACME support.")
            break

        prompt = _resolve_prompt(user_input)
        response = agent.reply(prompt)
        print(f"ACME: {response}")


if __name__ == "__main__":
    run_cli()
