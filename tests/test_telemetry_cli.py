"""Integration tests for the C++ application's telemetry option."""

import csv
import math
import os
import subprocess
import sys
import tempfile
import unittest
from datetime import datetime
from pathlib import Path


class TelemetryCliTests(unittest.TestCase):
    project_root = Path(__file__).resolve().parents[1]

    def application_path(self):
        configured = os.environ.get("RTOS_HEALTH_MONITOR_EXECUTABLE")
        candidates = []
        if configured:
            candidates.append(Path(configured))
        if len(sys.argv) > 1:
            candidates.append(Path(sys.argv[1]))
        candidates.extend(
            (
                self.project_root / "build-wsl" / "rtos_health_monitor",
                self.project_root / "build-wsl" / "rtos_health_monitor.exe",
                self.project_root / "build" / "Debug" / "rtos_health_monitor.exe",
                self.project_root / "build" / "rtos_health_monitor.exe",
            )
        )

        for candidate in candidates:
            if candidate.is_file():
                return str(candidate)

        self.fail(
            "Could not find the rtos_health_monitor executable. "
            "Set RTOS_HEALTH_MONITOR_EXECUTABLE or build the project."
        )

    def run_application(self, directory, *arguments):
        return subprocess.run(
            [self.application_path(), *arguments],
            cwd=directory,
            capture_output=True,
            text=True,
            timeout=30,
            check=False,
        )

    def test_telemetry_csv_has_expected_content(self):
        with tempfile.TemporaryDirectory() as directory:
            destination = Path(directory) / "session.csv"

            result = self.run_application(
                directory, "--telemetry", str(destination)
            )

            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertTrue(destination.is_file())
            self.assertIn("Telemetry saved to:", result.stdout)

            with destination.open("r", encoding="utf-8", newline="") as file:
                reader = csv.DictReader(file)
                self.assertEqual(
                    reader.fieldnames,
                    [
                        "timestamp",
                        "temperature_c",
                        "voltage_v",
                        "comms_ok",
                    ],
                )

                rows = list(reader)

            self.assertGreater(len(rows), 0)

            previous = None
            for row in rows:
                self.assertTrue(row["timestamp"].endswith("Z"))
                timestamp = datetime.fromisoformat(
                    row["timestamp"].replace("Z", "+00:00")
                )
                if previous is not None:
                    self.assertGreater(timestamp, previous)
                previous = timestamp

                self.assertTrue(math.isfinite(float(row["temperature_c"])))
                self.assertTrue(math.isfinite(float(row["voltage_v"])))
                self.assertIn(row["comms_ok"], ("true", "false"))

    def test_existing_file_is_preserved(self):
        with tempfile.TemporaryDirectory() as directory:
            destination = Path(directory) / "existing.csv"
            destination.write_text("keep this file", encoding="utf-8")

            result = self.run_application(
                directory, "--telemetry", str(destination)
            )

            self.assertNotEqual(result.returncode, 0)
            self.assertIn("already exists", result.stderr)
            self.assertEqual(
                destination.read_text(encoding="utf-8"), "keep this file"
            )

    def test_missing_parent_folder(self):
        with tempfile.TemporaryDirectory() as directory:
            destination = Path(directory) / "missing" / "session.csv"

            result = self.run_application(
                directory, "--telemetry", str(destination)
            )

            self.assertNotEqual(result.returncode, 0)
            self.assertIn("Cannot open telemetry destination", result.stderr)
            self.assertFalse(destination.exists())

    def test_invalid_arguments(self):
        with tempfile.TemporaryDirectory() as directory:
            for arguments in (
                ("--unknown",),
                ("--telemetry",),
                ("--telemetry", "session.csv", "extra"),
            ):
                with self.subTest(arguments=arguments):
                    result = self.run_application(directory, *arguments)
                    self.assertNotEqual(result.returncode, 0)
                    self.assertIn("Usage:", result.stderr)


if __name__ == "__main__":
    unittest.main(argv=[sys.argv[0]])
