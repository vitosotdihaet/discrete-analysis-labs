# Лабы по дискретному анализу

## Требования к работам:
- соблюдение [кодстайла](https://google.github.io/styleguide/cppguide.html)
- (если пишется на чистом Си, то юзать snake_case вместо CamelCase и замечания ниже имеют приоритет)
- не сокращать названия переменных\функций\стрктур\классов и т. п.
- плохо
    ```c
    int l = strlen(super_string);

    int a = l - 1;

    for (int i=0; i < l;i++) {...}

    int getres(int a, double c);
    ```
- хорошо
    ```c
    int super_string_length = strlen(super_string);

    int last_index_super_string = super_string_length - 1;

    for (int super_string_index=0; super_string_index < super_string_length;++super_string_index) {...}

    int calculate_result(int last_index_super_string, double coefficient);
    ```
- должны быть тест-кейсы (либо автоматические (проверяемые например bash скриптом или google-test'ами), либо ручные)
- применять декомпозицию - функций более 80-100 строк в коде не должно быть.
- define макросы заменять глобальными константами.
- в C++ не использовать STL кроме `*stream` и всего что есть в стандартных библиотеках Си.

## Источники:
Рекомендация литературы и других источников инфы:
- "Алгоритмы: построение и анализ" Томас Кормен, Рональд Линн Ривест, Чарльз Эрик Лейзерсон, Клиффорд Штайн. БАЗА по алгоритмам и структурам данных. Читать и вникать ВСЕМ. Супер мастхев.
- "Строки деревья и последовательности в алгоритмах" Дэн Гасфилд. 
Тут Вы найдете то, что нету в труде выше. Более узкое направление изучения. Кстати в курсе есть материал, который только тут. 
- C++. Есть хороший курс от Я/Физтех/ШАД "Основы разработки на C++: белый пояс". О нем есть [статья на Хабре](https://habr.com/ru/company/yandex/blog/332556/). Из [почитать лучше](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines). Так же [кодстайл гугла](https://google.github.io/styleguide/cppguide.html). Вашу ИДЕ по возможности настройте по нему, уверен есть готовые плагины для этого. 
- "Объектно-ориентированный анализ и проектирование с примерами приложений" Гради Буч, Роберт Максимчук, Майкл Энгл, Бобби Янг, Джим Коналлен, Келли Хьюстон. 

БАЗА по ООП. Если вы хотите в написание кода на C++, то вам мастхев понимать эту парадигму. Третья часть этой книги необязательная, но вот первые две нужно ОСОЗНАТЬ.