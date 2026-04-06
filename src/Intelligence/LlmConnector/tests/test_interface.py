from pathlib import Path

from src.Intelligence.LlmConnector import main


def test_initialize_returns_module_name() -> None:
    assert main.initialize() == "LlmConnector"


def test_enforce_single_sentence_truncates_multiple_sentences() -> None:
    text = "First answer. Second answer should be removed."
    assert main._enforce_single_sentence(text) == "First answer."


def test_is_audio_path_detects_existing_audio_file(tmp_path: Path) -> None:
    audio_file = tmp_path / "sample.wav"
    audio_file.write_bytes(b"RIFF")
    assert main._is_audio_path(str(audio_file)) is True
    assert main._is_audio_path(str(tmp_path / "missing.wav")) is False
