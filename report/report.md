Einleitung
==========

Mit einer Statemachine lassen sich vielfältigste Aufgaben lösen. Von einfachen
Taschenrechnern bis hin zu komplexen Smartwatches und intelligenten Routern
sind verschiedenste Anwendungen gegeben.

Doch leider wird gerade bei komplexen Projekten in der Praxis häufig die Größe
der Statemachine unterschätzt. Gerade während des gesamten Softwarelebenszyklus
ergeben sich durch vielfältige Änderungen schnell unkontrollierbare Konstrukte,
die sich nur mit viel Aufwand stabilisieren und erweitern lassen.  
Damit die Wartbarkeit nicht unter der Komplexität leidet, wurden diverse
Pattern als Erweiterung einer einfachen Statemachine entwicklet. Hierzu zählt
die hierarchische Statemachine und das Active Object Pattern.

Dabei ist beim Active Object Pattern der "Process" Schritt klar vom "Dispatch"
getrennt. Ein Event wird somit nicht direkt verarbeitet, sondern in eine Queue
abgelegt. Ein Scheduler entscheidet anschließend, welche Events als nächstes
verarbeitet werden sollen. Hierzu nimmt er eins aus der Queue und ruft den
entsprechenden Eventhandler auf.

Durch dieses Vorgehen wird eine "Inversion of Control" erreicht, was die
Verarbeitung von dem Event-Erzeugen gut entkoppelt und die Wartbarkeit deutlich
steigert.

Um diese und weitere Pattern praktisch an einem Beispiel zu erproben, wird in
dieser Arbeit die Umsetzung einer zeitgesteuerten Kaffeemachine mittels einer
Active Object getriebenen Statemachine beschrieben. Hierzu werden im nächsten
Kapitel praxisnahe Anforderungen definiert, die anschließend mit einem MCB2300
Board unter Zuhilfenahme des QP Frameworks umgesetzt werden. Diese Umsetzung wird
im folgenden Kapitel beschrieben und an Beispielen verdeutlicht. Das anschließende
Kapitel gibt dann eine kurzes Fazit zur Umsetzung und bewertet die gewonnen
Erkenntnisse.

Im letzten Kapitel fließt noch ein getrenntes Feedback der Autoren ein, dass die
individuelle Umsetzung veranschaulicht.

Anforderungen
=============

Das Ziel der Umsetzung ist eine benutzerfreundliche Kaffeemaschine,
welche auf dem MCB2300 realisiert werden soll. Damit der Nutzer seinen Kaffee
immer zum gewünschten Zeitpunkt erhält, soll eine Zeitsteuerung für den
Brühvorgang umgesetzt werden. Dadurch kann er z.B. am Vortag die Kaffeemaschine
so einstellen, dass am nächsten Morgen direkt nach dem Aufstehen der Kaffee
schon bereit steht. Um dies zu erreichen ist ein Menü zum Einstellen der
Alarmzeit einzubauen, bei dem sich sowohl Stunden als auch Minuten des geplanten
Brühvorganges einstellen lassen.

Damit die Kaffeemaschine z.B. während eines Urlaubs keinen Kaffee zubereitet,
soll sich die Zeitsteuerung (Alarm) auch deaktiviert werden könne. Hierzu ist
im Menü eine entsprechende Einstellung vorzusehen. Auch soll der
Brühvorgang dann nicht stattfinden, wenn keine Kaffeekanne bereit steht. So wird
ein Schaden beim Nutzer vermieden, der durch einen unkontrollierten Brühvorgang
entstehen würde.

Des Weiteren soll die Kaffeestärke vom Nutzer eingestellt werden können, damit er
den Kaffee so erhält, wie er ihn bevorzugt trinkt. Dazu ist im Menü eine Einstellung
für die Kaffeestärke einzurichten, welche zusätzlich die aktuell eingestellt Stärke
auch durch separate Leuchten dauerhaft und unabhängig von der momentanen
Anzeige darstellt.

Aus diesen Anforderungen können folgende User Stories abgeleitet werden, die die
benötigte Funktionalität noch einmal kurz zusammenfasst.

User Stories
------------

-   Als Kaffeetrinker möchte ich die momentane Uhrzeit auf dem Display
    sehen, um festzustellen, wann der nächste Kaffee gebraut wird.

-   Als Kaffeetrinker möchte ich die Startzeit für den Brühvorgang
    einstellen, um erst so spät wie möglich aufstehen zu müssen.

-   Als Kaffeetrinker möchte ich die Stärke zwischen leicht, mittel und
    stark einstellen, um den Kaffee anzupassen.

-   Als Kaffeetrinker möchte ich den automatischen Brühvorgang ein- bzw.
    ausstellen können, damit bei meiner Abwesenheit kein Kaffee gebrüht wird.

-   Als Kaffeetrinker möchte ich, dass der Kaffee nicht ausgegeben wird,
    wenn kein Behälter unter der Ausgabe ist, um Sauereien zu vermeiden.

Technische Anforderungen
------------------------

Clock
:   Integration der RTC mit gegebenem Treiber. Die Zeit soll nach ISO
    auf dem LCD angezeigt werden: `2004-06-14T23:34:30`

Control Menu

:   Über den INT0 Knopf wird ein Menü durchlaufen. Drücken des Knopfes
    bedeutet immer eine Bestätigung. Das Menü wird
    sequentiell durchlaufen.

    1.  Kaffeestärke
    2.  Startzeit für Brühvorgang

Einstellen der Kaffeestärke
:   AD/DC Rad wechselt zwischen leicht, mittel und stark. Die LEDs
    zeigen die Stärke an. Zwei für leicht, vier für mittel und sechs
    für stark.

Einstellen von Uhrzeiten
:   Die Werte werden von links nach rechts (Jahr, Monat, Tag, Stunde,
    Minute, Sekunde) eingestellt. Mit INT0 wird eine Zahl bestätigt und
    zur nächsten gewechselt. Die aktuell aktive Zahl wird markiert.

Simulieren des zu frühen Wegnehmens der Tasse
:   Brüht die Maschine gerade Kaffee und wird der INTO Knopf gedrückt,
    ist das gleichwertig zu dem Entfernen einer Tasse.

Umsetzung
=============

UML Modellierung
------------------------

Orthogonale Region
------------------------

Verwendete Treiber
------------------------

Anpassungen am Board Support Package
------------------------

Resümee Umsetzung
=============

Persönliche Berichte
=============

René Zarwel
------------------------
Peter Müller
------------------------
Andreas Wilhelm
------------------------
