# stugna-it

Проста у використанні програма для генерування запитів до сайтів)
Цілі та проксі завантажуються за посиланнями ITARMY, 
та періодично (1 раз на годину) оновлюються.

Скачати програму млжна в розділі Releases - https://github.com/tartminer/stugna-it/releases/
Збірка для Windows https://github.com/tartminer/stugna-it/releases/download/v0.1/stugna-it-win64.zip

Потрібно розпакувати будь-куди - запустити start.cmd ,
для перегляду статистики перейти в браузері за посиланням http://127.0.0.1:8088/

скрін роботи програми - https://github.com/tartminer/stugna-it/blob/master/stugna-screen.png

Програма написана на C++ з використанням бібліотеки CURL.

# Компіляція для Linux:

git clone https://github.com/tartminer/stugna-it.git

cd stugna-it

mkdir build

cd build

cmake .. && make






