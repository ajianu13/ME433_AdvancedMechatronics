import pygame
import math
import numpy as np

# basic setup
pygame.init()
W, H = 900, 500
screen = pygame.display.set_mode((W, H))
clock = pygame.time.Clock()

# knob center
CX, CY = 250, 250
KNOB_R = 120

# plot area
PX, PY = 480, 60
PW, PH = 360, 360

# modes
MODES = ["spring", "bump", "detent", "toggle", "dip"]
mode_index = 0

# spring force
def spring_force(t):
    # soft pull to center
    return -t / 90

# bump force
def bump_force(t):
    # soft hill in center
    if abs(t) < 25:
        return -math.sin(math.pi * t / 25)
    return 0

# detent force
def detent_force(t):
    # soft flat center with sharp sides
    return math.tanh(t * 0.1)

# toggle force
def toggle_force(t):
    # two stable sides and an unstable center
    return math.tanh((t - 20) * 0.15) - math.tanh((t + 20) * 0.15)

# dip force
def dip_force(t):
    # soft valley in center
    if abs(t) < 30:
        return math.sin(math.pi * t / 30)
    return 0

# pick force
def compute_force(t, mode):
    if mode == "spring":
        return max(-1, min(1, spring_force(t)))
    if mode == "bump":
        return max(-1, min(1, bump_force(t)))
    if mode == "detent":
        return max(-1, min(1, detent_force(t)))
    if mode == "toggle":
        return max(-1, min(1, toggle_force(t)))
    return max(-1, min(1, dip_force(t)))

# knob drawing
def draw_knob(theta, force):
    # knob body
    pygame.draw.circle(screen, (60, 60, 80), (CX, CY), KNOB_R)
    pygame.draw.circle(screen, (90, 90, 120), (CX, CY), KNOB_R - 20)

    # knob pointer
    ang = math.radians(theta - 90)
    px = CX + (KNOB_R - 30) * math.cos(ang)
    py = CY + (KNOB_R - 30) * math.sin(ang)
    pygame.draw.line(screen, (240, 240, 255), (CX, CY), (px, py), 4)

    # force arrow
    if abs(force) > 0.02:
        arrow_len = abs(force) * 70
        arrow_ang = theta + ( -90 if force > 0 else 90 )
        rad = math.radians(arrow_ang)
        ax = CX + arrow_len * math.cos(rad)
        ay = CY + arrow_len * math.sin(rad)
        pygame.draw.line(screen, (200, 180, 80), (CX, CY), (ax, ay), 5)

def draw_force_direction(force):
    # center of the arrow
    y = CY + KNOB_R + 60
    x1 = CX - 60
    x2 = CX + 60

    # no arrow if force is tiny
    if abs(force) < 0.02:
        return

    if force > 0:
        # arrow pointing right
        pygame.draw.line(screen, (255, 200, 120), (x1, y), (x2, y), 6)
        pygame.draw.polygon(screen, (255, 200, 120), [
            (x2, y),
            (x2 - 15, y - 10),
            (x2 - 15, y + 10)
        ])
    else:
        # arrow pointing left
        pygame.draw.line(screen, (255, 200, 120), (x2, y), (x1, y), 6)
        pygame.draw.polygon(screen, (255, 200, 120), [
            (x1, y),
            (x1 + 15, y - 10),
            (x1 + 15, y + 10)
        ])


# plot helpers
def plot_x(t):
    return int(PX + (t + 90) / 180 * PW)

def plot_y(f):
    return int(PY + PH / 2 - f * (PH / 2 - 10))

# plot drawing
def draw_plot(theta, force, mode):
    # plot box
    pygame.draw.rect(screen, (40, 40, 55), (PX, PY, PW, PH))

    # grid
    for f in [-1, -0.5, 0, 0.5, 1]:
        y = plot_y(f)
        pygame.draw.line(screen, (70, 70, 90), (PX, y), (PX + PW, y), 1)

    # curve
    ts = np.linspace(-90, 90, 300)
    pts = [(plot_x(t), plot_y(compute_force(t, mode))) for t in ts]
    pygame.draw.lines(screen, (120, 160, 240), False, pts, 2)

    # current point
    pygame.draw.circle(screen, (255, 220, 80), (plot_x(theta), plot_y(force)), 6)

    # label
    font = pygame.font.SysFont("monospace", 16)
    txt = font.render(f"{mode}", True, (230, 230, 255))
    screen.blit(txt, (PX + PW // 2 - txt.get_width() // 2, PY - 28))

# main loop
theta = 0
dragging = False

while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            raise SystemExit

        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_TAB:
                mode_index = (mode_index + 1) % len(MODES)
            if event.key == pygame.K_RIGHT:
                theta = min(90, theta + 5)
            if event.key == pygame.K_LEFT:
                theta = max(-90, theta - 5)

        if event.type == pygame.MOUSEBUTTONDOWN:
            mx, my = event.pos
            if math.hypot(mx - CX, my - CY) < KNOB_R:
                dragging = True

        if event.type == pygame.MOUSEBUTTONUP:
            dragging = False

    if dragging:
        mx, my = pygame.mouse.get_pos()
        ang = math.degrees(math.atan2(my - CY, mx - CX)) + 90
        ang = ((ang + 90) % 180) - 90
        theta = max(-90, min(90, ang))

    mode = MODES[mode_index]
    force = compute_force(theta, mode)

    screen.fill((20, 20, 30))
    draw_knob(theta, force)
    draw_plot(theta, force, mode)
    draw_force_direction(force)

    pygame.display.flip()
    clock.tick(60)
