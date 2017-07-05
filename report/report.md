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

Es soll eine benutzerfreundliche Kaffeemaschine auf dem MCB2300
realisiert werden.

User Stories
------------

-   Als Kaffeetrinker möchte die momentane Uhrzeit auf dem Display
    sehen, um festzustellen, wann der nächste Kaffee gebraut wird.

-   Als Kaffeetrinker möchte beim ersten Start die Uhrzeit anpassen
    können, um sie auf meine Ortszeit anzupassen.

-   Als Kaffeetrinker möchte die Startzeit für den Brühvorgang
    einstellen, um erst so spät wie möglich aufstehen zu müssen.

-   Als Kaffeetrinker möchte die Stärke zwischen leicht, mittel und
    stark einstellen, um den Kaffee anzupassen.

-   Als Kaffeetrinker möchte, dass der Kaffee nicht ausgegeben wird,
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
