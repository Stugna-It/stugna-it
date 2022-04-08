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


![Alt text](/../master/stugna-screen.png?raw=true "screenshot")

# Конфігурація
знаходиться в файлі config.json в папці програми, з коментарями
якщо Ви запускаєте програму на VPS, або на чомусь "потужному", 
можна підіймати параметри threads та batch_requests
до 16/4000 і вище, в залежності від потужності машини.


# Компіляція для Linux:

потрібні пакети - libcurl4-openssl-dev cmake make g++ gcc build-essential screen nginx

debian/ubuntu - apt-get install libcurl4-openssl-dev cmake make g++ gcc build-essential screen nginx

git clone https://github.com/tartminer/stugna-it.git

cd stugna-it

mkdir build

cd build

cmake .. && make
#
cp ../index.html index.html ; cp ../config.json config.json

./stugna-it



