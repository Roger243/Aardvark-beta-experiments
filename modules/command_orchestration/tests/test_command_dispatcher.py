from modules.command_orchestration.models.command_request import CommandRequest
from modules.command_orchestration.services.command_dispatcher import CommandDispatcher


def test_command_dispatcher_runs_echo() -> None:
    dispatcher = CommandDispatcher()
    request = CommandRequest(command_id="t1", executable="echo", arguments=["ok"])
    assert dispatcher.execute(request) == 0
