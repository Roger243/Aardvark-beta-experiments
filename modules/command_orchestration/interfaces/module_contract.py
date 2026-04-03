from typing import Protocol


class ModuleContract(Protocol):
    name: str

    def start(self) -> bool:
        ...

    def stop(self) -> None:
        ...
