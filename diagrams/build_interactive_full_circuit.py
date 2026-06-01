from pathlib import Path

import plotly.graph_objects as go


OUT_DIR = Path(__file__).resolve().parent

COLORS = {
    "12v": "#d5a100",
    "5v": "#d94b41",
    "gnd": "#2b2b2b",
    "ctrl": "#2f6fdd",
    "servo": "#8c48c9",
    "atx": "#2a9d55",
    "note": "#6c757d",
}


def rect(x0, y0, x1, y1, line="#333", fill="#f8f9fa", dash=None):
    return dict(
        type="rect",
        x0=x0,
        y0=y0,
        x1=x1,
        y1=y1,
        line=dict(color=line, width=2, dash=dash),
        fillcolor=fill,
        layer="below",
    )


def add_box(fig, spec):
    fig.add_shape(
        rect(
            spec["x0"],
            spec["y0"],
            spec["x1"],
            spec["y1"],
            line=spec.get("line", "#333"),
            fill=spec.get("fill", "#f8f9fa"),
            dash=spec.get("dash"),
        )
    )
    fig.add_annotation(
        x=(spec["x0"] + spec["x1"]) / 2,
        y=spec["y1"] - 2,
        text=spec["title"],
        showarrow=False,
        font=dict(size=18 if spec.get("big") else 13, family="Arial", color="#111"),
        align="center",
    )
    if spec.get("lines"):
        fig.add_annotation(
            x=spec["x0"] + 1.2,
            y=spec["y1"] - 6.0,
            text="<br>".join(spec["lines"]),
            showarrow=False,
            xanchor="left",
            yanchor="top",
            align="left",
            font=dict(size=10.5, family="Arial", color="#222"),
        )

    fig.add_trace(
        go.Scatter(
            x=[(spec["x0"] + spec["x1"]) / 2],
            y=[(spec["y0"] + spec["y1"]) / 2],
            mode="markers",
            marker=dict(size=20, opacity=0.01),
            hovertemplate=spec["hover"] + "<extra></extra>",
            showlegend=False,
        )
    )


def add_wire(fig, xs, ys, color, name, hover, legendgroup, dash=None, width=4, showlegend=True):
    fig.add_trace(
        go.Scatter(
            x=xs,
            y=ys,
            mode="lines+markers",
            line=dict(color=color, width=width, dash=dash),
            marker=dict(size=8, color=color, opacity=0.0),
            name=name,
            legendgroup=legendgroup,
            showlegend=showlegend,
            hovertemplate=hover + "<extra></extra>",
        )
    )


def add_note(fig, x, y, text):
    fig.add_annotation(
        x=x,
        y=y,
        text=text,
        showarrow=False,
        align="left",
        xanchor="left",
        yanchor="top",
        bordercolor="#c9c9c9",
        borderwidth=1,
        borderpad=6,
        bgcolor="rgba(255,255,255,0.92)",
        font=dict(size=10.5, color="#222"),
    )


def build_figure():
    fig = go.Figure()

    fig.update_layout(
        title=dict(
            text="Interactive Wiring Plan: GS700 PSU -> External Board -> Mega 2560 -> Servos + 2x L298N",
            x=0.5,
            xanchor="center",
            font=dict(size=22),
        ),
        width=1700,
        height=980,
        paper_bgcolor="#fcfcfd",
        plot_bgcolor="#fcfcfd",
        dragmode="pan",
        hoverlabel=dict(font_size=12),
        margin=dict(l=20, r=20, t=70, b=20),
        legend=dict(
            orientation="h",
            yanchor="bottom",
            y=0.01,
            xanchor="left",
            x=0.01,
            bgcolor="rgba(255,255,255,0.85)",
        ),
        xaxis=dict(visible=False, range=[0, 170], fixedrange=False),
        yaxis=dict(visible=False, range=[0, 100], fixedrange=False, scaleanchor="x", scaleratio=1),
    )

    boxes = [
        dict(
            x0=3, y0=60, x1=26, y1=91,
            title="Corsair GS700 PSU",
            fill="#edf2ff",
            lines=[
                "Use one Molex/peripheral cable as main power source",
                "Yellow = +12V",
                "Red = +5V",
                "Black = GND",
                "Green from 24-pin = PS_ON",
            ],
            hover=(
                "<b>Corsair GS700 PSU</b><br>"
                "Recommended extraction points:<br>"
                "- Molex/peripheral cable for +12V/+5V/GND<br>"
                "- 24-pin green PS_ON wire to turn PSU on<br>"
                "Do not open the PSU enclosure."
            ),
        ),
        dict(
            x0=7, y0=44, x1=22, y1=55,
            title="PSU ON Switch",
            fill="#eef7ee",
            lines=[
                "SPST toggle",
                "green PS_ON -> switch -> black GND",
            ],
            hover=(
                "<b>PSU startup switch</b><br>"
                "ATX PSU starts when green PS_ON is connected to GND.<br>"
                "Use a simple toggle switch mounted on your external board."
            ),
        ),
        dict(
            x0=34, y0=40, x1=72, y1=92,
            title="External Soldered Board",
            fill="#fff8db",
            big=True,
            lines=[
                "Molex IN: +12V, +5V, GND, GND",
                "ATX CTRL IN: green PS_ON",
                "STAR GND point for every return wire",
                "12V fuse rail -> L298N #1 + 7809",
                "5V fuse rail -> servos + L298N logic + L298N #2",
                "7809 + caps -> Mega VIN/barrel",
                "Servo bus: add 470uF..1000uF + 100nF nearby",
            ],
            hover=(
                "<b>External soldered distribution board</b><br>"
                "This is your custom board.<br>"
                "Recommended sections:<br>"
                "- Molex input terminal<br>"
                "- PS_ON switch header<br>"
                "- 12V fused rail<br>"
                "- 5V fused rail<br>"
                "- 7809 regulator for Arduino VIN<br>"
                "- Star-ground copper point<br>"
                "- Screw terminals / headers for outputs"
            ),
        ),
        dict(
            x0=78, y0=51, x1=103, y1=92,
            title="Arduino Mega 2560",
            fill="#eef4ff",
            lines=[
                "VIN/barrel <- 9V from 7809",
                "GND <- common GND",
                "Servo1 SIG <- D22",
                "Servo2 SIG <- D23",
                "L298N #1 ENA <- D5  PWM",
                "L298N #1 IN1 <- D24; IN2 <- D25",
                "L298N #2 ENA <- D6  PWM",
                "L298N #2 IN1 <- D26; IN2 <- D27",
            ],
            hover=(
                "<b>Arduino Mega 2560</b><br>"
                "Recommended pin map in this wiring plan:<br>"
                "- D22 -> Servo 1 signal<br>"
                "- D23 -> Servo 2 signal<br>"
                "- D5 -> L298N #1 ENA (PWM)<br>"
                "- D24/D25 -> L298N #1 direction<br>"
                "- D6 -> L298N #2 ENA (PWM)<br>"
                "- D26/D27 -> L298N #2 direction"
            ),
        ),
        dict(
            x0=79, y0=28, x1=104, y1=46,
            title="Motor Shield Rev3",
            fill="#f3f3f3",
            dash="dash",
            lines=[
                "Optional / not used in",
                "recommended topology",
            ],
            hover=(
                "<b>Arduino Motor Shield Rev3</b><br>"
                "Recommended to leave it out here.<br>"
                "Reason: your 2 servos do not use H-bridges, and your 2 DC motors need different supply voltages."
            ),
        ),
        dict(
            x0=116, y0=74, x1=138, y1=92,
            title="Servo 1",
            fill="#f8efff",
            lines=[
                "Assumption: 3-wire hobby servo",
                "+5V, GND, SIG",
                "Signal from D22",
            ],
            hover=(
                "<b>Servo 1</b><br>"
                "Assumed to be a standard 3-wire hobby servo.<br>"
                "Important: reverse is not done by swapping power wires.<br>"
                "Direction is controlled by the command pulse."
            ),
        ),
        dict(
            x0=116, y0=52, x1=138, y1=70,
            title="Servo 2",
            fill="#f8efff",
            lines=[
                "Assumption: 3-wire hobby servo",
                "+5V, GND, SIG",
                "Signal from D23",
            ],
            hover=(
                "<b>Servo 2</b><br>"
                "If this is a continuous-rotation servo, pulse width controls speed and direction.<br>"
                "If it is a positional servo, pulse width controls angle, not DC-style PWM."
            ),
        ),
        dict(
            x0=111, y0=27, x1=138, y1=45,
            title="L298N Board #1",
            fill="#fff4e8",
            lines=[
                "For 4.5-15V DC motor",
                "VS = +12V rail",
                "5V logic from ext board",
                "Use CH A only",
                "ENA=D5, IN1=D24, IN2=D25",
            ],
            hover=(
                "<b>L298N board #1</b><br>"
                "Recommended target: 4.5-15V DC motor.<br>"
                "Supply it from +12V on VS.<br>"
                "Feed logic 5V externally and keep GND common.<br>"
                "Using channel A only in this plan."
            ),
        ),
        dict(
            x0=111, y0=3, x1=138, y1=21,
            title="L298N Board #2",
            fill="#fff4e8",
            lines=[
                "For 5V DC motor",
                "VS = +5V rail",
                "5V logic from ext board",
                "Use CH A only",
                "ENA=D6, IN1=D26, IN2=D27",
            ],
            hover=(
                "<b>L298N board #2</b><br>"
                "Recommended target: 5V DC motor.<br>"
                "Set VS from the ATX +5V rail.<br>"
                "If your module has a 5V-EN jumper, remove it and feed 5V logic externally.<br>"
                "L298N has voltage drop, so a 5V motor may run weaker than direct drive."
            ),
        ),
        dict(
            x0=145, y0=28, x1=166, y1=44,
            title="4.5-15V DC Motor",
            fill="#eefaf2",
            lines=[
                "OUT1/OUT2 from L298N #1",
                "PWM + reverse supported",
            ],
            hover=(
                "<b>4.5-15V DC motor</b><br>"
                "Connected to L298N #1 output A.<br>"
                "You get on/off, PWM speed control, and reverse."
            ),
        ),
        dict(
            x0=145, y0=4, x1=166, y1=20,
            title="5V DC Motor",
            fill="#eefaf2",
            lines=[
                "OUT1/OUT2 from L298N #2",
                "PWM + reverse supported",
            ],
            hover=(
                "<b>5V DC motor</b><br>"
                "Connected to L298N #2 output A.<br>"
                "You get on/off, PWM speed control, and reverse.<br>"
                "But expect some voltage loss across L298N."
            ),
        ),
    ]

    for box in boxes:
        add_box(fig, box)

    # Power wires
    add_wire(
        fig, [26, 34], [82, 82], COLORS["12v"], "+12V rail", 
        "<b>Wire: PSU Molex yellow -> external board +12V input</b><br>Color: yellow<br>Function: main 12V bus",
        "12V",
    )
    add_wire(
        fig, [26, 34], [76, 76], COLORS["5v"], "+5V rail",
        "<b>Wire: PSU Molex red -> external board +5V input</b><br>Color: red<br>Function: servo and logic 5V bus",
        "5V",
    )
    add_wire(
        fig, [26, 34], [70, 70], COLORS["gnd"], "GND bus",
        "<b>Wire: PSU Molex black -> external board star ground</b><br>Use both black wires if possible.",
        "GND",
    )
    add_wire(
        fig, [26, 34], [67, 67], COLORS["gnd"], "GND bus",
        "<b>Wire: second PSU Molex black -> external board star ground</b><br>Parallel ground return improves current handling.",
        "GND",
        showlegend=False,
    )
    add_wire(
        fig, [14.5, 14.5, 22], [60, 55, 55], COLORS["atx"], "ATX PS_ON",
        "<b>Wire: 24-pin green PS_ON -> toggle switch -> GND</b><br>This turns the PSU on.",
        "ATX",
    )

    add_wire(
        fig, [72, 78], [84, 84], COLORS["12v"], "+9V branch (input to 7809)",
        "<b>Internal branch on external board</b><br>12V rail also feeds the 7809 regulator input.",
        "12V",
        dash="dot",
        showlegend=False,
    )
    add_wire(
        fig, [72, 78], [78, 78], COLORS["5v"], "+5V logic/servo branch",
        "<b>Internal branch on external board</b><br>5V rail goes to servo bus and L298N logic.",
        "5V",
        dash="dot",
        showlegend=False,
    )
    add_wire(
        fig, [72, 78], [71, 71], COLORS["gnd"], "Common GND",
        "<b>External board star ground -> Arduino GND</b><br>Keep all grounds common.",
        "GND",
        showlegend=False,
    )

    add_wire(
        fig, [72, 78], [87, 87], "#7a7a7a", "9V to Mega VIN",
        "<b>Wire: 7809 output -> Arduino VIN/barrel</b><br>Recommended regulator caps: 0.33uF input, 0.1uF output, plus 10uF electrolytics.",
        "MEGA",
    )
    add_wire(
        fig, [72, 116], [76, 83], COLORS["5v"], "+5V to Servo 1",
        "<b>Wire: external board +5V servo bus -> Servo 1 V+</b><br>Add bulk capacitor on the servo bus.",
        "5V",
        showlegend=False,
    )
    add_wire(
        fig, [72, 116], [74, 61], COLORS["5v"], "+5V to Servo 2",
        "<b>Wire: external board +5V servo bus -> Servo 2 V+</b>",
        "5V",
        showlegend=False,
    )
    add_wire(
        fig, [72, 116], [68, 80], COLORS["gnd"], "Servo GND",
        "<b>Wire: external board GND -> Servo 1 GND</b>",
        "GND",
        showlegend=False,
    )
    add_wire(
        fig, [72, 116], [66, 58], COLORS["gnd"], "Servo GND",
        "<b>Wire: external board GND -> Servo 2 GND</b>",
        "GND",
        showlegend=False,
    )

    add_wire(
        fig, [103, 116], [82, 83], COLORS["servo"], "Servo signal",
        "<b>Wire: Mega D22 -> Servo 1 SIG</b><br>Standard servo control pulse, not DC PWM power switching.",
        "SERVO",
    )
    add_wire(
        fig, [103, 116], [78, 61], COLORS["servo"], "Servo signal",
        "<b>Wire: Mega D23 -> Servo 2 SIG</b><br>If continuous-rotation servo: pulse width sets speed/direction.",
        "SERVO",
        showlegend=False,
    )

    add_wire(
        fig, [72, 111], [86, 36], COLORS["12v"], "+12V to L298N #1 VS",
        "<b>Wire: external board +12V -> L298N #1 VS</b><br>Use fuse on this branch sized to your motor current.",
        "12V",
        showlegend=False,
    )
    add_wire(
        fig, [72, 111], [76, 12], COLORS["5v"], "+5V to L298N #2 VS/logic",
        "<b>Wire: external board +5V -> L298N #2 VS and logic 5V</b><br>If your board has a 5V-EN jumper, remove it.",
        "5V",
        showlegend=False,
    )
    add_wire(
        fig, [72, 111], [72, 31], COLORS["gnd"], "GND to L298N #1",
        "<b>Wire: external board GND -> L298N #1 GND</b><br>Same ground also goes to Mega.",
        "GND",
        showlegend=False,
    )
    add_wire(
        fig, [72, 111], [70, 9], COLORS["gnd"], "GND to L298N #2",
        "<b>Wire: external board GND -> L298N #2 GND</b>",
        "GND",
        showlegend=False,
    )
    add_wire(
        fig, [72, 111], [74, 33], COLORS["5v"], "+5V logic to L298N #1",
        "<b>Wire: external board +5V -> L298N #1 logic 5V pin</b><br>Do this if you want one consistent logic supply scheme for both L298N boards.",
        "5V",
        dash="dot",
        showlegend=False,
    )

    add_wire(
        fig, [103, 111], [73, 39], COLORS["ctrl"], "L298N control",
        "<b>Wire bundle: Mega D5/D24/D25 -> L298N #1 ENA/IN1/IN2</b><br>D5 gives PWM. D24/D25 set direction.",
        "CTRL",
    )
    add_wire(
        fig, [103, 111], [68, 15], COLORS["ctrl"], "L298N control",
        "<b>Wire bundle: Mega D6/D26/D27 -> L298N #2 ENA/IN1/IN2</b><br>D6 gives PWM. D26/D27 set direction.",
        "CTRL",
        showlegend=False,
    )

    add_wire(
        fig, [138, 145], [36, 36], COLORS["ctrl"], "Motor output",
        "<b>Wire: L298N #1 OUT1/OUT2 -> 4.5-15V motor</b><br>This branch supports on/off, PWM, and reverse.",
        "MOTOR",
    )
    add_wire(
        fig, [138, 145], [12, 12], COLORS["ctrl"], "Motor output",
        "<b>Wire: L298N #2 OUT1/OUT2 -> 5V motor</b><br>This branch supports on/off, PWM, and reverse, but with L298N voltage loss.",
        "MOTOR",
        showlegend=False,
    )

    add_note(
        fig,
        4,
        38,
        "<b>Assumption for the top-right devices:</b><br>"
        "They are 3-wire hobby servos.<br>"
        "If they are actually 2-wire DC motors,<br>"
        "this branch must be redesigned."
    )
    add_note(
        fig,
        40,
        35,
        "<b>External board minimum BOM:</b><br>"
        "1. Molex input connector or cut Molex extension<br>"
        "2. Toggle switch for PS_ON<br>"
        "3. 7809 regulator + 0.33uF/0.1uF + 10uF caps<br>"
        "4. 5V rail capacitor near servos<br>"
        "5. Fuses on 12V and 5V branches<br>"
        "6. Screw terminals / headers / star ground copper"
    )
    add_note(
        fig,
        108,
        95,
        "<b>Why the Motor Shield is greyed out:</b><br>"
        "It becomes unnecessary in the recommended build.<br>"
        "Two servos want signal control, not H-bridge power control.<br>"
        "Two DC motors with different supply voltages are cleaner with 2 separate L298N boards."
    )
    add_note(
        fig,
        109,
        49,
        "<b>L298N caveat:</b><br>"
        "A 5V DC motor on L298N may receive less than 5V at the motor<br>"
        "because the driver drops voltage internally.<br>"
        "This is functional, but weaker than an ideal MOSFET driver."
    )
    add_note(
        fig,
        142,
        91,
        "<b>Control semantics:</b><br>"
        "Servo: signal pulse controls angle or continuous-rotation speed/direction.<br>"
        "DC motor via L298N: ENA=PWM speed, IN1/IN2=reverse."
    )

    fig.add_annotation(
        x=82,
        y=97,
        text="Use mouse wheel to zoom, drag to pan, hover on wires and boxes for pin-by-pin notes.",
        showarrow=False,
        font=dict(size=12, color=COLORS["note"]),
    )

    return fig


def main():
    fig = build_figure()
    html_path = OUT_DIR / "interactive_full_circuit.html"
    fig.write_html(html_path, include_plotlyjs=True, full_html=True)
    print(f"Wrote {html_path}")


if __name__ == "__main__":
    main()
