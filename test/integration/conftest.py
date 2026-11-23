# SPDX-FileCopyrightText: 2022-2026 Ryan Sherlock
# SPDX-License-Identifier: MIT

"""Fixtures for the integration tier: run the built application."""

import os
import subprocess
from pathlib import Path

import pytest


@pytest.fixture(scope="session")
def app_binary() -> Path:
    """The application CTest built, passed in as APP_BINARY."""
    binary = os.environ.get("APP_BINARY")
    if not binary:
        pytest.fail("APP_BINARY is not set; run the integration tier through ctest -L integration")
    return Path(binary)


@pytest.fixture
def run_app(app_binary: Path):
    """Run the application with the given words and environment."""

    def run(*words: str, env: dict[str, str] | None = None) -> subprocess.CompletedProcess[str]:
        return subprocess.run(
            [app_binary, *words],
            capture_output=True,
            text=True,
            check=False,
            timeout=30,
            env={**os.environ, **(env or {})},
        )

    return run
