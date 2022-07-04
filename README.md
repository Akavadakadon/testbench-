# testbench

Тестовый стенд для измерения скорости работы протоколов, когда клиент посылает запрос серверу, сервер отсылает клиенту файл размером в N байт. 

Этот стенд может быть применим для отслеживания изменений, когда стоит задача модифицировать протокол.

Сейчас в репозитории лежат исходники для работы с TCP и UDT, а файлы протокола QUIC пока что защищены корпоративной тайной и не опубликованы.

Каждое устройство для удобства тестирования и автоматической развёртывания помещается в Docker-контейнер. 
Это касается как приложений для клиента и сервера, так и симулятора сети. Docker создаёт из docker-образов новые контейнеры и развёртывает сервисы, а также сети и тома.

# QUIC контейнеры
Клиентское приложение для QUIC протокола было взято из проекта The Chromium Projects, подробное описание запустить клиент и сервер доступно по ссылке [23]. Клиент quic_client был доработан, чтобы собиралась необходимая информация о соединении и передаче. Сервер quic_server остался неизменённым. Протокол QUIC тестируется с включенным SSL.

# UDT контейнеры 
Клиент и сервер под UDT протокол были написаны с использованием библиотек, предоставленных самим создателем протокола. Исходный код протокола и примеров приложений доступны по ссылке[24]. Клиентское приложение, использованное собирает время выполнения каждой итерации, число повторных отправлении, число потерь и т.д. Сервер же только принимает файл и отравляет подтверждение. Протокол UDT не имеет защиты транспортного уровня, поэтому отличается от двух других протоколов отсутствием SSL. Файлы образов контейнеров показаны на рисунке .

# TCP контейнеры
Для тестирования производительности протокола TCP клиент и сервер не создавались под какую-либо принесённую библиотеку, а работали на протоколе, определённом в операционной системе.
Сервер nginx принимает соединение и по запросу отправляет ответ. Соединение закрывается после получения файла со стороны клиента. Чтобы настроить HTTPS-сервер, в контейнер передается файл конфигурации с необходимыми настройками для ssl. 

Протоколы QUIC и TCP тестировались с включенной защитой транспортного уровня -TLS. UDT - без.

# 3 Docker-контейнера: 
Клиент, сервер и симулятор сети совместно развертываются на хост системе.
Внутренние скрипты  настраивают маршрутизацию. 
 
# Скрипты отвечающие за маршрутеризацию:
1) Клиент ````ns3client_rquic.sh````
2) Сервер ``ns3server_rquic.sh`` 
3) Симулятор `сети ns3proxy.sh`

# runner.py 
Python скрипт для запуска контейнеров и тестирования
Пример вызова функции ``` python3 runner.py --delays=1ms,50ms --errors=0.01 --sizes=1024 --iterations=100 ```
Функция собирает контейнеры и совместно запускает клиент, сервер и симулятор сети на тестирование передачи файла в 1кб, по сети с потерями в 1 процент и задержкой для первого прогона в 1 миллисекунду, для второго в 50 миллисекунд. 
В эксперименте будет совершено 100 тестовых передач.

# Google Colab 
парсит текстовые файлы с отчетами. 
Пример такого блокнота - Главный отчет.ipynb.