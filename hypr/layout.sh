#!/bin/bash

# 1. Получаем ID клавиатуры (замените на свой!)
KEYBOARD_ID="at-translated-set-2-keyboard"  # Узнать: `hyprctl devices | grep -A 10 "Keyboard"`

# 2. Получаем текущую раскладку
CURRENT_LAYOUT=$(hyprctl devices | grep -A 10 "$KEYBOARD_ID" | grep "active keymap" | awk '{print $3}')

# 3. Переключаем раскладку
if [ "$CURRENT_LAYOUT" == "Russian" ] || [ "$CURRENT_LAYOUT" == "ru" ]; then
    hyprctl switchxkblayout "$KEYBOARD_ID" 0  # US
    notify-send "US Layout" -t 1000
else
    hyprctl switchxkblayout "$KEYBOARD_ID" 1  # RU
    notify-send "RU Layout" -t 1000
fi 
