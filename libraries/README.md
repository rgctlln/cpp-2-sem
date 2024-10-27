# Лабораторная работа №2. Использование библиотек

Текущий статус тестирования GitHub Actions: [![CI/CD](../../actions/workflows/ci.yaml/badge.svg?branch=main&event=workflow_dispatch)](../../actions/workflows/ci.yaml).

> [!Note]
> Чтобы GitHub Workflow отработал верно, файл с [функцией `main`](https://en.cppreference.com/w/c/language/main_function) должен называться `main.c`.

## Локальное тестирование

Вам предоставляется возможность запуска базовых тестов локальным способом. Для этого нужно:

1. Установить [Python](https://www.python.org/).
2. Убедиться, что у Вас установлены следующие библиотеки: `hashlib`, `difflib`, `pyperclip` (в ином случае, установить через [`pip`](https://pypi.org/project/pip/)).
3. Склонировать репозиторий рекурсивно `git clone <repo_url> --recursive`. В противном случае `submodule` тестов не склонируется и счастье не наступит.
4. В корне репозитория запустить `python tests.py <path/to/executable>`.
5. Посмотреть логи тестирования.

По умолчанию, запускаются все *категориальные* тесты. Если нужны конкретные категории, в аргументы запуска необходимо подать наименования категорий (см. ниже). Внимание: если тест требует две категории и был подан только один из них, тест *не будет запущен*, необходимо подать обе категории.

Список категорий:

* **`positive_test`** - корректный тест
* **`negative_test`** - некорректный тест
* **`positive_delta`** - значение $\delta$ больше нуля
* **`negative_delta`** - значение $\delta$ меньше нуля

Пример запуска:

```bash
python tests.py main.exe positive_test negative_delta
# запуск корректных тестов с выходным значением delta < 0
```

Если результат расходится с ожидаемым, то будет выведена ошибка в формате:

```
====> ERROR: assert at extracted positional value [X] => [actual = Y] vs [expected = Z].
```

где `X` – позиция несовпадающего значения, `Y` – полученное вашей программой значение, `Z` – ожидаемое.

`X` может принимать значения 0, 1 и 2:

* 0 – "delta"
* 1 – "sample rate"
* 2 – "delta time"

```
====> ERROR: assert at extracted positional value [0] => [actual = 356554] vs [expected = 356544].
```

## Примечение по автотестам на GH

### ASan

Новая опция `[experimental] use ASan sanitizer` позволяет при прогоне собрать код с использованием ASan ([AddressSanitizer](https://learn.microsoft.com/en-us/cpp/sanitizers/asan?view=msvc-170)). При включении этой опции шаг с сборкой+запуском на первом тесте из `positive_tests positive_delta` включается в общий workflow, но является неблокирующим в случае неуспешного завершения.

Рекомендуется использовать для отлавливания ошибок в коде.

### clang-format

Запускается на прогоне "windows-latest clang". Является блокирующим, если включена соответсвующая опция.
