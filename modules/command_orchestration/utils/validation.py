from modules.command_orchestration.models.command_request import CommandRequest


def validate_command_request(request: CommandRequest) -> None:
    if not request.command_id.strip():
        raise ValueError("command_id cannot be empty")
    if not request.executable.strip():
        raise ValueError("executable cannot be empty")
