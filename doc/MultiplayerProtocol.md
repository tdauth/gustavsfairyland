Gustavs Märchenwelt Netzwerk-Protokoll:
- QtNetwork mit High Level API verwenden
- https://wiki.qt.io/Qt_and_Web_Services

Zur Vereinfachung:
- Maximal 2 Spieler.
- Über einen zentralen Server.
- Der Host fängt mit awaitclip an (immer der gleiche Spieler)
- Maximal 4 Spiele gleichzeitig über den Server
- Maximal 15 MiBytes große Clips versenden (Bild + Erzählersound + Video)
- Maximal alle 5 Minuten den REST-Aufruf "sendclip" bzw. der Server verarbeitet die REST-Calls aus einer Queue
heraus und immer nur in einem bestimmten Intervall bzw. muss schauen, dass nicht zu viele Calls von einem Client
gequeuet werden!

Der Server ist ein separates Programm, dass auf auf einem Server läuft, dessen Domain bei jeden Client
eingestellt werden kann:
Domain: wc3lib.org
Port: 80 (HTTP ist meist erlaubt)

Server REST-API:

list
Befehl: GET
Inhalt: Nichts
Antwort: Eine Liste aller Spiele (IDs, Namen, Passwortflag)

create - erzeugt ein Spiel
Befehl: POST
Inhalt: Spielname, Passwort (optional)
Antwort: Gibt die ID des Spiels zurück

remove - entfernt ein Spiel
Befehl: DELETE
Inhalt: Spiele-ID, Session-Key

join - man nimmt an einem Spiel teil
Befehl: GET
Inhalt: ID des Spiels, Passwort (wenn notwendig)
Antwort: Bei Erfolg: Session-Key für das Spiel. Ansonsten: Fehler

leave - man verlässt das Spiel
Befehl: GET
Inhalt: Session-Key
Antwort: Bestätigung

awaitstart - warte auf Spielstart
Befehl: GET
Inhalt: ID des Spiels.
Antwort: Sobald es los geht eine Bestätigung oder eine Abbruchnachricht bei Abbruch

Spielmodus-REST-API:
awaitclip
Befehl: GET
Inhalt: Clip-Beschreibung
Antwort: Clip-Datei mit Bild, Video und Erzähler-Sound

sendclip
Befehl: GET
Inhalt: Clip-Datei mit Bild, Video und Erzähler-Sound
Antwort: Erfolg oder kein Erfolg