# Lods Patcher

## English description
The program is a patcher of a partition - to exclude a duplicate one (if the location was decompiled with a bardak converter, for example).
The program analyzes the input file with sections, looking at the sections of the Lods and looking for replacements according to the standard catalogs of the X-Ray SDK 0.7.

### Use:
* Create any * .bat file, and write the startup command in it.
* Run * .bat file and wait for the operation to finish

### How it works?
If the LOD file is most suitable for replacement with the current file from the section, then the path from the section is replaced with the path to the SDK directory.
Ways, copies of which were not found, remain unchanged.

### Description of commands:
* -editor "path_to_ed_editors (sdk)" - path to the SDK directory
* -d - key that will copy files to the output folder (debug mode, manual file comparison)
* -out "path_to_the output_folder (for debugging, copying LODs)" - used for debugging, is a way to copy replaced LOD files
(used only with the -d key)
* -l location_name (no spaces!) - postfix location, it will be built on the name (s) of LOD - sections

### Directory structure:
If there are no directories, then you need to create next to the program.
* levels\in\ - the scene_object.part file is stored here, in which sections are subject to change
* debug_info\edit_lods\ - new (copied) lods with their changed name will be stored here (for debugging)
* levels\out\ - the scene_object.part file will be stored here, which will contain the modified paths to the LOD

### Logs:
* replace_lods.log - main log, contains replaced and missing sections
* copy_lods_debug.log - for -d mode, contains copied / skipped lody when copying

### Example:
```
-editor "E:\X-Ray CoP SDK\editors" -out "D:\newProjects\xrai_utilites\lods_patcher\lods_patcher\debug_info\edit_lods" -l swamp
```

## Русское описание
Программа - патчер лод - секций, для исключения дублирующий лодов(в случае, если лоация была декомпилирована конвертером bardak'a к примеру).
Программа анализирует входной файл с секциями, просматривая секции лодов и ища замены по стандартным каталогам X-Ray SDK 0.7.

### Использование:
* Создайте любой *.bat - файл, и в него запишите команду запуска.
* Запустите *.bat - файл и дождитесь окончания операции

### Как это работает?
Если файл лода максимально подходит для замены с текущим файлом из секции, то путь из секции заменяется на путь до каталога SDK.
Пути, копий которых не нашлось, остаются без изменений.

### Описание команд:
* -editor "путь_к_editors(sdk)" - путь до каталога SDK
* -d - ключ, который будет копировать файлы в выходную папку(режим отладки, ручное сравнение файлов)
* -out "путь_к_выходной_папке(для дебага, копирование лодов)" - используется для отладки, является путем для копирования заменённых файлов лодов
(используется только с ключом -d)
* -l имя_локации(без пробелов!) - постфикс локации, по нему будет строится имя(имена) лод - секций

### Структура каталогов:
Если каталогов нет, то требуется создать рядом с программой.
* levels\in\ - здесь храниться файл scene_object.part, в котором лежат секции, подверженные изменению
* debug_info\edit_lods\ - здесь будут храниться новые(копированные) лоды с их изменённым именем(для отладки)
* levels\out\ - здесь будет храниться файл scene_object.part, который будет содержать изменённые пути к лодам

### Логи:
* replace_lods.log - главный лог, содержит замененные и пропущенные секции
* copy_lods_debug.log - для режима -d, содержит скопированные/пропущенные лоды при копировании

### Пример:
```
-editor "E:\X-Ray CoP SDK\editors" -out "D:\newProjects\xrai_utilites\lods_patcher\lods_patcher\debug_info\edit_lods" -l swamp
```
