# stugna-it

## Інструкція по запуску для Windows  https://github.com/Stugna-It/stugna-it/tree/master/doc

Група в Telegram https://t.me/stugna_it

Проста у використанні програма для генерування запитів до сайтів.
Цілі та проксі завантажуються за посиланнями ITARMY, 
та періодично, 1 раз на годину оновлюються.

Скачати програму можна в розділі Releases - https://github.com/Stugna-It/stugna-it/releases/


Потрібно розпакувати будь-куди - запустити start.cmd ,
для перегляду статистики перейти в браузері за посиланням http://127.0.0.1:8088/

Програма написана на C++ з використанням бібліотеки CURL.


![Alt text](/../master/stugna-screen.png?raw=true "screenshot")

# Конфігурація
знаходиться в файлі config.json в папці програми, з коментарями.
Якщо Ви запускаєте програму на VPS, або на чомусь "потужному", 
можна підіймати параметри threads та batch_requests
до 16/4000 і вище, в залежності від потужності машини.


# Компіляція для Linux:

потрібні пакети - libssl-dev libasio-dev libcurl4-openssl-dev cmake make g++ gcc build-essential screen nginx git

debian/ubuntu - apt-get install libssl-dev libasio-dev libcurl4-openssl-dev cmake make g++ gcc build-essential screen nginx git

git clone https://github.com/Stugna-It/stugna-it.git

cd stugna-it/src

mkdir build

cd build

cmake .. && make
#
cp ../index.html index.html ; cp ../config.json config.json

./stugna-it

# "Запроксювати" статистику "назовні":

Можна за допомогою nginx, приклад конфігу - 

```
server {
      listen       8008;
      server_name  _;
      location / {
        proxy_pass          http://127.0.0.1:8088;
      }
}
```


