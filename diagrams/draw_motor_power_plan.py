from pathlib import Path
from textwrap import fill

import matplotlib.pyplot as plt
from matplotlib.lines import Line2D
from matplotlib.patches import FancyBboxPatch


OUT_DIR = Path(__file__).resolve().parent


COLORS = {
    "12v": "#d4a017",
    "5v": "#d04a3a",
    "gnd": "#222222",
    "ctrl": "#2c6ed5",
    "warn": "#f08c2e",
    "ps_on": "#2d8f4e",
    "box": "#f7f7f7",
    "soft": "#eef4ff",
    "note": "#fff7db",
}


def draw_box(ax, x, y, w, h, title, lines, facecolor=None):
    patch = FancyBboxPatch(
        (x, y),
        w,
        h,
        boxstyle="round,pad=0.02,rounding_size=1.8",
        linewidth=1.6,
        edgecolor="#2a2a2a",
        facecolor=facecolor or COLORS["box"],
    )
    ax.add_patch(patch)
    ax.text(
        x + w / 2,
        y + h - 1.8,
        title,
        ha="center",
        va="top",
        fontsize=11,
        fontweight="bold",
    )

    line_y = y + h - 5.0
    for line in lines:
        wrapped = fill(line, width=max(20, int(w * 2.8)))
        ax.text(x + 1.1, line_y, wrapped, ha="left", va="top", fontsize=8.8)
        line_y -= 2.9


def draw_wire(ax, points, color, label=None, lw=3.0, ls="-"):
    xs = [p[0] for p in points]
    ys = [p[1] for p in points]
    ax.plot(xs, ys, color=color, linewidth=lw, linestyle=ls, solid_capstyle="round")
    if label:
        mid = len(points) // 2
        ax.text(
            points[mid][0] + 0.3,
            points[mid][1] + 0.8,
            label,
            color=color,
            fontsize=8.5,
            fontweight="bold",
        )


def add_legend(ax):
    handles = [
        Line2D([0], [0], color=COLORS["12v"], lw=3, label="+12V"),
        Line2D([0], [0], color=COLORS["5v"], lw=3, label="+5V"),
        Line2D([0], [0], color=COLORS["gnd"], lw=3, label="GND"),
        Line2D([0], [0], color=COLORS["warn"], lw=3, label="VMOT from converter (7.5-8.5V)"),
        Line2D([0], [0], color=COLORS["ctrl"], lw=3, label="Motor output / control path"),
        Line2D([0], [0], color=COLORS["ps_on"], lw=3, label="PS_ON switch"),
    ]
    ax.legend(
        handles=handles,
        loc="lower left",
        bbox_to_anchor=(0.01, 0.01),
        frameon=False,
        ncol=3,
        fontsize=8.5,
    )


def main():
    fig, ax = plt.subplots(figsize=(16, 9))
    ax.set_xlim(0, 100)
    ax.set_ylim(0, 100)
    ax.axis("off")

    ax.text(
        50,
        97,
        "Semester Project Power + Motor Wiring Plan",
        ha="center",
        va="top",
        fontsize=18,
        fontweight="bold",
    )
    ax.text(
        50,
        93.5,
        "Visual soldering guide for Corsair GS700 + Arduino Mega 2560 + Motor Shield Rev3",
        ha="center",
        va="top",
        fontsize=10.5,
    )

    draw_box(
        ax,
        3,
        52,
        18,
        30,
        "Corsair GS700 PSU",
        [
            "Molex / peripheral cable -> ext board",
            "yellow = +12V",
            "red = +5V",
            "black = GND",
            "green = PS_ON",
        ],
        facecolor=COLORS["soft"],
    )

    draw_box(
        ax,
        7,
        33,
        10,
        10,
        "PSU ON",
        [
            "Toggle switch",
            "green -> switch -> black",
        ],
        facecolor="#edf8ef",
    )

    draw_box(
        ax,
        27,
        42,
        29,
        40,
        "External Soldered Board",
        [
            "Input: 1x Molex + 1x green PS_ON wire",
            "F1: +12V fuse",
            "F2: +5V fuse",
            "STAR GND point",
            "DC/DC slot: 12V -> VMOT 7.5V .. 8.5V",
            "Optional 7809 -> Mega barrel/VIN",
            "Raw outputs: +12V/GND and +5V/GND",
        ],
        facecolor=COLORS["note"],
    )

    draw_box(
        ax,
        61,
        42,
        22,
        40,
        "Mega 2560 + Motor Shield Rev3",
        [
            "2 independent H-bridge channels only",
            "CH A: DIR D12, PWM D3, BRK D9, SNS A0",
            "CH B: DIR D13, PWM D11, BRK D8, SNS A1",
            "<= 2A continuous per channel",
            "Cut VIN-CONNECT if power is separate",
        ],
        facecolor=COLORS["soft"],
    )

    draw_box(
        ax,
        87,
        73,
        11,
        10,
        "Load A",
        [
            "Full control:",
            "ON/OFF",
            "PWM",
            "Reverse",
        ],
        facecolor="#eef8ff",
    )
    draw_box(
        ax,
        87,
        58,
        11,
        10,
        "Load B",
        [
            "Full control:",
            "ON/OFF",
            "PWM",
            "Reverse",
        ],
        facecolor="#eef8ff",
    )
    draw_box(
        ax,
        87,
        43,
        11,
        10,
        "Raw +12V",
        [
            "Direct supply",
            "or future",
            "1-direction",
            "driver",
        ],
        facecolor="#fff4e8",
    )
    draw_box(
        ax,
        87,
        28,
        11,
        10,
        "Raw +5V",
        [
            "Direct supply",
            "or future",
            "1-direction",
            "driver",
        ],
        facecolor="#fff4e8",
    )

    draw_box(
        ax,
        3,
        7,
        54,
        18,
        "Suggested Assignments For Tomorrow",
        [
            "A: CH A = 4.8-7.2V motor #1; CH B = 4.8-7.2V motor #2; Raw +5V = 5V motor; Raw +12V = 4.5-15V motor.",
            "B: CH A = both 4.8-7.2V motors in parallel only if Istall total < 2A; CH B = 4.5-15V motor; Raw +5V = 5V motor.",
            "For 4 independent motors with reverse, you need 4 H-bridges. The shield gives only 2.",
        ],
        facecolor="#f8f8f8",
    )

    draw_box(
        ax,
        60,
        7,
        38,
        18,
        "Hard Limits / Safety Notes",
        [
            "Do not feed 5V or 6V motors from raw ATX 12V through the shield.",
            "L298P drops voltage, so low-voltage motors usually want VMOT around 7.5V .. 8.5V.",
            "Keep motor current out of Arduino pins and out of the Mega 5V regulator.",
            "Measure stall current before final soldering if you can.",
        ],
        facecolor=COLORS["note"],
    )

    draw_wire(ax, [(21, 67), (27, 67)], COLORS["12v"])
    draw_wire(ax, [(21, 61), (27, 61)], COLORS["5v"])
    draw_wire(ax, [(21, 55), (27, 55)], COLORS["gnd"])
    draw_wire(ax, [(21, 52), (27, 52)], COLORS["gnd"])
    draw_wire(ax, [(12, 52), (12, 43)], COLORS["ps_on"], label="green PS_ON")
    draw_wire(ax, [(12, 43), (12, 38)], COLORS["ps_on"])
    draw_wire(ax, [(12, 38), (17, 38)], COLORS["gnd"], label="switch to GND")

    draw_wire(ax, [(56, 70), (61, 70), (61, 68)], COLORS["warn"])
    draw_wire(ax, [(56, 55), (61, 55)], COLORS["gnd"])
    draw_wire(ax, [(56, 50), (61, 50)], "#7f8c8d")

    draw_wire(ax, [(83, 76), (87, 76)], COLORS["ctrl"])
    draw_wire(ax, [(83, 61), (87, 61)], COLORS["ctrl"])
    draw_wire(ax, [(56, 46), (87, 46)], COLORS["12v"])
    draw_wire(ax, [(56, 31), (87, 31)], COLORS["5v"])
    draw_wire(ax, [(56, 43), (87, 43)], COLORS["gnd"], ls="--")
    draw_wire(ax, [(56, 28), (87, 28)], COLORS["gnd"], ls="--")

    ax.text(
        62,
        36,
        "Tomorrow's easiest logic power:\nUSB -> Mega",
        ha="left",
        va="center",
        fontsize=9,
        bbox={"boxstyle": "round,pad=0.3", "fc": "#ffffff", "ec": "#b0b0b0"},
    )

    ax.text(
        83,
        88,
        "Choose which motors\nmust have reverse.",
        ha="center",
        va="center",
        fontsize=9,
        bbox={"boxstyle": "round,pad=0.3", "fc": "#ffffff", "ec": "#b0b0b0"},
    )

    add_legend(ax)
    fig.tight_layout()

    png_path = OUT_DIR / "semester_project_wiring_plan.png"
    svg_path = OUT_DIR / "semester_project_wiring_plan.svg"
    fig.savefig(png_path, dpi=220, bbox_inches="tight")
    fig.savefig(svg_path, bbox_inches="tight")
    plt.close(fig)

    print(f"Wrote {png_path}")
    print(f"Wrote {svg_path}")


if __name__ == "__main__":
    main()
