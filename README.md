# stugna-it

Група в Telegram https://t.me/stugna_it

Проста у використанні програма для генерування запитів до сайтів.
Цілі та проксі завантажуються за посиланнями ITARMY,
та періодично, 1 раз на годину оновлюються.
Програма написана на C++ з використанням бібліотеки CURL.

### Скачати програму можна в розділі Releases - https://github.com/Stugna-It/stugna-it/releases/
Потрібно розпакувати будь-куди - запустити start.cmd ,
для перегляду статистики перейти в браузері за посиланням http://127.0.0.1:8088/

### Покрокова інструкція по запуску для Windows  https://github.com/Stugna-It/stugna-it/tree/master/doc

### Запуск через docker - docker run -p 8088:8088 stugnait/stugna-it

![Alt text](/../master/stugna-screen.png?raw=true "screenshot")

# Конфігурація
знаходиться в файлі config.json в папці програми, з коментарями.
Якщо Ви запускаєте програму на VPS, або на чомусь "потужному", 
можна підіймати параметри threads та batch_requests
до 16/4000 і вище, в залежності від потужності машини.


# Запуск Ubuntu/Debian

копіпастимо в термінал слідуючі команді - 
```
mkdir stugna-it && cd stugna-it
curl https://raw.githubusercontent.com/Stugna-It/stugna-it/master/builds/stugna-it-debian -o stugna-it
curl https://raw.githubusercontent.com/Stugna-It/stugna-it/master/src/index.html -O
curl https://raw.githubusercontent.com/Stugna-It/stugna-it/master/src/config.docker.json -o config.json

chmod +x stugna-it
./stugna-it
```

# Компіляція для Linux:

потрібні пакети - libssl-dev libasio-dev libcurl4-openssl-dev cmake make g++ gcc build-essential screen nginx git

debian/ubuntu - apt-get install libssl-dev libasio-dev libcurl4-openssl-dev cmake make g++ gcc build-essential screen nginx git
```
git clone https://github.com/Stugna-It/stugna-it.git
cd stugna-it/src
mkdir build
cd build
cmake .. && make

./stugna-it
```


# "Запроксювати" статистику "назовні":

Можна змінивши в config.json параметр "listen_on":"0.0.0.0",  або на зовнішню IP

Або за допомогою nginx, приклад конфігу - 

```
server {
      listen       8008;
      server_name  _;
      location / {
        proxy_pass          http://127.0.0.1:8088;
      }
}
```


