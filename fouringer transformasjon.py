import pygame as py
from pygame import gfxdraw
import numpy as np
from time import sleep
from numpy import sqrt, log, e
import inspect

py.init()

py.font.init() # you have to call this at the start, 
                   # if you want to use this module.
myfont = py.font.SysFont('Comic Sans MS', 30)


# 1920x1080 px
screen = py.display.set_mode((0, 0), py.FULLSCREEN)

# Colors
black = (0, 0, 0)
white = np.array((255, 255, 255))
base_1 = np.array((1, 0, 1))
base_2 = np.array((0,1,1))
base = base_2

# Function
c = -0.420 + 0.69j
f = lambda z: e**(-z**3.1415926535) + c
g = lambda f_n, c_0: f_n**8 + c_0


"""
Great values:
c = -0.7j
c = -0.909 -0.275j
"""

# Step_size
step = 500


def draw_pixel(x, y, color):
    # Draws pixel on the given x-y-position
    gfxdraw.pixel(screen, 960 + x, 540 - y, color)


def julia(f, x_c, y_c, r, x_corr, y_corr):
    x = x_c / step
    y = y_c / step
    result = complex(x, y)
    c = 0
    for i in range(1, r + 1):
        try:
            result = f(result)
            if abs(result) > 300:
                c = 255 * i / r * 3
                break
        except OverflowError:
            c = 255
    draw_pixel(x_c - x_corr, y_c - y_corr, ((base * c).clip(0, 255)).astype(int))


def mandelbrot(f, x_c, y_c, r, x_corr, y_corr):
    x = x_c / step
    y = y_c / step
    c_0 = complex(x, y)
    c = 0
    result = f(0, c_0)
    for i in range(1, r + 1):
        try:
            result = g(result, c_0)
        except ZeroDivisionError:
            result += 0.0001
        if abs(result) > 300:
            c = 255 * i / r * 2
            break
    draw_pixel(x_c - x_corr, y_c - y_corr, ((base * c).clip(0, 255)).astype(int))
    
y_c = 100
x_c = -350
# Draws the set
# Julia Set
for x in range(-960 + x_c, 960 + x_c):
        for y in range(-540, 540 + y_c):
            #mandelbrot(g, x, y, 30, x_c, y_c)
            julia(f, x, y, 100, x_c, y_c)
""" content = inspect.getsource(f)
textsurface = myfont.render(content[content.index(":") + 2:-1].replace("**", "^") + ", c = " + str(c).replace("j", "i").replace("(", "").replace(")", ""), True, white) """
content = inspect.getsource(g)
textsurface = myfont.render(content[content.index(":") + 2:-1].replace("**", "^").replace("f_n", "z").replace("c_0", "c"), True, white)
screen.blit(textsurface, (1570, 1000))
running = True
while running:
    for event in py.event.get():
        if event.type == py.KEYDOWN:
            if event.key == py.K_ESCAPE:
                running = False
    
    py.display.update()