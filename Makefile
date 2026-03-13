# SPDX-License-Identifier: CC0-1.0
#
# SPDX-FileContributor: Antonio Niño Díaz, 2025

BLOCKSDS	?= /opt/blocksds/core

# User config

NAME		:= racer_dsi
GAME_TITLE	:= Racer
GAME_SUBTITLE	:= BlockDS DSi port

# Build only the ARM9 port entrypoint. This avoids pulling legacy desktop C++.
SOURCEDIRS	:= source/main.c
NITROFSDIR	:= Media

include $(BLOCKSDS)/sys/default_makefiles/rom_arm9/Makefile
