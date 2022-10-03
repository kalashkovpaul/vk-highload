## Сервер для раздачи статической информации

Архитектура сервера: thread pool

Язык реализации: C++

###  Команды

Запуск сервера происходит на 8080 порту, запуск nginx на 8888 (чтобы изменить - см. makefile).

Запускать сервер из Docker реализовано только для ОС Linux Ubuntu, на других OC следует вручную сделать
```
make build
./server
```

##### Запуск сервера
```bash
make build-docker
make docker-run
```
Остановка сервера:
```bash
make docker-stop
```
Удаление контейнера(чтобы запустить новый):
```bash
make docker-rm
```
##### Запуск nginx
```bash
make build-docker-nginx
make docker-run-nginx
```
Остановка nginx(a):
```bash
make docker-stop-nginx
```
Удаление контейнера nginx(a)(чтобы запустить новый):
```bash
make docker-rm-nginx
```

### Тестирование
Было произведено тестирование сервера. Кейсы:
1. Пустой запрос
2. Запрос по HTTP.1.1 без двух символов перевода стркои
3. Запрос с заголовком "Server"
4. Запрос директории (отдача индексого файла)
5. Запрос директории (индексный файл внутри других директорий, не найден)
6. Запрос отсутствующего файла
7. Запрос с вложенными директориями
8. Запрос со "/" после файла
9. Запрос с параметрами
10. Запрос с пробелами
11. Запрос, сделанный в HEX коде
12. Запрос большого файла (страница Википедии)
13. Запрос файла из директории, куда нет доступа
14. Запрос файла с точками в названии
15. POST-запрос
16. HEAD-запрос
17. Запрос html файла
18. Запрос ccs файла
19. Запрос js файла
20. Запрос jpg файла
21. Запрос jpeg файла
22. Запрос png файла
23. Запрос gif файла
24. Запрос swf файла

Запуск тестирования - посредством команды
```
make func-test
```
Результаты тестирования:
```
test_directory_index (__main__.HttpServer)
directory index file exists ... ok
test_document_root_escaping (__main__.HttpServer)
document root escaping forbidden ... ok
test_empty_request (__main__.HttpServer)
Send empty line ... ok
test_file_in_nested_folders (__main__.HttpServer)
file located in nested folders ... ok
test_file_not_found (__main__.HttpServer)
absent file returns 404 ... ok
test_file_type_css (__main__.HttpServer)
Content-Type for .css ... ok
test_file_type_gif (__main__.HttpServer)
Content-Type for .gif ... ok
test_file_type_html (__main__.HttpServer)
Content-Type for .html ... ok
test_file_type_jpeg (__main__.HttpServer)
Content-Type for .jpeg ... ok
test_file_type_jpg (__main__.HttpServer)
Content-Type for .jpg ... ok
test_file_type_js (__main__.HttpServer)
Content-Type for .js ... ok
test_file_type_png (__main__.HttpServer)
Content-Type for .png ... ok
test_file_type_swf (__main__.HttpServer)
Content-Type for .swf ... ok
test_file_urlencoded (__main__.HttpServer)
urlencoded filename ... ok
test_file_with_dot_in_name (__main__.HttpServer)
file with two dots in name ... ok
test_file_with_query_string (__main__.HttpServer)
query string with get params ... ok
test_file_with_slash_after_filename (__main__.HttpServer)
slash after filename ... ok
test_file_with_spaces (__main__.HttpServer)
filename with spaces ... ok
test_head_method (__main__.HttpServer)
head method support ... ok
test_index_not_found (__main__.HttpServer)
directory index file absent ... ok
test_large_file (__main__.HttpServer)
large file downloaded correctly ... ok
test_post_method (__main__.HttpServer)
post method forbidden ... ok
test_request_without_two_newlines (__main__.HttpServer)
Send GET without two newlines ... ok
test_server_header (__main__.HttpServer)
Server header exists ... ok

----------------------------------------------------------------------
Ran 24 tests in 1.037s

OK
```

### Запуск нагрузочного тестирования
1. **nginx сервер**
```bash
make bench-nginx
```
Результат:
```bash
p.kalashkov in ~/Desktop/fifthTerm/vk-highload/server on branch master > make bench-nginx
wrk -t12 -c400 -d30s 'http://127.0.0.1:8888/httptest/splash.css'
Running 30s test @ http://127.0.0.1:8888/httptest/splash.css
  12 threads and 400 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   408.38ms   74.92ms 748.99ms   68.44%
    Req/Sec    80.56     27.71   212.00     64.48%
  28890 requests in 30.04s, 2.68GB read
  Socket errors: connect 0, read 346, write 0, timeout 0
Requests/sec:    961.77
Transfer/sec:     91.20MB
```
2. **static-web-server**
```bash
make bench-static-web-server
```
Результат:
```bash
p.kalashkov in ~/Desktop/fifthTerm/vk-highload/server on branch master > make bench-static-web-server
wrk -t12 -c400 -d30s 'http://127.0.0.1:8080/httptest/splash.css'
Running 30s test @ http://127.0.0.1:8080/httptest/splash.css
  12 threads and 400 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    95.12ms  116.34ms 998.87ms   94.15%
    Req/Sec   313.09    176.16   640.00     59.26%
  16098 requests in 30.10s, 1.48GB read
  Socket errors: connect 0, read 1333, write 68, timeout 0
Requests/sec:    534.79
Transfer/sec:     50.37MB
```
