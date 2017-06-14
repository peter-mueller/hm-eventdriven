Abstract
========

Einleitung
==========

Anforderungen
=============

Es soll eine benutzerfreundliche Kaffeemaschine auf dem MCB2300
realisiert werden.

User Stories
------------

-   Ich als Kaffeetrinker möchte die momentane Uhrzeit auf dem Display
    sehen, um festzustellen, wann der nächste Kaffee gebraut wird.

-   Ich als Kaffeetrinker möchte beim ersten Start die Uhrzeit anpassen
    können, um sie auf meine Ortszeit anzupassen.

-   Ich als Kaffeetrinker möchte die Startzeit für den Brühvorgang
    einstellen, um erst so spät wie möglich aufstehen zu müssen.

-   Ich als Kaffeetrinker möchte die Stärke zwischen leicht, mittel und
    stark einstellen, um den Kaffee anzupassen.

-   Ich als Kaffeetrinker möchte, dass der Kaffee nicht ausgegeben wird,
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


