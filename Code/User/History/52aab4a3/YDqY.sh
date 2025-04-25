#!/bin/bash

words_file="words.txt"    # Файл с словами (каждое на новой строке)
chars_file="chars.txt"    # Файл с символами (каждый на новой строке)
output="wordlist.txt"     # Результирующий файл

# Проверяем, существуют ли файлы
[ ! -f "$words_file" ] && { echo "Ошибка: $words_file не найден"; exit 1; }
[ ! -f "$chars_file" ] && { echo "Ошибка: $chars_file не найден"; exit 1; }

# Читаем слова и символы, убираем дубликаты
words=$(sort -u "$words_file")
chars=$(sort -u "$chars_file")

# Временный файл для новых комбинаций
temp_file=$(mktemp)

# Генерация всех комбинаций
for word in $words; do
    # Вариант 1: Само слово (без символов)
    echo "$word" >> "$temp_file"

    # Вариант 2: Символы только слева
    for char in $chars; do
        echo "${char}${word}" >> "$temp_file"
    done

    # Вариант 3: Символы только справа
    for char in $chars; do
        echo "${word}${char}" >> "$temp_file"
    done

    # Вариант 4: Символы и слева, и справа
    for char1 in $chars; do
        for char2 in $chars; do
            echo "${char1}${word}${char2}" >> "$temp_file"
        done
    done
done

# Фильтруем только новые уникальные строки и добавляем их в output
if [ -f "$output" ]; then
    grep -Fxvf "$output" "$temp_file" >> "$output"
else
    cat "$temp_file" > "$output"
fi

# Удаляем временный файл
rm "$temp_file"

echo "Готово! Новые комбинации добавлены в $output"