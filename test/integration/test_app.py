# SPDX-FileCopyrightText: 2022-2026 Ryan Sherlock
# SPDX-License-Identifier: MIT

"""The application as a user runs it: arguments in, one line per word out."""


def test_new_then_seen(run_app):
    result = run_app("alpha", "beta", "alpha")
    assert result.returncode == 0
    assert result.stdout.splitlines() == ["new alpha", "new beta", "seen alpha"]


def test_no_words_prints_nothing(run_app):
    result = run_app()
    assert result.returncode == 0
    assert result.stdout == ""


def test_every_repeat_is_seen(run_app):
    words = [f"word-{i}" for i in range(50)]
    result = run_app(*words, *words)
    lines = result.stdout.splitlines()
    assert len(lines) == 100
    assert lines[50:] == [f"seen {word}" for word in words]


def test_words_keep_their_spelling(run_app):
    result = run_app("Hello", "hello", "héllo")
    assert result.stdout.splitlines()[0] == "new Hello"
    assert [line.split(" ", 1)[1] for line in result.stdout.splitlines()] == [
        "Hello",
        "hello",
        "héllo",
    ]


def test_debug_log_goes_to_stderr(run_app):
    result = run_app("alpha", env={"SPDLOG_LEVEL": "debug"})
    assert result.stdout.splitlines() == ["new alpha"]
    assert "Observing word alpha" in result.stderr


def test_quiet_by_default(run_app):
    result = run_app("alpha")
    assert result.stderr == ""
