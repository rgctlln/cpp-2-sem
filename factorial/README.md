# Домашняя работа №1. Факториал

Текущий статус тестирования GitHub Actions: [![CI/CD](../../actions/workflows/classroom.yml/badge.svg?branch=main&event=workflow_dispatch)](../../actions/workflows/classroom.yml).

> [!Note]
> Чтобы GitHub Workflow отработал верно, файл с [функцией `main`](https://en.cppreference.com/w/c/language/main_function) должен называться `main.c`.
>
> [**Не забудьте инициализировать репозиторий!**](#инициализация-репозитория)

## Инициализация репозитория

После *первого клонирования* репозитория его необходимо инициализировать. Для этого Вам необходимо запустить соответствующий скрипт, для запуска требуется [Bash](https://en.wikipedia.org/wiki/Bash_(Unix_shell)) (есть в UNIX-подобных системах, в Windows – подойдёт [Git Bash](https://git-scm.com/)) и [GitHub CLI](https://cli.github.com/):

```bash
bash init-repo.sh
```

Скрипт необходимо запустить **только один раз**.

## Локальное тестирование

Вам предоставляется возможность запуска тестов локальным способом. Для этого нужно:

1. Установить [Python](https://www.python.org/).
2. Установить дополнительные библиотеки через [`pip`](https://pypi.org/project/pip/): `difflib`.
3. В корне репозитория запустить `python tests.py <path to executable filename>`.
4. Посмотреть логи тестирования.
