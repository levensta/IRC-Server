<h1>IRC-server</h1>

> В далеком 1998 году, в сети DALnet, на одном популярном тогда русскоязычном канале, на вопрос новичка «А что такое IRC?», один человек ответил именно так:
*«IRC — это мания, это страх, это тоска, это желание, это первая программа, запускаемая на компьютере. IRC — это бессонные ночи, это несделанная работа, это отсутствие всякого образования, кроме умения стучать по клавишам!»‎*

IRC (Internet Relay Chat) – устаревший протокол прикладного уровня для обмена сообщениями в режиме реального времени. Разработан в 1988 году и предназначен для группового общения, также позволяет общаться через личные сообщения и обмениваться файлами. Несмотря на устарелость, на основе IRC в дальнейшем разработано множество мессенджеров, такие как: ICQ, Skype, Discord, Telegram, Slack, etc...

Проект представляет собой IRC-сервер, написанный на основе стандарта [rfc1459](https://datatracker.ietf.org/doc/html/rfc1459).

<h2>Начало</h2>

Установка:
```
git clone https://github.com/levensta/IRC-Server.git
cd IRC-Server/
make
```

Запуск сервера:
```
./ircserv 6667 secretpassword
```

Запуск клиента:
- ```nc localhost 6667```
- Графический клиент Pidgin (Linux)
- Графический клиент Adium (MacOS)

При необходимости можно менять конфиг сервера **IRConf.json**

---
К серверу прилагается бот, который, используя [api.intra.42.fr](https://api.intra.42.fr/), возвращает местоположение человека с отправленным никнеймом. <br>
Запуск бота:
```
./bot/bot IRCatBot.conf.json
```
Пример команды для бота:
```
PRIVMSG wherebot :levensta
// :wherebot!where_is_bot@127.0.0.1 PRIVMSG lev :"Location: Russian Federation, Kazan, Spartakovskaya, 2b2 -> et-e1"
```
---

<h2>Документация rfc1459 (ред.)</h2>

**ПРИМЕЧАНИЕ:** Cервер не поддерживает коммуникацию *сервер-сервер*. Ниже представлена сокращенная документация стандарта [rfc1459](https://datatracker.ietf.org/doc/html/rfc1459).

<h3>Содержание:</h3>

1. [Сообщения](#message)
2. [Регистрация соединения](#registration)
    * [PASS](#pass)
    * [NICK](#nick)
    * [USER](#user)
    * [OPER](#oper)
    * [QUIT](#quit)
3. [Пользователи](#users)
   * [PRIVMSG](#privmsg)
   * [AWAY](#away)
   * [NOTICE](#notice)
   * [WHO](#who)
   * [WHOIS](#whois)
   * [WHOWAS](#whowas)
4. [Каналы](#channels)
    * [Оператор канала](#channel-operator)
    * [MODE](#mode)
    * [TOPIC](#topic)
    * [JOIN](#join)
    * [INVITE](#invite)
    * [KICK](#kick)
    * [PART](#part)
    * [NAMES](#names)
    * [LIST](#list)
5. [Прочее](#other)
    * [IRC-операторы и конфиг](#irc-operators) 
    * [WALLOPS](#wallops)
    * [PING / PONG](#ping-pong)
    * [ISON](#ison)
    * [USERHOST](#userhost)
	* [VERSION](#version)
    * [INFO](#info)
    * [ADMIN](#admin)
    * [TIME](#time)
6. [Числовые ответы](#numerical-answers)
    * [Ошибки](#errors)
    * [Ответы на команды](#command-responses)
    * [Зарезервированные числа](#reserved-nums)
7. [Ссылки на полезные материалы](#links)

---

## Сообщения <a name="message"></a>

Каждое IRC-сообщение может содержать до трех частей, разделенных одним (или более) символом пробела:

* Префикс (опционально)
* Команда
* Параметры команды (до 15).

**Префикс** используется серверами для обозначения источника появления сообщения. Обозначается одним символом `:`, стоящим вначале, который должен быть первым символом в сообщении. Между префиксом и `:` не должно быть никаких пробелов.

**Команда должна** содержать правильную IRC-команду или трехзначное число, представленное в ASCII-тексте. Максимальная длина строки для команд и параметров - 510 символов. Перенос строки означает конец команды.
На каждое успешное выполнение команды клиенту отправляется [числовой ответ](#command-responses). Если в сообщении неправильно указана команда и/или параметры, на этапе парсинга сервер может выдать клиенту [сообщение об ошибке](#errors).

Для обеспечения контроля от флуда, рекомендуется проверять частоту сообщений. Например, не принимать сообщения более, чем раз в 2 секунды и если этот лимит превышается, то увеличивать задержку для клиента на 2 секунды за каждое сообщение.

## Регистрация соединения <a name="registration"></a>

Обязательные условия для регистрации пользователя:

* PASS-сообщение
* NICK-сообщение
* USER-сообщение

PASS-сообщение не должно быть введено клиентом в последнюю очередь.
Если регистрация прошла успешна, клиенту отправляется MOTD (message of the day) с соответствующими числовыми ответами:

    375     RPL_MOTDSTART               ":- <server> Message of the day - "
    372     RPL_MOTD                    ":- <text>"
    376     RPL_ENDOFMOTD               ":End of /MOTD command"


### PASS <a name="pass"></a>

**Параметры:** `<password>`

Команда `PASS` используется для установки 'парольного соединения', в качестве пароля выступает второй аргумент,
переданный при запуске сервера. Пароль может и должен быть установлен перед любой попыткой регистрации соединения.
В текущий момент, это требования к клиентам отправлять команду `PASS` перед отправкой комбинации `NICK`/`USER`.

Числовые ответы:

            ERR_NEEDMOREPARAMS          ERR_ALREADYREGISTRED

Пример:
```
  PASS secretpassword
```

### NICK <a name="nick"></a>

**Параметры:** `<nickname>` `[ <hopcount> ]`

Официальная документация гласит, что если сервер получит NICK, который уже существует, то произойдет конфликт и командой KILL удалит этот никнейм из базы данных сервера.
На деле же клиент не сможет подключиться к серверу, пока введенный им ник занят пользователем, который находится в сети.
Если сообщение NICK станет причиной изменения никнейма, то оригинальный (старый) никнейм удалится.

Числовые ответы:

            ERR_NONICKNAMEGIVEN         ERR_ERRONEUSNICKNAME
            ERR_NICKNAMEINUSE           ERR_NICKCOLLISION

Пример:
```
  NICK Wiz		// Вступление нового никнейма "Wiz".
  :WiZ NICK Kilroy	// WiZ изменяет свой никнейм на Kilroy.
```

### USER <a name="user"></a>

**Параметры:** `<username>` `<hostname>` `<servername>` `<realname>`

Команда `USER` используется в начале соединения для указания имени пользователя, названия хоста, названия сервера и реального имени нового пользователя.

Параметры `<hostname>` `<servername>` не проверяются IRC-сервером, когда они используются клиентом, но могут использоваться при коммуникации *сервер-сервер*.
Должно быть объявлено, что параметр `<realname>` должен быть последним, потому что он может содержать пробелы и перед ним должен быть префикс `:`, делающим распознавание лучшим.

Числовые ответы:

            RR_NEEDMOREPARAMS           ERR_ALREADYREGISTRED

Пример:
```
  USER guest tolmoon tolsun :Ronnie Reagan
  // Пользователь зарегистрировал себя под именем "guest" и его реальное имя "Ronnie Reagan"
```

### OPER <a name="oper"></a>

**Параметры:** `<user>` `<password>`

Команда `OPER` используется авторизации за [IRC-оператора](#irc-operators) (не путать с [оператором канала](#channel-operator)). Параметры `<user>` и `<password>` используются для идентификации пользователя, запрашивающего права IRC-оператора.

При коммуникации *сервер-сервер*, если пользователь послал команду `OPER` с корректным паролем для текущего пользователя, сервер информирует сеть о новом операторе, используя `MODE +o` для никнейма клиента.

Числовые ответы:

            ERR_NEEDMOREPARAMS          RPL_YOUREOPER
            ERR_NOOPERHOST              ERR_PASSWDMISMATCH

Пример:
```
  OPER foo bar
  // Попытка зарегистрироваться как IRC-оператор, используя имя пользователя "foo" и пароль "bar".
```

### QUIT <a name="quit"></a>

**Параметры:** `[<Quit message>]`

Сессия пользователя заканчивается с QUIT-сообщением. Сервер должен завершить соединение с клиентом, когда увидит посланное сообщение.
Если, по какой-либо причине, соединение клиента закрылось без введения пользователем команды QUIT (например, обрыв связи), сервер потребует запаса в quit-сообщениях с некоторой сортировкой сообщения, в поисках причины разрыва.

Пример:
```
  QUIT :Gone to have lunch          // Обычный формат сообщения.
```

## Пользователи <a name="user"></a>

Каждый пользователь имеет никнейм длиной не больше девяти (9) символов.
Доступные символы в нике: `a-z`, `A-Z`, `0-9` и спец. символы `-`, `[`, `]`, `\`, `^`, `{`, `}` 

Сервер должен знать о клиенте:

* имя хоста, с которого запустился пользователь.
* имя пользователя на этом хосте.
* сервер, к которому присоединился пользователь.

Когда клиент закрывает соединение, создается QUIT-сообщение от имени клиента сервером.

### PRIVMSG <a name="privmsg"></a>

**Параметры:** `<receiver>{,<receiver>}` `<text to be sent>`

`PRIVMSG` используется для частной переписки между пользователями.
`<receiver>` – никнейм получателя сообщения. Так же там можно указать
список имен или каналов, разделенных запятыми.

Числовые ответы:

            ERR_NORECIPIENT             ERR_NOTEXTTOSEND
            ERR_CANNOTSENDTOCHAN        ERR_NOTOPLEVEL
            ERR_WILDTOPLEVEL            ERR_TOOMANYTARGETS
            ERR_NOSUCHNICK
            RPL_AWAY

Примеры:
```
  :Angel PRIVMSG Wiz :Hello are you receiving this message ?
  // Сообщение от Angel к Wiz;

  PRIVMSG Angel :yes I'm receiving it !receiving it !'u>(768u+1n) .br
  // Сообщение к Angel;
```

### AWAY <a name="away"></a>

**Параметры:** `[message]`

Команда `AWAY` устанавливает автоматический ответ на сообщение типа `PRIVMSG`, направленное клиенту
Использование `AWAY` без указания параметра, снимает автоматический ответ, если ранее он был установлен.

Числовые ответы:

            RPL_UNAWAY                  RPL_NOWAWAY

Примеры:
```
AWAY :Gone to lunch.  Back in 5     // установка away-сообщения "Gone to lunch. Back in 5".
:WiZ AWAY                           // снятие пользователем WiZ сообщения AWAY.
```

### NOTICE <a name="notice"></a>

**Параметры:** `<nickname>` `<text>`

Эта команда работает так же, как `PRIVMSG`, за исключением того, что автоматический ответ `AWAY` не может быть отправлен в ответ на сообщение.
Сообщение `NOTICE` не отправится, если у получателя стоит [режим `+s`](#user-modes).

### WHO <a name="who"></a>

**Параметры:** `[<nickmask> [<o>]]`

Возвращает список пользователей, совпадающих с параметром `name`, за исключением [невидимых пользователей (+i)](#user-modes)
Если установлен флаг `o`, возвращается информация только об IRC-операторах.

Числовые ответы:

            ERR_NOSUCHSERVER
            RPL_WHOREPLY                RPL_ENDOFWHO

Примеры:
```
  WHO *.fi        // Список пользователей, соответствующие маске "*.fi";
  WHO jto* o      // Список пользователей, соответствующие маске "jto*", если они являются операторами.
```

### WHOIS <a name="whois"></a>

**Параметры:** `<nickmask>`

Cервер ответит на это сообщение несколькими числовыми сообщениями с указанием разных статусов каждого пользователя.
Если в `<nickmask>` не указана никакая информация о том, какой никнейм опросить, вы получите
информацию о всех доступных никнеймах. Запятая разделяет список никнеймов.

Числовые ответы:

            ERR_NOSUCHSERVER            ERR_NONICKNAMEGIVEN
            RPL_WHOISUSER               RPL_WHOISCHANNELS
            RPL_WHOISCHANNELS           RPL_WHOISSERVER
            RPL_AWAY                    RPL_WHOISOPERATOR
            RPL_WHOISIDLE               ERR_NOSUCHNICK
            RPL_ENDOFWHOIS

Примеры:
```
  WHOIS wiz             // возврат пользователю информацию о никнейме WiZ
```

### WHOWAS <a name="whowas"></a>

**Параметры:** `<nickname>` `[<count>]`

Возвращает информацию об имени пользователя, которое сейчас не используется (в связи с отключением пользователя или изменением его имени).
Если определен параметр `<count>`, возвращается информация о последних `<count>` случаях использования ника.

Числовые ответы:

            ERR_NONICKNAMEGIVEN         ERR_WASNOSUCHNICK
            RPL_WHOWASUSER              RPL_WHOISSERVER
            RPL_ENDOFWHOWAS

Примеры:
```
  WHOWAS Wiz            // возврат всей информации в истории о никнейме "WiZ"
  WHOWAS Mermaid 9      // возврат 9 наиболее запрашиваемых вводов в никнейм-истории для "Mermaid"
```

## Каналы <a name="channels"></a>

Канал создается при соединении первого клиента с ним и канал исчезает, когда его покидает последний пользователь. Пока канал отсутствует, любой пользователь может завладеть каналом, назвав свой таким же именем.
**Имя канала** – строка (начинающаяся с символа `&` или `#`) длинной до 200 символов.

Для создания нового канала или входа в существующий, пользователь должен запросить [`JOIN`](#join) канала. Если канал отсутствовал, то канал создается, и вошедший пользователь становится оператором канала. Если канал уже существует, но так или иначе он не отвечает на попытки войти, значит в настройках канала установлен какой-либо из нижеприведенных режимов. Возможно, этот канал только для приглашенных (invite-only), (режим +i), и вы сможете на его войти только будучи приглашенным.
Пользователь может находиться в нескольких каналах одновременно, но рекомендуется ограничиться десятью (10) каналами.

Название канал может в себе содержать любой ASCII-символ, кроме `\r`, `\n`, `\g`, `SPACE`, `\0`, `,`.

Протоколом предоставляется два типа каналов:

* Один распространяемый канал, который известен всем серверам, подсоединенным к сети.
Эти каналы доступны только тем клиентам, на сервере которых он существует. Такие каналы отличаются символом `&` перед названием.
* Остальные каналы помечены символом `#`.

Так как в этом проекте не реализовано подключение *сервер-сервер*, эти каналы ничем не будут отличаться.

### Оператор канала <a name="channel-operator"></a>

Оператор канала, идентифицируется символом `@`, следующим за его никнеймом, всякий раз, как он ассоциируется с каналом (например, ответы на команды `NAMES`, `WHO` и `WHOIS`).
Если создатель канала покинет его, то права оператора наследуются самому старому по времени нахождения пользователю или оператору, если таковой существует.
Оператор канала имеет возможность выполнять команды:

* `MODE`
* `TOPIC` (+t)
* `INVITE` (+i)
* `KICK`

### MODE <a name="mode"></a>

Команда `MODE` позволяет настроить режим имен пользователей или каналов.

Числовые ответы:

            ERR_NEEDMOREPARAMS          RPL_CHANNELMODEIS
            ERR_CHANOPRIVSNEEDED        ERR_NOSUCHNICK
            ERR_NOTONCHANNEL            ERR_KEYSET
            RPL_BANLIST                 RPL_ENDOFBANLIST
            ERR_UNKNOWNMODE             ERR_NOSUCHCHANNEL

            ERR_USERSDONTMATCH          RPL_UMODEIS
            ERR_UMODEUNKNOWNFLAG

#### Режимы канала <a name="channel-modes"></a>

**Параметры:** `<channel>` `{[+|-]|o|p|s|i|t|n|b|v}` `[<limit>]` `[<user>]` `[<ban mask>]`

Режимы и характеристику канала может изменить только оператор канала т.е. тот, кто его создал.
<br> Список доступных режимов канала:

  * o – брать/давать привилегии операторов канала
  * p – флаг приватности канала;
  * s – флаг секретности канала;
  * i – флаг канала invite-only;
  * t – при установке этого флага, менять топик могут только операторы;
  * n – запрещает сообщения на канал от посторонних клиентов;
  * m – модерируемый канал;
  * l – установка ограничения на количество пользователей;
  * b – установка маски бана;
  * v – брать/давать возможность голоса при модерируемом режиме;
  * k – установка пароля на канал.

Примеры использования режимов канала:
```
  MODE #Finnish +im                 // Делает канал #Finnish модерируемым и 'invite-only'.
  MODE #Finnish +o Kilroy           // Дает привилегии оператора Kilroy на канале #Finnish.
  MODE #Finnish +v Wiz              // Дает WiZ право голоса на канале #Finnish.
  MODE #Fins -s                     // Убирает флаг 'secret' с канала #Fins.
  MODE #42 +k oulu                  // Устанавливает на канал пароль "oulu".
  MODE #eu-opers +l 10              // Устанавливает максимальное количество пользователей на канале (10).
  MODE &oulu +b                     // Вывод списка масок бана для канала.
  MODE &oulu +b *!*@*               // Предотвращает вход на канал для любого пользователя.
  MODE &oulu +b *!*@*.edu           // Предотвращает вход любого пользователя подходящего под маску хоста *.edu.
```

#### Режимы пользователя <a name="user-modes"></a>

**Параметры:** `<nickname>` `{[+|-]|i|w|s|o}`

Режимы пользователя обычны такими изменениями, которые воздействуют на то, каким видят клиента или какие 'экста'-сообщения посылает пользователь.
Пользовательская команда `MODE` может относиться или к отправителю сообщения или к тому, чей никнейм указали в качестве параметра.
<br> Доступные режимы:

  * i – делает пользователя невидимым;
  * s – пользователь (не)получает [NOTICE](#notice) сообщения;
  * o – флаг IRC-оператора;
  * w – пользователь (не)получает [WALLOPS](#wallops) сообщения;

Если пользователь пытается сделать себя IRC-оператором, используя `+o` флаг, его попытка будет проигнорирована. Это не разрешено, в отличие от сброса себя самого (используя `-o`).

Примеры использования пользовательских режимов:
```
  :MODE WiZ -w                      // отключает прием сообщений WALLOPS для WiZ.
  :Angel MODE Angel +i              // Сообщение от Angel далает его невидимым.
  MODE WiZ -o                       // WiZ сбрасывает себе статус оператора.
```

### TOPIC <a name="topic"></a>

**Параметры:** `<channel>` `[<topic>]`

Используется для изменения или просмотра топика канала. Топик канала `<channel>` останется прежним, если не будет задан новый топик `<topic>`.
Если параметр `<topic>` подставлен – топик канала изменится, если режим канала позволяет это сделать. Если у канала стоит флаг `t`, топик могут менять только операторы.

Числовые ответы:

            ERR_NEEDMOREPARAMS          ERR_NOTONCHANNEL
            RPL_NOTOPIC                 RPL_TOPIC
            ERR_CHANOPRIVSNEEDED

Примеры:
```
  :Wiz TOPIC #test :New topic       // Пользователь Wiz устанавливает топик.
  TOPIC #test :another topic        // Установка на #test топика "another topic".
  TOPIC #test                       // Проверка топика на #test.
```

### JOIN <a name="join"></a>

**Параметры:** `<channel>{,<channel>}` `[<key>{,<key>}]`

Команда `JOIN` используется клиентом для входа на канал. Для входа в канал, должны соблюдаться следующе условия:

1. Пользователь может быть приглашен, если канал invite-only.
2. Никнейм/имя пользователя/имя хоста не должны быть забанеными.
3. Если установлен пароль, но должен быть верным.

Когда пользователи заходят на канал, они получат уведомление о всех командах их сервера. Оно вмещает в себе `MODE`, `KICK`, `PART`, `QUIT` и, конечно же, `PRIVMSG`/`NOTICE`.

Если `JOIN` выполнился успешно, пользователь получает топик канала(используя `RPL_TOPIC`) и список пользователей, которые находятся на канале (используя `RPL_NAMREPLY`).

Числовые ответы:

            ERR_NEEDMOREPARAMS          ERR_BANNEDFROMCHAN
            ERR_INVITEONLYCHAN          ERR_BADCHANNELKEY
            ERR_CHANNELISFULL           ERR_BADCHANMASK
            ERR_NOSUCHCHANNEL           ERR_TOOMANYCHANNELS
            RPL_TOPIC

Примеры:
```
  JOIN #foobar                      // вход на канал #foobar.
  JOIN &foo fubar                   // вход на канал &foo, используя ключ "fubar".
  JOIN #foo,&bar fubar              // вход на канал #foo, используя ключ "fubar" и на канал &bar без использования ключа.
  JOIN #foo,#bar fubar,foobar       // вход на канал #foo, используя ключ "fubar" и на канал #bar, используя ключ "foobar".
  JOIN #foo,#bar                    // вход на каналы #foo и #bar.
  :WiZ JOIN #Twilight_zone          // JOIN-сообщение от WiZ
```

### INVITE <a name="invite"></a>

**Параметры:** `<nickname>` `<channel>`

Команда `INVITE` используется для приглашения пользователей на канал.
Параметр `<nickname>` – указание пользователя, которого требуется пригласить в канал `<channel>`.
Если канал является invite-only (MODE +i), приглашать в него может только оператор канала.

Числовые ответы:

            ERR_NEEDMOREPARAMS          ERR_NOSUCHNICK
            ERR_NOTONCHANNEL            ERR_USERONCHANNEL
            ERR_CHANOPRIVSNEEDED
            RPL_INVITING                RPL_AWAY

Примеры:
```
  :Angel INVITE Wiz #Dust           // Пользователь Angel пригласил WiZ в канал #Dust
  INVITE Wiz #Twilight_Zone         // Команда приглашения WiZ в канал #Twilight_zone
```

### KICK <a name="kick"></a>

**Параметры:** `<channel>` `<user>` `[<comment>]`

Команда `KICK` исключает пользователя из канала может быть использована только оператором канала.

Числовые ответы:

            ERR_NEEDMOREPARAMS          ERR_NOSUCHCHANNEL
            ERR_BADCHANMASK             ERR_CHANOPRIVSNEEDED
            ERR_NOTONCHANNEL

Примеры:
```
  KICK &Melbourne Matthew                 // Кик Matthew с канала &Melbourne
  KICK #Finnish John :Speaking English    // Кик John с #Finnish, указав комментарий/причину "Speaking English".
  :WiZ KICK #Finnish John                 // KICK-сообщение WiZ удаляет John с канала #Finnish
```

### PART <a name="part"></a>

**Параметры:** `<channel>{,<channel>}`

С помощью команды `PART` пользователь может покинуть каналы, которые он укажет в параметрах.

Числовые ответы:

            ERR_NEEDMOREPARAMS          ERR_NOSUCHCHANNEL
            ERR_NOTONCHANNEL

Примеры:
```
  PART #twilight_zone               // покинуть канал "#twilight_zone"
  PART #oz-ops,&group5              // покинуть каналы "&group5" и "#oz-ops".
```

### NAMES <a name="names"></a>

**Параметры:** `[<channel>{,<channel>}]`

Используя команду `NAMES`, пользователь может получить список всех
пользователей, состоящих в канале (при условии, что канал виден отправителю команды).
Каналы, которые не видны пользователю – это приватные (+p), секретные (+s) и в которых он уже состоит.
Эта команда не возвращает кода ошибки из-за неправильных названий каналов.

Если параметр `<channel>` не задан, выводится список всех каналов и имен тех, кто в них состоит.
И к концу выводимого списка – лист пользователей, которые не состоят ни в одном видимом канале.

Числовые ответы:

            RPL_NAMREPLY                RPL_ENDOFNAMES

Примеры:
```
  NAMES #twilight_zone,#42          // Список видимых пользователей на канале #twilight_zone и #42, если каналы видимы вам.
  NAMES                             // Список всех видимых каналов и пользователей.
```

### LIST <a name="list"></a>

**Параметры:** `[<channel>{,<channel>}` `[<server>]]`

`LIST` используется для вывода списка каналов и их топиков. Если используется параметр `<channel>`, то выводится только статус этого канала.

Приватные каналы указаны (без их топиков) как каналы "Prv", без указания количества клиентов, сидящих на этом канале.
Секретные каналы не выводятся в списке, если пользователь не является членом подобного канала.

Числовые ответы:

            ERR_NOSUCHSERVER            RPL_LISTSTART
            RPL_LIST                    RPL_LISTEND

Примеры:
```
  LIST                              // Список всех каналов.
  LIST #twilight_zone,#42           // Список каналов #twilight_zone и #42
```

## Прочее <a name="other"></a>

### IRC-операторы и конфиг<a name="irc-operators"></a>

IRC-операторы – специальный класс клиентов, имеющий возможность выполнять команды:

* `RESTART` – перезапускает сервер.
<br>Числовые ответы:

        ERR_NOPRIVILEGES

* `KILL` – отключает пользователя от сервера. Принимает два обязательных параметра `<user>` и `<message>`
<br>Числовые ответы:

        ERR_NOPRIVILEGES                ERR_NEEDMOREPARAMS
        ERR_NOSUCHNICK                  ERR_CANTKILLSERVER

* `REHASH` – перечитывает конфигурационный файл, в котором содержатся никнеймы и пароли IRC-операторов.
<br>Числовые ответы:

        RPL_REHASHING                   ERR_NOPRIVILEGES

IRC-операторы, в отличие от остальных пользователей, имеют наивысшие права и собственные пароли. Чтобы произвести пользователю авторизацию в качестве IRC-оператора, требуется выполнить команду [OPER](#oper).

#### Конфигурационный файл

Предоставляя гибкость настройки и запуска сервера, рекомендуется использовать конфигурационный файл. Минимальные требования к содержанию конфига:

 * С каких хостов допускать соединения клиентов.
 * С каких хостов допускать серверные соединения.
 * Местоположение сервера(университет, город/район, компания и т.п.).
 * Администратор сервера и его e-mail.
 * Никнеймы и пароли IRC-операторов.
 
Пароли IRC-операторов рекомендуется хранить в зашифрованном виде. Например, с помощью [SHA256](https://wikipedia.org/wiki/SHA-2)

### WALLOPS <a name="wallops"></a>

**Параметры:** `<message>`

Команда отправляет сообщения всем IRC-операторам, находящимся в сети, кроме тех, кто отключил уведомления с помощью `MODE -w`.
Чтобы избежать злоупотребления командой `WALLOPS`, рекомендуется ограничить его использование для обычных пользователей.

Числовые ответы:

            ER_NEEDMOREPARAMS

### PING / PONG <a name="ping-pong"></a>

**Параметры:** `<server1>` `[<server2>]`

`PING` используется для проверки наличия активности клиента на другом конце соединения. Это сообщение посылается с регулярными интервалами, если не замечено другой активности, исходящей от соединения.
Если клиент не посылает `PONG` в ответ на `PING` – соединение закрывается.

Числовые ответы:

            ERR_NOORIGIN                ERR_NOSUCHSERVER

### ISON <a name="ison"></a>

**Параметры:** `<nickname>{<space><nickname>}`

Принимает в качестве параметра `<nickname>` список пользователей, разделенных пробелами.
Сервер возвращает список тех из них, которые в данный момент находятся в сети, в виде разделенного пробелами списка.
Если ни один из них не находится в сети, сервер возвращает пустой список.

Числовые ответы:

            RPL_ISON                    ERR_NEEDMOREPARAMS

### USERHOST <a name="userhost"></a>

**Параметры:** `<nickname>{<space><nickname>}`

Команда `USERHOST` требует список до 5-ти никнеймов, разделеных пробелами и возвращает информацию о каждом никнейме, который был найден.

Числовые ответы:

            RPL_USERHOST                ERR_NEEDMOREPARAMS

Примеры:
```
  USERHOST Wiz Michael Marty p          // USERHOST запрос информации о никнеймах "Wiz", "Michael", "Marty" и "p"
```

### VERSION <a name="version"></a>

**Параметры:** `[<server>]`

Возвращает версию сервера `<server>`, или текущего сервера, если этот параметр не определен.

Числовые ответы:

            ERR_NOSUCHSERVER            RPL_VERSION

### INFO <a name="info"></a>

**Параметры:** `[<server>]`

Возвращает информацию о текущем сервере. Информацию включает версию сервера, дату его компиляции, информацию о наложенных патчах, дате запуска, и любую другую информацию, которая может являться нужной.

Числовые ответы:

            ERR_NOSUCHSERVER
            RPL_INFO                    RPL_ENDOFINFO

### ADMIN <a name="admin"></a>

**Параметры:** `[<server>]`

Возвращает информацию об администраторе указанного или текущего сервера, которая содержится в конфиг-файле.

Числовые ответы:

            ERR_NOSUCHSERVER
            RPL_ADMINME                 RPL_ADMINLOC
            RPL_ADMINLOC2               RPL_ADMINEMAIL


### TIME <a name="time"></a>

**Параметры:** `[<server>]`

Возвращает локальное время указанного или текущего сервера.

Числовые ответы:

            ERR_NOSUCHSERVER            RPL_TIME

## Числовые ответы <a name="numerical-answers"></a>

Ниже приводится список числовых ответов, которые генерируются в ответ на команды.

### Ошибки <a name="errors"></a>

    401     ERR_NOSUCHNICK              "<nickname> :No such nick/channel"
    402     ERR_NOSUCHSERVER            "<server name> :No such server"
    403     ERR_NOSUCHCHANNEL           "<channel name> :No such channel"
    404     ERR_CANNOTSENDTOCHAN        "<channel name> :Cannot send to channel"
    405     ERR_TOOMANYCHANNELS         "<channel name> :You have joined too many channels"
    406     ERR_WASNOSUCHNICK           "<nickname> :There was no such nickname"
    407     ERR_TOOMANYTARGETS          "<target> :Duplicate recipients. No message delivered"
    409     ERR_NOORIGIN                ":No origin specified"
    411     ERR_NORECIPIENT             ":No recipient given (<command>)"
    412     ERR_NOTEXTTOSEND            ":No text to send"
    413     ERR_NOTOPLEVEL              "<mask> :No toplevel domain specified"
    414     ERR_WILDTOPLEVEL            "<mask> :Wildcard in toplevel domain"
    421     ERR_UNKNOWNCOMMAND          "<command> :Unknown command"
    422     ERR_NOMOTD                  ":MOTD File is missing"
    423     ERR_NOADMININFO             "<server> :No administrative info available"
    424     ERR_FILEERROR               ":File error doing <file op> on <file>"
    431     ERR_NONICKNAMEGIVEN         ":No nickname given"
    432     ERR_ERRONEUSNICKNAME        "<nick> :Erroneus nickname"
    433     ERR_NICKNAMEINUSE           "<nick> :Nickname is already in use"
    436     ERR_NICKCOLLISION           "<nick> :Nickname collision KILL"
    441     ERR_USERNOTINCHANNEL        "<nick> <channel> :They aren't on that channel"
    442     ERR_NOTONCHANNEL            "<channel> :You're not on that channel"
    443     ERR_USERONCHANNEL           "<user> <channel> :is already on channel"
    444     ERR_NOLOGIN                 "<user> :User not logged in"
    445     ERR_SUMMONDISABLED          ":SUMMON has been disabled"
    446     ERR_USERSDISABLED           ":USERS has been disabled"
    451     ERR_NOTREGISTERED           ":You have not registered"
    461     ERR_NEEDMOREPARAMS          "<command> :Not enough parameters"
    462     ERR_ALREADYREGISTRED        ":You may not reregister"
    463     ERR_NOPERMFORHOST           ":Your host isn't among the privileged"
    464     ERR_PASSWDMISMATCH          ":Password incorrect"
    465     ERR_YOUREBANNEDCREEP        ":You are banned from this server"
    467     ERR_KEYSET                  "<channel> :Channel key already set"
    471     ERR_CHANNELISFULL           "<channel> :Cannot join channel (+l)"
    472     ERR_UNKNOWNMODE             "<char> :is unknown mode char to me"
    473     ERR_INVITEONLYCHAN          "<channel> :Cannot join channel (+i)"
    474     ERR_BANNEDFROMCHAN          "<channel> :Cannot join channel (+b)"
    475     ERR_BADCHANNELKEY           "<channel> :Cannot join channel (+k)"
    481     ERR_NOPRIVILEGES            ":Permission Denied- You're not an IRC operator"
    482     ERR_CHANOPRIVSNEEDED        "<channel> :You're not channel operator"
    483     ERR_CANTKILLSERVER          ":You cant kill a server!"
    491     ERR_NOOPERHOST              ":No O-lines for your host"
    501     ERR_UMODEUNKNOWNFLAG        ":Unknown MODE flag"
    502     ERR_USERSDONTMATCH          ":Cant change mode for other users"

### Ответы на команды <a name="command-responses"></a>

    300     RPL_NONE
    302     RPL_USERHOST                ":[<reply>{<space><reply>}]"
    303     RPL_ISON                    ":[<nick> {<space><nick>}]"
    301     RPL_AWAY                    "<nick> :<away message>"
    305     RPL_UNAWAY                  ":You are no longer marked as being away"
    306     RPL_NOWAWAY                 ":You have been marked as being away"
    311     RPL_WHOISUSER               "<nick> <user> <host> * :<real name>"
    312     RPL_WHOISSERVER             "<nick> <server> :<server info>"
    313     RPL_WHOISOPERATOR           "<nick> :is an IRC operator"
    317     RPL_WHOISIDLE               "<nick> <integer> :seconds idle"
    318     RPL_ENDOFWHOIS              "<nick> :End of /WHOIS list"
    319     RPL_WHOISCHANNELS           "<nick> :{[@|+]<channel><space>}"
    314     RPL_WHOWASUSER              "<nick> <user> <host> * :<real name>"
    369     RPL_ENDOFWHOWAS             "<nick> :End of WHOWAS"
    321     RPL_LISTSTART               "Channel :Users  Name"
    322     RPL_LIST                    "<channel> <# visible> :<topic>"
    323     RPL_LISTEND      	        ":End of /LIST"
    324     RPL_CHANNELMODEIS           "<channel> <mode> <mode params>"
    331     RPL_NOTOPIC                 "<channel> :No topic is set"
    332     RPL_TOPIC                   "<channel> :<topic>"
    341     RPL_INVITING                "<channel> <nick>"
    342     RPL_SUMMONING               "<user> :Summoning user to IRC"
    351     RPL_VERSION                 "<version>.<debuglevel> <server> :<comments>"
    352     RPL_WHOREPLY                "<channel> <user> <host> <server> <nick> <H|G>[*][@|+] :<hopcount> <real name>"
    315     RPL_ENDOFWHO                "<name> :End of /WHO list"
    353     RPL_NAMREPLY                "<channel> :[[@|+]<nick> [[@|+]<nick> [...]]]"
    366     RPL_ENDOFNAMES              "<channel> :End of /NAMES list"
    364     RPL_LINKS                   "<mask> <server> :<hopcount> <server info>"
    365     RPL_ENDOFLINKS              "<mask> :End of /LINKS list"
    367     RPL_BANLIST                 "<channel> <banid>"
    368     RPL_ENDOFBANLIST            "<channel> :End of channel ban list"
    371     RPL_INFO                    ":<string>"
    374     RPL_ENDOFINFO               ":End of /INFO list"
    375     RPL_MOTDSTART               ":- <server> Message of the day - "
    372     RPL_MOTD                    ":- <text>"
    376     RPL_ENDOFMOTD               ":End of /MOTD command"
    381     RPL_YOUREOPER               ":You are now an IRC operator"
    382     RPL_REHASHING               "<config file> :Rehashing"
    391     RPL_TIME                    "<server> :<string showing server's local time>"
    392     RPL_USERSSTART              ":UserID   Terminal  Host"
    393     RPL_USERS                   ":%-8s %-9s %-8s"
    394     RPL_ENDOFUSERS              ":End of users"
    395     RPL_NOUSERS                 ":Nobody logged in"
    200     RPL_TRACELINK               "Link <version & debug level> <destination> <next server>"
    201     RPL_TRACECONNECTING         "Try. <class> <server>"
    202     RPL_TRACEHANDSHAKE          "H.S. <class> <server>"
    203     RPL_TRACEUNKNOWN            "???? <class> [<client IP address in dot form>]"
    204     RPL_TRACEOPERATOR           "Oper <class> <nick>"
    205     RPL_TRACEUSER               "User <class> <nick>"
    206     RPL_TRACESERVER             "Serv <class> <int>S <int>C <server> <nick!user|*!*>@<host|server>"
    208     RPL_TRACENEWTYPE            "<newtype> 0 <client name>"
    261     RPL_TRACELOG                "File <logfile> <debug level>"
    211     RPL_STATSLINKINFO           "<linkname> <sendq> <sent messages> <sent bytes> <received messages> <received bytes> <time open>"
    212     RPL_STATSCOMMANDS           "<command> <count>"
    213     RPL_STATSCLINE              "C <host> * <name> <port> <class>"
    214     RPL_STATSNLINE              "N <host> * <name> <port> <class>"
    215     RPL_STATSILINE              "I <host> * <host> <port> <class>"
    216     RPL_STATSKLINE              "K <host> * <username> <port> <class>"
    218     RPL_STATSYLINE              "Y <class> <ping frequency> <connect frequency> <max sendq>"
    219     RPL_ENDOFSTATS              "<stats letter> :End of /STATS report"
    221     RPL_UMODEIS                 "<user mode string>"
    241     RPL_STATSLLINE              "L <hostmask> * <servername> <maxdepth>"
    242     RPL_STATSUPTIME             ":Server Up %d days %d:%02d:%02d"
    243     RPL_STATSOLINE              "O <hostmask> * <name>"
    244     RPL_STATSHLINE              "H <hostmask> * <servername>"
    251     RPL_LUSERCLIENT             ":There are <integer> users and <integer> invisible on <integer> servers"
    252     RPL_LUSEROP                 "<integer> :operator(s) online"
    253     RPL_LUSERUNKNOWN            "<integer> :unknown connection(s)"
    254     RPL_LUSERCHANNELS           "<integer> :channels formed"
    255     RPL_LUSERME                 ":I have <integer> clients and <integer> servers"
    256     RPL_ADMINME                 "<server> :Administrative info"
    257     RPL_ADMINLOC1               ":<admin info>"
    258     RPL_ADMINLOC2               ":<admin info>"
    259     RPL_ADMINEMAIL              ":<admin info>"

### Зарезервированные числа <a name="reserved-nums"></a>

    209     RPL_TRACECLASS              217     RPL_STATSQLINE
    231     RPL_SERVICEINFO             232     RPL_ENDOFSERVICES
    233     RPL_SERVICE                 234     RPL_SERVLIST
    235     RPL_SERVLISTEND
    316     RPL_WHOISCHANOP             361     RPL_KILLDONE
    362     RPL_CLOSING                 363     RPL_CLOSEEND
    373     RPL_INFOSTART               384     RPL_MYPORTIS
    466     ERR_YOUWILLBEBANNED         476     ERR_BADCHANMASK
    492     ERR_NOSERVICEHOST

## Ссылки на полезные материалы <a name="links"></a>

1. https://datatracker.ietf.org/doc/html/rfc1459 – документация rfc1459 на английском
2. https://www.lissyara.su/doc/rfc/rfc1459/ – документация rfc1459, переведенная на русский
3. https://ru.wikipedia.org/wiki/Список_IRC-команд – wiki с основными командами IRC более простыми словами
4. https://masandilov.ru/network/guide_to_network_programming – гайд по сетевому программированию
5. https://ncona.com/2019/04/building-a-simple-server-with-cpp/ – про сокеты и TCP-сервер на C++
6. https://youtu.be/cNdlrbZSkyQ – еще немного про сокеты