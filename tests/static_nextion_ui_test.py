from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text(encoding="utf-8")


def test_nextion_ui_contract_symbols_exist():
    nextion_h = read("src/nextion.h")
    app_c = read("src/app.c")
    main_c = read("src/main.c")

    required_symbols = [
        "NEXTION_CONTROL_START",
        "NEXTION_CONTROL_CALIBRATION",
        "NEXTION_CONTROL_STOP",
        "NEXTION_CONTROL_TARE",
        "NEXTION_CONTROL_MANUAL",
        "NEXTION_CONTROL_MANUAL_A_LEFT",
        "NEXTION_CONTROL_MANUAL_A_RIGHT",
        "NEXTION_CONTROL_MANUAL_B_LEFT",
        "NEXTION_CONTROL_MANUAL_B_RIGHT",
        "NEXTION_CONTROL_MANUAL_V_LEFT",
        "NEXTION_CONTROL_MANUAL_V_RIGHT",
        "NEXTION_CONTROL_MANUAL_SERVO_OPEN",
        "NEXTION_CONTROL_MANUAL_SERVO_CLOSE",
        "NEXTION_CONTROL_BACK",
        "NEXTION_CONTROL_STOP_ALL",
        "nextion_set_status",
        "nextion_set_step",
        "nextion_set_load_progress",
    ]

    combined = "\n".join([nextion_h, app_c, main_c])

    for symbol in required_symbols:
        assert symbol in combined


def test_app_has_safety_controls_and_manual_mode():
    app_c = read("src/app.c")

    required_states = [
        "APP_MANUAL_A_LEFT",
        "APP_MANUAL_A_RIGHT",
        "APP_MANUAL_B_LEFT",
        "APP_MANUAL_B_RIGHT",
        "APP_MANUAL_V_LEFT",
        "APP_MANUAL_V_RIGHT",
        "APP_MANUAL_SERVO_OPEN",
        "APP_MANUAL_SERVO_CLOSE",
    ]

    for state in required_states:
        assert state in app_c

    assert "handle_tare_request" in app_c
    assert "handle_stop_request" in app_c


def test_load_display_uses_only_text_kg_field():
    config_h = read("src/config.h")
    main_c = read("src/main.c")
    nextion_c = read("src/nextion.c")
    readme = read("README.md")

    combined_firmware = "\n".join([config_h, main_c, nextion_c])

    assert "load_number_name" not in combined_firmware
    assert "CFG_NEXTION_LOAD_NUMBER_NAME" not in combined_firmware
    assert '"nLoad"' not in combined_firmware
    assert 'uart2_write_str(" kg\\"")' in nextion_c
    assert "nLoad" not in readme


if __name__ == "__main__":
    test_nextion_ui_contract_symbols_exist()
    test_app_has_safety_controls_and_manual_mode()
    test_load_display_uses_only_text_kg_field()
    print("static_nextion_ui_test OK")
